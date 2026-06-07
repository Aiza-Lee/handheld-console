#include "scenes/breakout/BreakoutScreen.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include <algorithm>

namespace handheld {

using namespace breakout::cfg;
namespace bg = breakout;

// ── RNG ─────────────────────────────────────────

uint32_t BreakoutScreen::next_rng() {
    _rng ^= _rng << 13U;
    _rng ^= _rng >> 17U;
    _rng ^= _rng << 5U;
    return _rng;
}

// ── 生命周期 ────────────────────────────────────

void BreakoutScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG_COLOR);
    _level = 0;
    reset_game();
}

void BreakoutScreen::reset_game() {
    _paddle_x = 32;
    _score = 0;
    _frame = 0;
    _rng = 12345;
    _celebration = 0;
    _paused = false;
    for (auto& b : _balls) b.active = false;
    _ball_count = 0;
    for (auto& p : _powerups) p.active = false;
    _particles.clear();
    load_level(_level);
}

void BreakoutScreen::load_level(uint8_t level) {
    if (level >= LEVEL_COUNT) level = 0;
    _grid.load(*LEVELS[level]);

    for (auto& b : _balls) b.active = false;
    _ball_count = 1;
    _balls[0] = {static_cast<int8_t>(_paddle_x + PADDLE_W / 2), static_cast<int8_t>(PADDLE_Y - 1), 0, 0, 0, 0, true};

    for (auto& p : _powerups) p.active = false;
    _particles.clear();
    _state = State::ATTACHED;
}

// ── 球 ──────────────────────────────────────────

void BreakoutScreen::launch_ball() {
    auto& b = _balls[0];
    b.vx = (next_rng() & 1U) ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
    b.vy = -2;
    _state = State::ACTIVE;
}

void BreakoutScreen::spawn_triple(int8_t x, int8_t y) {
    int16_t slots[3] = {-1, -1, -1};
    int16_t found = 0;
    for (int16_t i = 0; i < MAX_BALLS && found < 3; ++i)
        if (!_balls[i].active) {
            slots[found] = i;
            ++found;
        }
    if (found < 3) return;

    const int8_t vx_list[3] = {-1, 0, 1};
    for (int i = 0; i < 3; ++i) {
        auto& b = _balls[slots[i]];
        b = {x, y, vx_list[i], -2, x, y, true};
    }
    _ball_count += 3;
}

void BreakoutScreen::split_balls() {
    if (_ball_count <= 0 || _ball_count >= MAX_BALLS) return;
    int16_t room = MAX_BALLS - _ball_count;
    int16_t to_clone = (_ball_count <= room) ? _ball_count : room;

    int16_t next_slot = 0;
    for (int16_t i = 0; i < MAX_BALLS && to_clone > 0; ++i) {
        if (!_balls[i].active) continue;
        int16_t slot = -1;
        for (int16_t s = next_slot; s < MAX_BALLS; ++s) {
            if (!_balls[s].active) { slot = s; break; }
        }
        if (slot < 0) break;
        auto& orig = _balls[i];
        auto& clone = _balls[slot];
        clone = {orig.x, orig.y, static_cast<int8_t>(-orig.vx), orig.vy,
                 orig.x, orig.y, true};
        ++_ball_count;
        --to_clone;
        next_slot = slot + 1;
    }
}

// ── 碰撞：单步移动球，处理墙体 / 砖块 / 挡板 ────

