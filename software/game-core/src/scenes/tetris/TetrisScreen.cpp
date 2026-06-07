#include "scenes/tetris/TetrisScreen.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "core/math/Prng.h"

#include <algorithm>

namespace handheld {

using namespace tetris::cfg;

// ── 工具函数 ──────────────────────────────────────────────

uint32_t TetrisScreen::next_rng() {
    return xorshift32(_rng);
}

bool TetrisScreen::piece_cell(uint8_t piece, uint8_t rot, int row, int col) const {
    return (PIECES[piece].rotations[rot].r[row] >> col) & 1U;
}

bool TetrisScreen::collides(int8_t x, int8_t y, uint8_t piece, uint8_t rot) const {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!piece_cell(piece, rot, r, c)) continue;
            int bx = x + c;
            int by = y + r;
            if (bx < 0 || bx >= COLS) return true;
            if (by >= ROWS) return true;
            if (by < 0) continue; // 允许部分在屏外（生成与踢墙时）
            if (_stack[by * COLS + bx] != 0) return true;
        }
    }
    return false;
}

void TetrisScreen::try_rotate() {
    uint8_t new_rot = static_cast<uint8_t>((_current_rot + 1U) & 0x3U);
    // 简化 SRS-style kick 列表：水平偏移 + 垂直偏移组合
    // 7 个候选：center / ±1 / ±2 / up1 / up1+right1（I-piece 视觉居中提示）
    const int8_t kicks[7][2] = {
        { 0,  0},
        {-1,  0},
        { 1,  0},
        {-2,  0},
        { 2,  0},
        { 0, -1},
        { 1, -1},
    };
    for (auto& k : kicks) {
        int8_t nx = static_cast<int8_t>(_current_x + k[0]);
        int8_t ny = static_cast<int8_t>(_current_y + k[1]);
        if (!collides(nx, ny, _current_piece, new_rot)) {
            _current_x = nx;
            _current_y = ny;
            _current_rot = new_rot;
            return;
        }
    }
}

void TetrisScreen::draw_cell(IDisplay& d, int col, int row, Color color) const {
    int16_t x = static_cast<int16_t>(AREA_X + col * CELL);
    int16_t y = static_cast<int16_t>(AREA_Y + row * CELL);
    // 1px 间隙作为网格线：每格 3×3 实心 + 1px 边
    d.fill_rect(Rect{static_cast<int16_t>(x + 1), static_cast<int16_t>(y + 1),
                     static_cast<int16_t>(CELL - 1), static_cast<int16_t>(CELL - 1)},
                color);
}

// ── 生命周期 ──────────────────────────────────────────────

void TetrisScreen::enter(IPlatform& platform, IScreenHost& host) {
    reset_game();
    platform.display().clear(COLOR_BG);
}

void TetrisScreen::reset_game() {
    _score = 0;
    _level = 1;
    _lines = 0;
    _paused = false;
    _game_over = false;
    _rng = 12345;
    _gravity_counter = 0;
    _gravity_interval = INIT_INTERVAL;
    for (unsigned char & i : _stack) i = 0;
    refill_bag();
    _next_piece = pop_bag();
    spawn_piece();
}

void TetrisScreen::refill_bag() {
    for (int i = 0; i < PIECE_COUNT; ++i) _bag[i] = static_cast<uint8_t>(i);
    // Fisher–Yates 洗牌
    for (int i = PIECE_COUNT - 1; i > 0; --i) {
        uint32_t r = next_rng() % static_cast<uint32_t>(i + 1);
        uint8_t tmp = _bag[i];
        _bag[i] = _bag[r];
        _bag[r] = tmp;
    }
    _bag_idx = 0;
}

uint8_t TetrisScreen::pop_bag() {
    if (_bag_idx >= PIECE_COUNT) refill_bag();
    return _bag[_bag_idx++];
}

void TetrisScreen::spawn_piece() {
    _current_piece = _next_piece;
    _next_piece = pop_bag();
    _current_rot = 0;
    _current_x = 3; // (10 - 4) / 2：4×4 旋转盒在 10 列板的中央
    _current_y = 0;
    if (collides(_current_x, _current_y, _current_piece, _current_rot)) {
        _game_over = true;
    }
}

