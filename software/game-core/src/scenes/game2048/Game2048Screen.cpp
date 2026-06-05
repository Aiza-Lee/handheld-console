#include "scenes/game2048/Game2048Screen.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/NumberRenderer.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

using namespace game2048::cfg;

namespace {

// ── 滑动方向 ────────────────────────────────────────
constexpr int8_t D_UP = 0;
constexpr int8_t D_DOWN = 1;
constexpr int8_t D_LEFT = 2;
constexpr int8_t D_RIGHT = 3;

struct SlideResult {
    bool changed = false;
    uint32_t gained = 0;
};

// 就地压缩并合并一行，reverse=true 时从高端累积。
// 规则：先压缩、再合并相邻等值、最后再次压缩补零；
// 每个 tile 在一次移动中只能参与一次合并。
SlideResult slide_line(uint16_t line[GRID], bool reverse) {
    SlideResult r;
    uint16_t buf[GRID];
    for (int8_t i = 0; i < GRID; ++i) buf[i] = line[i];

    uint16_t out[GRID] = {0, 0, 0, 0};
    int8_t in_pos = 0;
    int8_t out_pos = 0;
    while (in_pos < GRID) {
        if (buf[in_pos] == 0) {
            ++in_pos;
            continue;
        }
        int8_t next = static_cast<int8_t>(in_pos + 1);
        while (next < GRID && buf[next] == 0) ++next;
        if (next < GRID && buf[next] == buf[in_pos]) {
            uint16_t merged = static_cast<uint16_t>(buf[in_pos] * 2);
            if (reverse) {
                out[GRID - 1 - out_pos] = merged;
            } else {
                out[out_pos] = merged;
            }
            r.gained += merged;
            ++out_pos;
            in_pos = static_cast<int8_t>(next + 1);
        } else {
            if (reverse) {
                out[GRID - 1 - out_pos] = buf[in_pos];
            } else {
                out[out_pos] = buf[in_pos];
            }
            ++out_pos;
            in_pos = next;
        }
    }

    for (int8_t i = 0; i < GRID; ++i) {
        if (line[i] != out[i]) {
            r.changed = true;
            line[i] = out[i];
        }
    }
    return r;
}

} // namespace

// ── 生命周期 ────────────────────────────────────────

void Game2048Screen::enter(IPlatform& platform, IScreenHost& host) {
    reset_game();
    platform.display().clear(BG_COLOR);
    if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_2048, sounds::BGM_2048_COUNT);
}

void Game2048Screen::reset_game() {
    for (int8_t r = 0; r < GRID; ++r)
        for (int8_t c = 0; c < GRID; ++c) _board[r][c] = 0;
    _score = 0;
    _rng = RNG_SEED;
    _game_over = false;
    _won = false;
    _paused = false;
    _keep_playing = false;
    // 开局放 2 个 tile
    spawn_tile();
    spawn_tile();
}

uint32_t Game2048Screen::next_rng() {
    _rng ^= _rng << 13U;
    _rng ^= _rng >> 17U;
    _rng ^= _rng << 5U;
    return _rng;
}

void Game2048Screen::spawn_tile() {
    // 收集空格
    int8_t empties[GRID * GRID][2];
    uint8_t count = 0;
    for (int8_t r = 0; r < GRID; ++r) {
        for (int8_t c = 0; c < GRID; ++c) {
            if (_board[r][c] == 0) {
                empties[count][0] = r;
                empties[count][1] = c;
                ++count;
            }
        }
    }
    if (count == 0) return;

    uint8_t pick = static_cast<uint8_t>(next_rng() % count);
    int8_t rr = static_cast<int8_t>(empties[pick][0]);
    int8_t cc = static_cast<int8_t>(empties[pick][1]);
    // 90% 出 2，10% 出 4
    uint16_t val = (next_rng() % 10 == 0) ? 4 : 2;
    _board[rr][cc] = val;
}

// ── 滑动逻辑 ────────────────────────────────────────

bool Game2048Screen::slide(int8_t dir) {
    SlideResult res;
    uint16_t line[GRID];

    if (dir == D_LEFT || dir == D_RIGHT) {
        bool reverse = (dir == D_RIGHT);
        for (int8_t r = 0; r < GRID; ++r) {
            for (int8_t c = 0; c < GRID; ++c) line[c] = _board[r][c];
            SlideResult sr = slide_line(line, reverse);
            for (int8_t c = 0; c < GRID; ++c) _board[r][c] = line[c];
            res.changed = res.changed || sr.changed;
            res.gained += sr.gained;
        }
    } else {
        bool reverse = (dir == D_DOWN);
        for (int8_t c = 0; c < GRID; ++c) {
            for (int8_t r = 0; r < GRID; ++r) line[r] = _board[r][c];
            SlideResult sr = slide_line(line, reverse);
            for (int8_t r = 0; r < GRID; ++r) _board[r][c] = line[r];
            res.changed = res.changed || sr.changed;
            res.gained += sr.gained;
        }
    }

    if (res.changed) {
        _score += res.gained;
        spawn_tile();
    }
    return res.changed;
}