bool BreakoutScreen::step_ball(Ball& b, IScreenHost& host) {
    int8_t abs_vx = (b.vx > 0) ? b.vx : static_cast<int8_t>(-b.vx);
    int8_t abs_vy = (b.vy > 0) ? b.vy : static_cast<int8_t>(-b.vy);

    // X 方向逐像素移动
    int8_t sx = (b.vx > 0) ? 1 : -1;
    for (int8_t s = 0; s < abs_vx; ++s) {
        int8_t nx = b.x + sx;
        if (nx < 0 || nx >= 80) {
            b.vx = -b.vx;
            host.audio().play_sfx(sounds::SFX_WALL_BOUNCE, sounds::SFX_WALL_BOUNCE_COUNT);
            break;
        }

        uint8_t tile = _grid.hit_test(nx, b.y);
        if (tile != 0) {
            b.vx = -b.vx;
            // 将球推到砖块外侧
            int16_t col = (nx - bg::BrickGrid::OX) / bg::BrickGrid::CELL;
            int16_t row = (b.y - bg::BrickGrid::OY) / bg::BrickGrid::CELL;
            auto br = bg::BrickGrid::brick_rect(row, col);
            b.x = (sx > 0) ? static_cast<int8_t>(br.x - 1) : static_cast<int8_t>(br.x + br.width);
            // 摧毁砖块
            if (tile >= 1 && tile <= 5) {
                _score += _grid.destroy(row, col);
                spawn_particles(row, col, tile);
                host.audio().play_sfx(sounds::SFX_BRICK, sounds::SFX_BRICK_COUNT);
                spawn_powerup(row, col);
            } else {
                host.audio().play_sfx(sounds::SFX_INDESTRUCTIBLE, sounds::SFX_INDESTRUCTIBLE_COUNT);
            }
            break;
        }
        b.x = nx;
    }

    // Y 方向逐像素移动
    int8_t sy = (b.vy > 0) ? 1 : -1;
    for (int8_t s = 0; s < abs_vy; ++s) {
        int8_t ny = b.y + sy;
        if (ny < 0) {
            b.vy = -b.vy;
            host.audio().play_sfx(sounds::SFX_WALL_BOUNCE, sounds::SFX_WALL_BOUNCE_COUNT);
            break;
        }
        if (ny >= 80) {
            b.active = false;
            host.audio().play_sfx(sounds::SFX_BALL_LOST, sounds::SFX_BALL_LOST_COUNT);
            return true;
        } // 掉出屏幕

        uint8_t tile = _grid.hit_test(b.x, ny);
        if (tile != 0) {
            b.vy = -b.vy;
            int16_t col = (b.x - bg::BrickGrid::OX) / bg::BrickGrid::CELL;
            int16_t row = (ny - bg::BrickGrid::OY) / bg::BrickGrid::CELL;
            auto br = bg::BrickGrid::brick_rect(row, col);
            b.y = (sy > 0) ? static_cast<int8_t>(br.y - 1) : static_cast<int8_t>(br.y + br.height);
            if (tile >= 1 && tile <= 5) {
                _score += _grid.destroy(row, col);
                spawn_particles(row, col, tile);
                host.audio().play_sfx(sounds::SFX_BRICK, sounds::SFX_BRICK_COUNT);
                spawn_powerup(row, col);
            } else {
                host.audio().play_sfx(sounds::SFX_INDESTRUCTIBLE, sounds::SFX_INDESTRUCTIBLE_COUNT);
            }
            break;
        }

        // 挡板碰撞（仅下降时检测）
        if (ny >= PADDLE_Y && sy > 0 && b.x >= _paddle_x && b.x < _paddle_x + PADDLE_W) {
            b.y = PADDLE_Y - 1;
            int16_t off = b.x - (_paddle_x + PADDLE_W / 2);
            for (int z = 0; z < 5; ++z) {
                if (off <= PADDLE_ZONES[z][0]) {
                    b.vx = static_cast<int8_t>(PADDLE_ZONES[z][1]);
                    b.vy = PADDLE_ZONE_VY[z];
                    break;
                }
            }
            host.audio().play_sfx(sounds::SFX_PADDLE, sounds::SFX_PADDLE_COUNT);
            break;
        }
        b.y = ny;
    }
    return false;
}

void BreakoutScreen::move_balls(IScreenHost& host) {
    for (int16_t i = 0; i < MAX_BALLS; ++i) {
        if (!_balls[i].active) continue;
        auto& b = _balls[i];
        b.px = b.x;
        b.py = b.y;
        if (step_ball(b, host)) --_ball_count;
    }
}

// ── 道具 ────────────────────────────────────────

void BreakoutScreen::spawn_powerup(int16_t row, int16_t col) {
    if ((next_rng() & 0xFF) >= POWERUP_DROP_CHANCE) return;
    for (auto& p : _powerups) {
        if (!p.active) {
            p.x = static_cast<int8_t>(bg::BrickGrid::OX + col * bg::BrickGrid::CELL + bg::BrickGrid::SIZE / 2);
            p.y = static_cast<int8_t>(bg::BrickGrid::OY + row * bg::BrickGrid::CELL + bg::BrickGrid::SIZE / 2);
            p.type = (next_rng() & 1U) ? POWERUP_TYPE_SPLIT : POWERUP_TYPE_TRIPLE;
            p.active = true;
            return;
        }
    }
}