void TetrisScreen::lock_piece(IScreenHost& host) {
    // 将方块写入堆叠（by < 0 或 by >= ROWS 的格子静默丢弃）
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!piece_cell(_current_piece, _current_rot, r, c)) continue;
            int bx = _current_x + c;
            int by = _current_y + r;
            if (bx < 0 || bx >= COLS) continue;
            if (by < 0 || by >= ROWS) continue;
            _stack[by * COLS + bx] = static_cast<uint8_t>(_current_piece + 1U);
        }
    }

    // 死亡检测：遍历前 2 行，任一格子非空即 game over
    // （覆盖 I/O piece 4×4 盒无 row 0 格子、仍能 spawn 进 row 1 空隙的 bug）
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (_stack[r * COLS + c] != 0) {
                _game_over = true;
                goto end_dead_check;
            }
        }
    }
    end_dead_check:;

    // 检查并消除满行
    int cleared = 0;
    for (int r = 0; r < ROWS; ++r) {
        bool full = true;
        for (int c = 0; c < COLS; ++c) {
            if (_stack[r * COLS + c] == 0) { full = false; break; }
        }
        if (full) {
            // 把 r 行以上的所有行下移一行
            for (int rr = r; rr > 0; --rr) {
                for (int c = 0; c < COLS; ++c) {
                    _stack[rr * COLS + c] = _stack[(rr - 1) * COLS + c];
                }
            }
            for (int c = 0; c < COLS; ++c) _stack[c] = 0;
            ++cleared;
            --r; // 同一行可能因上方下移再次满
        }
    }
    if (cleared > 0) {
        if (cleared <= 4) {
            _score = static_cast<uint16_t>(_score + SCORE_TABLE[cleared] * _level);
        }
        _lines = static_cast<uint8_t>(_lines + cleared);
        // 升级 + 加速
        _level = static_cast<uint8_t>(1 + _lines / SPEED_STEP_LINES);
        uint32_t new_interval = INIT_INTERVAL -
                                static_cast<uint32_t>(_lines / SPEED_STEP_LINES) * SPEED_DECREMENT;
        new_interval = std::max(new_interval, MIN_INTERVAL);
        _gravity_interval = new_interval;
        host.audio().play_sfx(sounds::SFX_TETRIS_CLEAR, sounds::SFX_TETRIS_CLEAR_COUNT);
    }
    spawn_piece();
    if (_game_over) {
        host.audio().play_sfx(sounds::SFX_TETRIS_GAMEOVER, sounds::SFX_TETRIS_GAMEOVER_COUNT);
    }
}

// ── 主循环 ──────────────────────────────────────────────

void TetrisScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            _paused = false;
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (_game_over) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            reset_game();
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (input.was_pressed(ButtonBits::START)) {
        _paused = true;
        return;
    }

    // 游戏中按 B：先暂停，再在暂停中按 B 才退出
    if (input.was_pressed(ButtonBits::B)) {
        _paused = true;
        return;
    }

    if (input.was_pressed(ButtonBits::UP)) {
        try_rotate();
    }
    if (input.was_pressed(ButtonBits::LEFT)) {
        if (!collides(static_cast<int8_t>(_current_x - 1), _current_y, _current_piece, _current_rot)) {
            _current_x = static_cast<int8_t>(_current_x - 1);
        }
    }
    if (input.was_pressed(ButtonBits::RIGHT)) {
        if (!collides(static_cast<int8_t>(_current_x + 1), _current_y, _current_piece, _current_rot)) {
            _current_x = static_cast<int8_t>(_current_x + 1);
        }
    }

    // 硬降：立刻落地并锁定
    if (input.was_pressed(ButtonBits::A)) {
        int drop = 0;
        while (!collides(_current_x, static_cast<int8_t>(_current_y + 1),
                         _current_piece, _current_rot)) {
            _current_y = static_cast<int8_t>(_current_y + 1);
            ++drop;
        }
        _score = static_cast<uint16_t>(_score + drop * 2U);
        host.audio().play_sfx(sounds::SFX_TETRIS_DROP, sounds::SFX_TETRIS_DROP_COUNT);
        lock_piece(host);
        return;
    }

    // 重力（DOWN 长按为软降，每帧 1 格）
    bool soft_drop = input.is_down(ButtonBits::DOWN);
    uint32_t interval = soft_drop ? 1U : _gravity_interval;
    ++_gravity_counter;
    if (_gravity_counter >= interval) {
        _gravity_counter = 0;
        if (collides(_current_x, static_cast<int8_t>(_current_y + 1),
                     _current_piece, _current_rot)) {
            lock_piece(host);
        } else {
            _current_y = static_cast<int8_t>(_current_y + 1);
        }
    }
}

// ── 渲染 ──────────────────────────────────────────────

void TetrisScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& d = platform.display();
    d.clear(COLOR_BG);

    // ── HUD（顶部 8px） ────────────────────────────
    // 分数
    TextRenderer::draw_text(d, {2, 2}, "S:", COLOR_HUD, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_uint(d, 10, 2, _score, COLOR_HUD, COMPACT_FONT_3X5);
    // 等级
    TextRenderer::draw_text(d, {36, 2}, "L:", COLOR_HUD, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_uint(d, 44, 2, _level, COLOR_HUD, COMPACT_FONT_3X5);
    // 消行
    TextRenderer::draw_text(d, {60, 2}, "N:", COLOR_HUD, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_uint(d, 68, 2, _lines, COLOR_HUD, COMPACT_FONT_3X5);

    // ── 网格线（淡色背景格） ──────────────────────
    for (int r = 0; r <= ROWS; ++r) {
        d.draw_h_line(AREA_X, static_cast<int16_t>(AREA_Y + r * CELL), AREA_W, COLOR_GRID);
    }
    for (int c = 0; c <= COLS; ++c) {
        d.draw_v_line(static_cast<int16_t>(AREA_X + c * CELL), AREA_Y, AREA_H, COLOR_GRID);
    }
    d.draw_rect(Rect{AREA_X, AREA_Y, AREA_W + 1, AREA_H + 1}, COLOR_BORDER);

    // ── 已堆叠方块 ───────────────────────────────
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            uint8_t v = _stack[r * COLS + c];
            if (v != 0) draw_cell(d, c, r, PIECES[v - 1].color);
        }
    }

    // ── 当前方块 ───────────────────────────────
    if (!_game_over) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                if (!piece_cell(_current_piece, _current_rot, r, c)) continue;
                int bx = _current_x + c;
                int by = _current_y + r;
                if (bx < 0 || bx >= COLS) continue;
                if (by < 0 || by >= ROWS) continue;
                draw_cell(d, bx, by, PIECES[_current_piece].color);
            }
        }
    }

    // ── 高度限制线（row 2 上沿，y=14，红色 1px 横线） ────────────
    d.draw_h_line(AREA_X, static_cast<int16_t>(AREA_Y + 2 * CELL), AREA_W, COLOR_HEIGHT_LIMIT);

    // ── NEXT 预览（左侧 8×8 区） ───────────────────
    TextRenderer::draw_text(d, {4, 12}, "NEXT", COLOR_HINT, 1, COMPACT_FONT_3X5);
    const int16_t prev_ox = 4;
    const int16_t prev_oy = 22;
    constexpr int16_t prev_cell = 2;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!piece_cell(_next_piece, 0, r, c)) continue;
            d.fill_rect(Rect{static_cast<int16_t>(prev_ox + c * prev_cell),
                             static_cast<int16_t>(prev_oy + r * prev_cell),
                             prev_cell, prev_cell},
                        PIECES[_next_piece].color);
        }
    }

    // ── 按键提示（右侧） ─────────────────────────
    TextRenderer::draw_text(d, {64, 12}, "KEYS", COLOR_HINT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text(d, {64, 22}, "L/R", COLOR_HINT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text(d, {64, 32}, "U", COLOR_HINT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text(d, {64, 42}, "A", COLOR_HINT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text(d, {64, 52}, "DN", COLOR_HINT, 1, COMPACT_FONT_3X5);

    // ── 覆盖层 ───────────────────────────────────
    if (_paused) {
        d.fill_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, COLOR_OVERLAY);
        d.draw_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, COLOR_PAUSE);
        TextRenderer::draw_text_centered(d, {40, 28}, "PAUSED", COLOR_PAUSE, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(d, {40, 42}, "A/START: Resume", COLOR_PAUSE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "B: Menu", COLOR_HINT, 1, COMPACT_FONT_3X5);
    }
    if (_game_over) {
        d.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, COLOR_OVERLAY);
        d.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, COLOR_GAMEOVER);
        TextRenderer::draw_text_centered(d, {40, 22}, "GAME OVER", COLOR_GAMEOVER, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text(d, {10, 34}, "SCORE ", COLOR_HUD, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_uint(d, 34, 34, _score, COLOR_HUD, COMPACT_FONT_3X5);
        TextRenderer::draw_text(d, {6, 42}, "LV ", COLOR_HUD, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_uint(d, 18, 42, _level, COLOR_HUD, COMPACT_FONT_3X5);
        TextRenderer::draw_text(d, {30, 42}, "  N ", COLOR_HUD, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_uint(d, 48, 42, _lines, COLOR_HUD, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 50}, "A/START: Again", COLOR_HINT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 58}, "B: Menu", COLOR_HINT, 1, COMPACT_FONT_3X5);
    }
}

// ── 菜单预览（左侧 10×10 方框内绘制 3×3 迷你 T 块）──────

void TetrisScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t /*frame*/) {
    const int16_t ox = static_cast<int16_t>(box.x + 2);
    const int16_t oy = static_cast<int16_t>(box.y + 2);
    // T 块 R0 = {0x2, 0x7, 0x0, 0x0}（顶行 col=1，第二行 col=0..2）
    constexpr uint8_t t_piece = 2; // PIECES 数组中 T 的索引
    constexpr int cell = 2;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (((PIECES[t_piece].rotations[0].r[r] >> c) & 1U) == 0) continue;
            display.fill_rect(Rect{static_cast<int16_t>(ox + c * cell),
                                   static_cast<int16_t>(oy + r * cell), cell, cell},
                              COLOR_T);
        }
    }
}

} // namespace handheld
