#include "scenes/breakout/BreakoutScreen.h"
#include "scenes/breakout/BreakoutConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/audio/AudioMixer.h"
#include "core/runtime/ScreenType.h"
#include <algorithm>
#include <cstdio>

extern "C" [[gnu::weak]] const handheld::Tone _sound_BGM_BREAKOUT[];
extern "C" [[gnu::weak]] const uint32_t _sound_BGM_BREAKOUT_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_BRICK[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_BRICK_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_PADDLE[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_PADDLE_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_DEATH_BREAKOUT[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_DEATH_BREAKOUT_count;

namespace handheld {

using namespace breakout::cfg;

// ── 瓦片缓存 ──────────────────────────────────

bool BreakoutScreen::tile_at(int8_t x, int8_t y) const {
    int16_t cx = static_cast<int16_t>(x) - BRICK_OX;
    int16_t cy = static_cast<int16_t>(y) - BRICK_OY;
    if (cx < 0 || cx >= TILE_W || cy < 0 || cy >= TILE_H) return false;
    return _tile[cy][cx] != TILE_EMPTY;
}

void BreakoutScreen::build_tile_cache() {
    for (int16_t y = 0; y < TILE_H; ++y)
        for (int16_t x = 0; x < TILE_W; ++x)
            _tile[y][x] = TILE_EMPTY;

    for (int16_t r = 0; r < MAX_BRICK_ROWS; ++r) {
        for (int16_t c = 0; c < BRICK_COLS; ++c) {
            uint8_t t = _bricks[r][c];
            if (t == 0) continue;
            int16_t bx = c * BRICK_CELL;
            int16_t by = r * BRICK_CELL;
            for (int16_t dy = 0; dy < BRICK_SIZE; ++dy)
                for (int16_t dx = 0; dx < BRICK_SIZE; ++dx)
                    _tile[by + dy][bx + dx] = t;
        }
    }
}

static void pixel_to_brick(int16_t px, int16_t py, int16_t& row, int16_t& col) {
    col = (px - BRICK_OX) / BRICK_CELL;
    row = (py - BRICK_OY) / BRICK_CELL;
    if (col < 0) col = 0;
    if (col >= BRICK_COLS) col = BRICK_COLS - 1;
    if (row < 0) row = 0;
    if (row >= MAX_BRICK_ROWS) row = MAX_BRICK_ROWS - 1;
}

// ── 生命周期 ──────────────────────────────────

void BreakoutScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG_COLOR);
    _level = 0;
    reset_game();
    if (_sound_BGM_BREAKOUT) host.mixer().set_bgm(_sound_BGM_BREAKOUT, _sound_BGM_BREAKOUT_count);
}

uint32_t BreakoutScreen::next_rng() {
    _rng ^= _rng << 13U; _rng ^= _rng >> 17U; _rng ^= _rng << 5U;
    return _rng;
}

// ── 关卡加载 ──────────────────────────────────

void BreakoutScreen::load_level(uint8_t level) {
    if (level >= LEVEL_COUNT) level = 0;
    const auto& src = *LEVELS[level];

    _bricks_remaining = 0;
    for (int16_t r = 0; r < MAX_BRICK_ROWS; ++r) {
        for (int16_t c = 0; c < BRICK_COLS; ++c) {
            uint8_t t = src[r][c];
            _bricks[r][c] = t;
            if (t >= 1 && t <= 5) ++_bricks_remaining;
        }
    }

    build_tile_cache();

    for (auto& b : _balls) b.active = false;
    _ball_count = 1;
    _balls[0].active = true;
    _balls[0].x = static_cast<int8_t>(_paddle_x + PADDLE_W / 2);
    _balls[0].y = static_cast<int8_t>(PADDLE_Y - 1);
    _balls[0].vx = 0;
    _balls[0].vy = 0;
    _balls[0].px = _balls[0].x;
    _balls[0].py = _balls[0].y;

    for (auto& p : _powerups) p.active = false;
    _particles.clear();
    _state = State::ATTACHED;
}

void BreakoutScreen::reset_game() {
    _paddle_x = 32;
    _score = 0;
    _frame = 0;
    _rng = 12345;
    _celebration_timer = 0;
    for (auto& b : _balls) b.active = false;
    _ball_count = 0;
    for (auto& p : _powerups) p.active = false;
    _particles.clear();
    load_level(_level);
}

// ── 球的操作 ──────────────────────────────────

void BreakoutScreen::launch_ball() {
    auto& b = _balls[0];
    b.vx = (next_rng() % 2 == 0) ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
    b.vy = -2;
    _state = State::ACTIVE;
}

void BreakoutScreen::spawn_triple_balls(int8_t x, int8_t y) {
    int16_t slots[3] = {-1, -1, -1};
    int16_t found = 0;
    for (int16_t i = 0; i < MAX_BALLS && found < 3; ++i)
        if (!_balls[i].active) { slots[found] = i; ++found; }
    if (found < 3) return;

    const int8_t vx_list[3] = {-1, 0, 1};
    for (int i = 0; i < 3; ++i) {
        auto& b = _balls[slots[i]];
        b.active = true;
        b.x = x; b.px = x;
        b.y = y; b.py = y;
        b.vx = vx_list[i];
        b.vy = -2;
    }
    _ball_count += 3;
}

void BreakoutScreen::split_balls() {
    int16_t active_indices[MAX_BALLS];
    int16_t active_count = 0;
    for (int16_t i = 0; i < MAX_BALLS; ++i)
        if (_balls[i].active) active_indices[active_count++] = i;

    for (int16_t i = 0; i < active_count - 1; ++i)
        for (int16_t j = i + 1; j < active_count; ++j)
            if (_balls[active_indices[i]].y > _balls[active_indices[j]].y) {
                int16_t tmp = active_indices[i];
                active_indices[i] = active_indices[j];
                active_indices[j] = tmp;
            }

    int16_t can_add = MAX_BALLS - _ball_count;
    for (int16_t i = 0; i < active_count && can_add > 0; ++i) {
        int16_t idx = active_indices[i];
        if (!_balls[idx].active) continue;
        if (_ball_count >= SPLIT_SOFT_CAP && _balls[idx].y > 26) continue;

        int16_t slot = -1;
        for (int16_t s = 0; s < MAX_BALLS; ++s)
            if (!_balls[s].active) { slot = s; break; }
        if (slot < 0) break;

        auto& orig = _balls[idx];
        auto& clone = _balls[slot];
        clone.active = true;
        clone.x = orig.x; clone.px = orig.x;
        clone.y = orig.y; clone.py = orig.y;
        clone.vy = orig.vy;
        clone.vx = static_cast<int8_t>(-orig.vx);
        ++_ball_count;
        --can_add;
    }
}

// ── 碰撞检测 ──────────────────────────────────

bool BreakoutScreen::check_tile_collision(int8_t test_x, int8_t test_y, Ball& ball, IScreenHost& host) {
    if (!tile_at(test_x, test_y)) return false;

    int16_t row, col;
    pixel_to_brick(test_x, test_y, row, col);
    int16_t brx = BRICK_OX + col * BRICK_CELL;
    int16_t bry = BRICK_OY + row * BRICK_CELL;
    int16_t br_right = brx + BRICK_SIZE - 1;
    int16_t br_bottom = bry + BRICK_SIZE - 1;

    int16_t ov_left = test_x - brx;
    int16_t ov_right = br_right - test_x;
    int16_t ov_top = test_y - bry;
    int16_t ov_bottom = br_bottom - test_y;

    int16_t min_ov = ov_left;
    bool hit_x = true;
    if (ov_right < min_ov) { min_ov = ov_right; hit_x = true; }
    if (ov_top < min_ov)    { min_ov = ov_top;    hit_x = false; }
    if (ov_bottom < min_ov) { min_ov = ov_bottom; hit_x = false; }

    uint8_t t = _tile[test_y - BRICK_OY][test_x - BRICK_OX];

    if (hit_x) {
        ball.vx = static_cast<int8_t>(-ball.vx);
        if (ov_left < ov_right) ball.x = static_cast<int8_t>(brx - 1);
        else                    ball.x = static_cast<int8_t>(br_right + 1);
    } else {
        ball.vy = static_cast<int8_t>(-ball.vy);
        if (ov_top < ov_bottom) ball.y = static_cast<int8_t>(bry - 1);
        else                    ball.y = static_cast<int8_t>(br_bottom + 1);
    }

    // 墙角修复
    if (tile_at(ball.x, ball.y)) {
        if (hit_x) {
            if (ov_top < ov_bottom) ball.y = static_cast<int8_t>(bry - 1);
            else                    ball.y = static_cast<int8_t>(br_bottom + 1);
        } else {
            if (ov_left < ov_right) ball.x = static_cast<int8_t>(brx - 1);
            else                    ball.x = static_cast<int8_t>(br_right + 1);
        }
    }

    if (t != TILE_INDESTRUCTIBLE) destroy_brick(row, col, t, host);
    return true;
}

void BreakoutScreen::destroy_brick(int16_t row, int16_t col, uint8_t brick_type, IScreenHost& host) {
    if (brick_type < 1 || brick_type > 5) return;
    _bricks[row][col] = 0;
    --_bricks_remaining;
    _score += (6 - brick_type) * 10;

    int16_t bx = col * BRICK_CELL;
    int16_t by = row * BRICK_CELL;
    for (int16_t dy = 0; dy < BRICK_SIZE; ++dy)
        for (int16_t dx = 0; dx < BRICK_SIZE; ++dx)
            _tile[by + dy][bx + dx] = TILE_EMPTY;

    spawn_brick_particles(row, col, brick_type);
    if (_sound_SFX_BRICK) host.mixer().play_sfx(_sound_SFX_BRICK, _sound_SFX_BRICK_count);
    try_spawn_powerup(row, col);
}

// ── 粒子 ── 限制数量 + 缩小范围 ──────────────

void BreakoutScreen::spawn_brick_particles(int16_t row, int16_t col, uint8_t brick_type) {
    int16_t cx = BRICK_OX + col * BRICK_CELL + BRICK_SIZE / 2;
    int16_t cy = BRICK_OY + row * BRICK_CELL + BRICK_SIZE / 2;
    Color color = BRICK_COLORS[brick_type - 1];

    // 仅 3 颗粒子，小速度，短生命
    for (int16_t i = 0; i < 3; ++i) {
        int8_t vx = static_cast<int8_t>((static_cast<int16_t>(next_rng() % 3) - 1));
        int8_t vy = static_cast<int8_t>(-1 - static_cast<int16_t>(next_rng() % 2));
        _particles.emit(static_cast<int8_t>(cx), static_cast<int8_t>(cy), vx, vy, 6, color);
    }
}

// ── 移动 ──────────────────────────────────────

void BreakoutScreen::move_balls(IScreenHost& host) {
    for (int16_t i = 0; i < MAX_BALLS; ++i) {
        if (!_balls[i].active) continue;
        auto& b = _balls[i];
        b.px = b.x;
        b.py = b.y;

        int8_t sx = (b.vx > 0) ? 1 : -1;
        int8_t steps_x = (b.vx > 0) ? b.vx : static_cast<int8_t>(-b.vx);
        for (int8_t s = 0; s < steps_x; ++s) {
            int8_t nx = static_cast<int8_t>(b.x + sx);
            if (nx < 0 || nx >= 80) { b.vx = static_cast<int8_t>(-b.vx); break; }
            if (tile_at(nx, b.y)) { check_tile_collision(nx, b.y, b, host); break; }
            b.x = nx;
        }

        int8_t sy = (b.vy > 0) ? 1 : -1;
        int8_t steps_y = (b.vy > 0) ? b.vy : static_cast<int8_t>(-b.vy);
        for (int8_t s = 0; s < steps_y; ++s) {
            int8_t ny = static_cast<int8_t>(b.y + sy);
            if (ny < 0) { b.vy = static_cast<int8_t>(-b.vy); break; }
            if (ny >= 80) { b.active = false; --_ball_count; break; }
            if (tile_at(b.x, ny)) { check_tile_collision(b.x, ny, b, host); break; }
            b.y = ny;
        }
    }
}

void BreakoutScreen::move_powerups() {
    for (auto& p : _powerups) {
        if (!p.active) continue;
        p.y = static_cast<int8_t>(p.y + POWERUP_VY);
        if (p.y >= 80) p.active = false;
    }
}

// ── 挡板碰撞 ──────────────────────────────────

void BreakoutScreen::check_paddle_collisions(IScreenHost& host) {
    for (int16_t i = 0; i < MAX_BALLS; ++i) {
        if (!_balls[i].active) continue;
        auto& b = _balls[i];
        if (b.y >= PADDLE_Y && b.y < PADDLE_Y + PADDLE_H &&
            b.x >= _paddle_x && b.x < _paddle_x + PADDLE_W) {
            b.y = static_cast<int8_t>(PADDLE_Y - 1);
            int16_t off = static_cast<int16_t>(b.x) - (_paddle_x + PADDLE_W / 2);
            for (int z = 0; z < 5; ++z) {
                if (off <= PADDLE_ZONES[z][0]) {
                    b.vx = static_cast<int8_t>(PADDLE_ZONES[z][1]);
                    b.vy = PADDLE_ZONE_VY[z];
                    break;
                }
            }
            if (_sound_SFX_PADDLE) host.mixer().play_sfx(_sound_SFX_PADDLE, _sound_SFX_PADDLE_count);
        }
    }
}

// ── 道具 ──────────────────────────────────────

void BreakoutScreen::try_spawn_powerup(int16_t row, int16_t col) {
    if ((next_rng() & 0xFF) >= POWERUP_DROP_CHANCE) return;
    for (auto& p : _powerups) {
        if (!p.active) {
            int16_t px = BRICK_OX + col * BRICK_CELL + BRICK_SIZE / 2;
            int16_t py = BRICK_OY + row * BRICK_CELL + BRICK_SIZE / 2;
            p.active = true;
            p.x = static_cast<int8_t>(px);
            p.y = static_cast<int8_t>(py);
            p.type = (next_rng() % 2 == 0) ? POWERUP_TYPE_TRIPLE : POWERUP_TYPE_SPLIT;
            return;
        }
    }
}

void BreakoutScreen::check_powerup_collection() {
    for (auto& p : _powerups) {
        if (!p.active) continue;
        if (p.y + POWERUP_SIZE >= PADDLE_Y && p.y < PADDLE_Y + PADDLE_H &&
            p.x + POWERUP_SIZE > _paddle_x && p.x < _paddle_x + PADDLE_W) {
            p.active = false;
            if (p.type == POWERUP_TYPE_TRIPLE)
                spawn_triple_balls(static_cast<int8_t>(_paddle_x + PADDLE_W / 2), static_cast<int8_t>(PADDLE_Y - 5));
            else
                split_balls();
        }
    }
}

// ── 主循环 ────────────────────────────────────

void BreakoutScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();
    _particles.update();
    if (_celebration_timer > 0) --_celebration_timer;

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
        return;
    }

    if (_state == State::GAME_OVER) {
        if (input.was_pressed(ButtonBits::START)) { _level = 0; reset_game(); }
        if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
        return;
    }

    if ((_state == State::ATTACHED || _state == State::ACTIVE) && input.was_pressed(ButtonBits::START)) {
        _paused = true; return;
    }

    if (input.is_down(ButtonBits::LEFT))  _paddle_x -= PADDLE_SPEED;
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
        if (_sound_SFX_DEATH_BREAKOUT) host.mixer().play_sfx(_sound_SFX_DEATH_BREAKOUT, _sound_SFX_DEATH_BREAKOUT_count);
        return;
    }

    check_paddle_collisions(host);
    check_powerup_collection();

    if (_bricks_remaining == 0) {
        ++_level;
        if (_level >= LEVEL_COUNT) { _state = State::GAME_OVER; _celebration_timer = CELEBRATION_DURATION; _level = 0; }
        else load_level(_level);
    }
}