bool Game2048Screen::can_move() const {
    for (int8_t r = 0; r < GRID; ++r)
        for (int8_t c = 0; c < GRID; ++c)
            if (_board[r][c] == 0) return true;
    for (int8_t r = 0; r < GRID; ++r) {
        for (int8_t c = 0; c < GRID; ++c) {
            uint16_t v = _board[r][c];
            if (c + 1 < GRID && _board[r][c + 1] == v) return true;
            if (r + 1 < GRID && _board[r + 1][c] == v) return true;
        }
    }
    return false;
}

// ── 主循环 ──────────────────────────────────────────

void Game2048Screen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (_game_over) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            reset_game();
            return;
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

    int8_t dir = -1;
    if (input.was_pressed(ButtonBits::UP)) dir = D_UP;
    else if (input.was_pressed(ButtonBits::DOWN)) dir = D_DOWN;
    else if (input.was_pressed(ButtonBits::LEFT)) dir = D_LEFT;
    else if (input.was_pressed(ButtonBits::RIGHT)) dir = D_RIGHT;

    if (dir >= 0) {
        // 先计算合并值用于音效判断
        uint32_t pre_score = _score;
        bool moved = slide(dir);
        if (moved) {
            bool merged = (_score > pre_score);
            if (merged) {
                host.audio().play_sfx(sounds::SFX_2048_MERGE, sounds::SFX_2048_MERGE_COUNT);
            } else {
                host.audio().play_sfx(sounds::SFX_2048_MOVE, sounds::SFX_2048_MOVE_COUNT);
            }
            // 检查胜利
            for (int8_t r = 0; r < GRID && !_won; ++r)
                for (int8_t c = 0; c < GRID && !_won; ++c)
                    if (_board[r][c] >= WIN_VALUE) _won = true;
        }
    }

    // 游戏结束判定
    if (!_game_over && !can_move()) {
        _game_over = true;
        host.audio().play_sfx(sounds::SFX_2048_GAMEOVER, sounds::SFX_2048_GAMEOVER_COUNT);
    }
}

// ── 渲染 ────────────────────────────────────────────

Color Game2048Screen::tile_color(uint16_t value) const {
    switch (value) {
        case 2: return rgb565(220, 215, 200);
        case 4: return rgb565(210, 195, 165);
        case 8: return rgb565(240, 165, 70);
        case 16: return rgb565(230, 110, 70);
        case 32: return rgb565(225, 200, 80);
        case 64: return rgb565(150, 210, 90);
        case 128: return rgb565(95, 175, 220);
        case 256: return rgb565(80, 130, 215);
        case 512: return rgb565(130, 95, 200);
        case 1024: return rgb565(180, 90, 170);
        case 2048: return rgb565(220, 80, 130);
        default: return rgb565(255, 60, 90);
    }
}

bool Game2048Screen::tile_text_dark(uint16_t value) const {
    return value <= 4;
}

void Game2048Screen::draw_tile(IDisplay& d, int8_t row, int8_t col, uint16_t value) const {
    int16_t x = static_cast<int16_t>(BOARD_X + col * CELL);
    int16_t y = static_cast<int16_t>(BOARD_Y + row * CELL);
    // 1px 间隙让 tile 看起来有分隔
    d.fill_rect(Rect{static_cast<int16_t>(x + 1), static_cast<int16_t>(y + 1),
                     static_cast<int16_t>(CELL - 2), static_cast<int16_t>(CELL - 2)},
                value == 0 ? EMPTY_TILE : tile_color(value));
    if (value != 0) {
        draw_number(d, static_cast<int16_t>(x + CELL / 2), static_cast<int16_t>(y + CELL / 2), value);
    }
}

void Game2048Screen::draw_number(IDisplay& d, int16_t cx, int16_t cy, uint16_t value) const {
    char buf[8];
    itoa_dec(value, buf, sizeof(buf));
    Color text_color = tile_text_dark(value) ? TILE_TEXT_DARK : TILE_TEXT_LIGHT;
    Size sz = TextRenderer::measure_text(buf, 1, COMPACT_FONT_3X5);
    Point origin{static_cast<int16_t>(cx - sz.width / 2), static_cast<int16_t>(cy - sz.height / 2)};
    TextRenderer::draw_text(d, origin, buf, text_color, 1, COMPACT_FONT_3X5);
}