void BreakoutScreen::move_powerups() {
    for (auto& p : _powerups) {
        if (!p.active) continue;
        p.y += POWERUP_VY;
        if (p.y >= 80) p.active = false;
    }
}

void BreakoutScreen::collect_powerups(IScreenHost& host) {
    for (auto& p : _powerups) {
        if (!p.active) continue;
        if (p.y + POWERUP_SIZE >= PADDLE_Y && p.y < PADDLE_Y + PADDLE_H && p.x + POWERUP_SIZE > _paddle_x &&
            p.x < _paddle_x + PADDLE_W) {
            p.active = false;
            host.audio().play_sfx(sounds::SFX_POWERUP, sounds::SFX_POWERUP_COUNT);
            int8_t cx = static_cast<int8_t>(_paddle_x + PADDLE_W / 2);
            int8_t cy = static_cast<int8_t>(PADDLE_Y - 5);
            if (p.type == POWERUP_TYPE_TRIPLE) spawn_triple(cx, cy);
            else split_balls();
        }
    }
}

// ── 粒子 ────────────────────────────────────────

void BreakoutScreen::spawn_particles(int16_t row, int16_t col, uint8_t brick_type) {
    int16_t cx = bg::BrickGrid::OX + col * bg::BrickGrid::CELL + bg::BrickGrid::SIZE / 2;
    int16_t cy = bg::BrickGrid::OY + row * bg::BrickGrid::CELL + bg::BrickGrid::SIZE / 2;
    Color color = BRICK_COLORS[brick_type - 1];
    for (int16_t i = 0; i < 3; ++i) {
        int8_t vx = static_cast<int8_t>((next_rng() % 3) - 1);
        int8_t vy = static_cast<int8_t>(-1 - static_cast<int16_t>(next_rng() % 2));
        _particles.emit(static_cast<int8_t>(cx), static_cast<int8_t>(cy), vx, vy, 6, color);
    }
}

// ── 渲染：砖块 ──────────────────────────────────

void BreakoutScreen::draw_bricks(IDisplay& display) const {
    for (int16_t r = 0; r < bg::BrickGrid::ROWS; ++r) {
        for (int16_t c = 0; c < bg::BrickGrid::COLS; ++c) {
            uint8_t t = _grid.row_data(r)[c];
            if (t == 0) continue;
            auto br = bg::BrickGrid::brick_rect(r, c);
            if (t == bg::BrickGrid::INDESTRUCTIBLE) {
                display.fill_rect(br, INDESTRUCTIBLE_COLOR);
                display.draw_pixel(br.x, br.y, INDESTRUCTIBLE_EDGE);
                display.draw_pixel(static_cast<int16_t>(br.x + br.width - 1), br.y, INDESTRUCTIBLE_EDGE);
            } else {
                display.fill_rect(br, BRICK_COLORS[t - 1]);
            }
        }
    }
}

// ── 渲染：状态栏 ────────────────────────────────