// ── 渲染 ──────────────────────────────────────

void BreakoutScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG_COLOR);

    // 状态栏
    display.fill_rect(Rect{0, 0, 80, STATUS_H}, BAR_COLOR);
    char buf[20];
    std::snprintf(buf, sizeof(buf), "LV%d %d", _level + 1, _score);
    TextRenderer::draw_text(display, {2, 0}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
    std::snprintf(buf, sizeof(buf), "%d", _ball_count);
    TextRenderer::draw_text(display, {72, 0}, buf, _ball_count > 3 ? TRIPLE_COLOR : Color::WHITE, 1, COMPACT_FONT_3X5);

    // 砖块
    for (int16_t r = 0; r < MAX_BRICK_ROWS; ++r) {
        for (int16_t c = 0; c < BRICK_COLS; ++c) {
            uint8_t t = _bricks[r][c];
            if (t == 0) continue;
            int16_t bx = BRICK_OX + c * BRICK_CELL;
            int16_t by = BRICK_OY + r * BRICK_CELL;
            if (t == 9) {
                display.fill_rect(Rect{bx, by, BRICK_SIZE, BRICK_SIZE}, INDESTRUCTIBLE_COLOR);
                display.draw_pixel(bx, by, INDESTRUCTIBLE_EDGE);
                display.draw_pixel(static_cast<int16_t>(bx + BRICK_SIZE - 1), by, INDESTRUCTIBLE_EDGE);
            } else if (t >= 1 && t <= 5) {
                display.fill_rect(Rect{bx, by, BRICK_SIZE, BRICK_SIZE}, BRICK_COLORS[t - 1]);
            }
        }
    }

    // 粒子（砖块碎片）
    _particles.render(display);

    // 道具
    for (const auto& p : _powerups) {
        if (!p.active) continue;
        Color pc = (p.type == POWERUP_TYPE_TRIPLE) ? TRIPLE_COLOR : SPLIT_COLOR;
        display.fill_rect(Rect{p.x, p.y, POWERUP_SIZE, POWERUP_SIZE}, pc);
        if (p.type == POWERUP_TYPE_TRIPLE) {
            display.draw_pixel(static_cast<int16_t>(p.x + 1), static_cast<int16_t>(p.y + 1), BG_COLOR);
            display.draw_pixel(static_cast<int16_t>(p.x + 2), static_cast<int16_t>(p.y + 1), BG_COLOR);
            display.draw_pixel(static_cast<int16_t>(p.x + 1), static_cast<int16_t>(p.y + 2), BG_COLOR);
        } else {
            display.draw_pixel(static_cast<int16_t>(p.x + 1), static_cast<int16_t>(p.y + 1), BG_COLOR);
            display.draw_pixel(static_cast<int16_t>(p.x + 2), static_cast<int16_t>(p.y + 2), BG_COLOR);
        }
    }

    // 球 + 拖尾
    for (const auto& b : _balls) {
        if (!b.active || b.y > 77) continue;
        if (b.px != b.x || b.py != b.y)
            display.draw_pixel(b.px, b.py, TRAIL_COLOR);
        display.draw_pixel(b.x, b.y, BALL_COLOR);
    }

    // 挡板
    display.fill_rect(Rect{_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);
    display.draw_h_line(_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_EDGE);

    // 庆祝
    if (_celebration_timer > 0) {
        for (uint8_t i = 0; i < 8; ++i)
            display.draw_pixel(static_cast<int16_t>(next_rng() % 80), static_cast<int16_t>(next_rng() % 80), BRICK_COLORS[next_rng() % 5]);
    }

    // 游戏结束 UI
    if (_state == State::GAME_OVER) {
        bool won = (_celebration_timer > 0);
        display.fill_rect(Rect{8, 24, 64, 32}, BG_COLOR);
        display.draw_rect(Rect{8, 24, 64, 32}, won ? WIN_COLOR : BRICK_COLORS[0]);
        TextRenderer::draw_text_centered(display, {40, 30}, won ? "YOU WIN!" : "GAME OVER",
                                         won ? WIN_COLOR : BRICK_COLORS[0], 1, BASIC_FONT_5X7);
        std::snprintf(buf, sizeof(buf), "SC:%d", _score);
        TextRenderer::draw_text_centered(display, {40, 44}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    if (_paused) {
        display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
        display.draw_rect({10, 22, 60, 36}, PADDLE_COLOR);
        TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 54}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

}  // namespace handheld