void Game2048Screen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& d = platform.display();
    d.clear(BG_COLOR);

    // ── 顶部 HUD ────────────────────────────────────
    d.fill_rect(Rect{HUD_X, HUD_Y, HUD_W, HUD_H}, HUD_BG);
    d.draw_rect(Rect{HUD_X, HUD_Y, HUD_W, HUD_H}, BORDER_COLOR);
    char buf[20];
    buf[0] = 'S';
    buf[1] = 'C';
    buf[2] = 'O';
    buf[3] = 'R';
    buf[4] = 'E';
    buf[5] = ':';
    buf[6] = ' ';
    itoa_dec(static_cast<uint16_t>(_score > 9999 ? 9999 : _score), buf + 7, sizeof(buf) - 7);
    TextRenderer::draw_text(d, {4, 4}, buf, HUD_TEXT, 1, COMPACT_FONT_3X5);

    // 顶部右侧显示最高 tile 值
    uint16_t best = 0;
    for (int8_t r = 0; r < GRID; ++r)
        for (int8_t c = 0; c < GRID; ++c)
            if (_board[r][c] > best) best = _board[r][c];
    buf[0] = 'B';
    buf[1] = 'E';
    buf[2] = 'S';
    buf[3] = 'T';
    buf[4] = ':';
    buf[5] = ' ';
    itoa_dec(best, buf + 6, sizeof(buf) - 6);
    Size bsz = TextRenderer::measure_text(buf, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text(d,
                            Point{static_cast<int16_t>(78 - bsz.width), 4},
                            buf, HUD_TEXT, 1, COMPACT_FONT_3X5);

    // ── 棋盘背景 ───────────────────────────────────
    d.fill_rect(Rect{BOARD_X, BOARD_Y, BOARD_W, BOARD_H}, BOARD_BG);

    // ── 棋盘外框 ───────────────────────────────────
    d.draw_rect(Rect{BORDER_X, BORDER_Y, BORDER_W, BORDER_H}, BORDER_COLOR);

    // ── tile ───────────────────────────────────────
    for (int8_t r = 0; r < GRID; ++r) {
        for (int8_t c = 0; c < GRID; ++c) {
            draw_tile(d, r, c, _board[r][c]);
        }
    }

    // ── 胜利覆盖层（半透明压暗）──────────────────
    if (_won && !_keep_playing) {
        d.fill_rect(Rect{WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, OVERLAY_BG);
        d.draw_rect(Rect{WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, WIN_COLOR);
        TextRenderer::draw_text_centered(d, {40, 40}, "YOU WIN!", WIN_COLOR, 1, BASIC_FONT_5X7);
    }

    // ── 暂停覆盖层 ───────────────────────────────
    if (_paused) {
        d.fill_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BG);
        d.draw_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BORDER);
        TextRenderer::draw_text_centered(d, {40, 28}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(d, {40, 42}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    // ── 游戏结束覆盖层 ──────────────────────────
    if (_game_over) {
        d.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, OVERLAY_BG);
        d.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, GAMEOVER_COLOR);
        TextRenderer::draw_text_centered(d, {40, 28}, "GAME OVER", GAMEOVER_COLOR, 1, BASIC_FONT_5X7);
        buf[0] = 'S';
        buf[1] = 'C';
        buf[2] = 'O';
        buf[3] = 'R';
        buf[4] = 'E';
        buf[5] = ':';
        buf[6] = ' ';
        itoa_dec(static_cast<uint16_t>(_score > 9999 ? 9999 : _score), buf + 7, sizeof(buf) - 7);
        TextRenderer::draw_text_centered(d, {40, 42}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "A: New Game", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 58}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

// ── 菜单预览 ────────────────────────────────────────

void Game2048Screen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    // 2x2 静态 tile，颜色随帧循环切换
    const auto cx = static_cast<int16_t>(box.x + box.width / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    const int16_t size = 5;
    const int16_t gap = 1;
    Color palette[4] = {
        rgb565(220, 215, 200),
        rgb565(240, 165, 70),
        rgb565(95, 175, 220),
        rgb565(180, 90, 170),
    };
    for (int8_t r = 0; r < 2; ++r) {
        for (int8_t c = 0; c < 2; ++c) {
            int16_t x = static_cast<int16_t>(cx - size - gap / 2 + c * (size + gap));
            int16_t y = static_cast<int16_t>(cy - size - gap / 2 + r * (size + gap));
            uint8_t idx = static_cast<uint8_t>((r * 2 + c + (frame / 8)) % 4);
            display.fill_rect(Rect{x, y, size, size}, palette[idx]);
        }
    }
}

} // namespace handheld