void BreakoutScreen::draw_status_bar(IDisplay& display) const {
    TextRenderer::draw_text(display, {2, 0}, "LV", Color::WHITE, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_int(display, 10, 0, _level + 1, Color::WHITE, COMPACT_FONT_3X5);
    TextRenderer::draw_text(display, {18, 0}, " ", Color::WHITE, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_int(display, 22, 0, _score, Color::WHITE, COMPACT_FONT_3X5);
    Color ball_color = (_ball_count > 3) ? TRIPLE_COLOR : Color::WHITE;
    TextRenderer::draw_int(display, 72, 0, _ball_count, ball_color, COMPACT_FONT_3X5);
}

// ── 渲染：覆盖层（最后绘制，盖住所有游戏元素）───

void BreakoutScreen::draw_overlay(IDisplay& display) const {
    if (_state == State::GAME_OVER) {
        bool won = (_celebration > 0);
        Color c = won ? WIN_COLOR : BRICK_COLORS[0];
        display.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, BG_COLOR);
        display.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, c);
        TextRenderer::draw_text_centered(display, {40, 28}, won ? "YOU WIN!" : "GAME OVER", c, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text(display, {20, 42}, "SC:", Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_int(display, 32, 42, _score, Color::WHITE, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 50}, "A/START: Again", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 58}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    if (_paused) {
        display.fill_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BG);
        display.draw_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PADDLE_COLOR);
        TextRenderer::draw_text_centered(display, {40, 28}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "A/START: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

// ── 主循环 ──────────────────────────────────────

void BreakoutScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();
    _particles.update();
    if (_celebration > 0) --_celebration;

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

    if (_state == State::GAME_OVER) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            _level = 0;
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

    // 挡板移动
    if (input.is_down(ButtonBits::LEFT)) _paddle_x -= PADDLE_SPEED;
    if (input.is_down(ButtonBits::RIGHT)) _paddle_x += PADDLE_SPEED;
    _paddle_x = std::max<int16_t>(_paddle_x, 0);
    if (_paddle_x + PADDLE_W > 80) _paddle_x = static_cast<int16_t>(80 - PADDLE_W);

    if (_state == State::ATTACHED) {
        _balls[0].x = static_cast<int8_t>(_paddle_x + PADDLE_W / 2);
        _balls[0].y = static_cast<int8_t>(PADDLE_Y - 1);
        _balls[0].px = _balls[0].x;
        _balls[0].py = _balls[0].y;
        if (input.was_pressed(ButtonBits::A)) launch_ball();
        return;
    }

    move_balls(host);
    move_powerups();

    if (_ball_count == 0) {
        _state = State::GAME_OVER;
        host.audio().play_sfx(sounds::SFX_DEATH_BREAKOUT, sounds::SFX_DEATH_BREAKOUT_COUNT);
        return;
    }

    collect_powerups(host);

    if (_grid.remaining() == 0) {
        host.audio().play_sfx(sounds::SFX_LEVEL_CLEAR_BREAKOUT, sounds::SFX_LEVEL_CLEAR_BREAKOUT_COUNT);
        ++_level;
        if (_level >= LEVEL_COUNT) {
            _state = State::GAME_OVER;
            _celebration = CELEBRATION_DURATION;
            _level = 0;
        } else {
            load_level(_level);
        }
    }
}

void BreakoutScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG_COLOR);

    // 状态栏
    display.fill_rect(Rect{0, 0, 80, STATUS_H}, BAR_COLOR);
    draw_status_bar(display);

    // 砖块
    draw_bricks(display);

    // 粒子
    _particles.render(display);

    // 道具
    for (const auto& p : _powerups) {
        if (!p.active) continue;
        Color pc = (p.type == POWERUP_TYPE_TRIPLE) ? TRIPLE_COLOR : SPLIT_COLOR;
        display.fill_rect(Rect{p.x, p.y, POWERUP_SIZE, POWERUP_SIZE}, pc);
        display.draw_pixel(static_cast<int16_t>(p.x + 1), static_cast<int16_t>(p.y + 1), BG_COLOR);
        display.draw_pixel(static_cast<int16_t>(p.x + 2), static_cast<int16_t>(p.y + 1), BG_COLOR);
        display.draw_pixel(static_cast<int16_t>(p.x + 1), static_cast<int16_t>(p.y + 2), BG_COLOR);
    }

    // 球 + 拖尾
    for (const auto& b : _balls) {
        if (!b.active || b.y > 77) continue;
        if (b.px != b.x || b.py != b.y) display.draw_pixel(b.px, b.py, TRAIL_COLOR);
        display.draw_pixel(b.x, b.y, BALL_COLOR);
    }

    // 挡板
    display.fill_rect(Rect{_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);
    display.draw_h_line(_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_EDGE);

    // 庆祝粒子
    if (_celebration > 0) {
        for (uint8_t i = 0; i < 8; ++i)
            display.draw_pixel(static_cast<int16_t>(next_rng() % 80), static_cast<int16_t>(next_rng() % 80),
                               BRICK_COLORS[next_rng() % 5]);
    }

    // 覆盖层（最后绘制，盖住所有游戏元素）
    draw_overlay(display);
}

void BreakoutScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t /*frame*/) {
    const auto bx = static_cast<int16_t>(box.x + 2);
    const auto by = static_cast<int16_t>(box.y + 1);
    const Color colors[2] = {rgb565(200, 60, 60), rgb565(190, 180, 40)};
    for (int row = 0; row < 2; ++row)
        for (int col = 0; col < 3; ++col)
            for (int px = 0; px < 2; ++px)
                display.draw_pixel(static_cast<int16_t>(bx + col * 2 + px), static_cast<int16_t>(by + row * 3),
                                   colors[row]);
    display.draw_pixel(static_cast<int16_t>(bx + 4), static_cast<int16_t>(by + 6), rgb565(220, 240, 255));
}
} // namespace handheld
