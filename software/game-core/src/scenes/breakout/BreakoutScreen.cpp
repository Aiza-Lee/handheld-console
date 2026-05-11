#include "scenes/breakout/BreakoutScreen.h"
#include "scenes/breakout/BreakoutConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include <algorithm>
#include <cstdio>

namespace handheld {

using namespace breakout::cfg;

void BreakoutScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
    reset_game();
    platform.display().clear(BG_COLOR);
}

uint32_t BreakoutScreen::next_rng() {
    _rng ^= _rng << 13U; _rng ^= _rng >> 17U; _rng ^= _rng << 5U;
    return _rng;
}

Rect BreakoutScreen::brick_rect(int16_t row, int16_t col) const {
    return {
        static_cast<int16_t>(1 + (col * (BRICK_W + BRICK_GAP_X)) + (BRICK_GAP_X / 2)),
        static_cast<int16_t>(BRICK_OY + (row * (BRICK_H + BRICK_GAP_Y))),
        BRICK_W, BRICK_H,
    };
}

void BreakoutScreen::spawn_brick_particles(int16_t row, int16_t col) {
    const Rect br = brick_rect(row, col);
    int16_t cx = static_cast<int16_t>(br.x + br.width / 2);
    int16_t cy = static_cast<int16_t>(br.y + br.height / 2);
    for (int16_t i = 0; i < 5 && _brick_particle_count < MAX_BRICK_PARTICLES; ++i) {
        uint8_t idx = _brick_particle_count++;
        _brick_particles[idx].x = static_cast<int8_t>(cx);
        _brick_particles[idx].y = static_cast<int8_t>(cy);
        _brick_particles[idx].vx = static_cast<int8_t>((next_rng() % 5) - 2);
        _brick_particles[idx].vy = static_cast<int8_t>(-1 - (next_rng() % 3));
        _brick_particles[idx].life = 8;
        _brick_particles[idx].color = PARTICLE_COLORS[row];
    }
}

void BreakoutScreen::update_particles() {
    for (uint8_t i = 0; i < _brick_particle_count; ) {
        _brick_particles[i].x += _brick_particles[i].vx;
        _brick_particles[i].y += _brick_particles[i].vy;
        if (_brick_particles[i].life == 0) {
            _brick_particles[i] = _brick_particles[_brick_particle_count - 1];
            --_brick_particle_count;
        } else { --_brick_particles[i].life; ++i; }
    }
}

void BreakoutScreen::reset_game() {
    _state = State::ATTACHED; _paused = false;
    _paddle_x = 32;
    _ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2;
    _ball_y = PADDLE_Y - BALL_H;
    _ball_vx = 0; _ball_vy = 0;
    _score = 0; _lives = INIT_LIVES;
    _dying_timer = 0; _frame = 0; _rng = 12345;
    _bricks_remaining = BRICK_ROWS * BRICKS_PER_ROW;
    _brick_particle_count = 0; _trail_index = 0;
    for (auto& t : _ball_trail) { t.x = -1; t.y = -1; }
    _hit_spark_timer = 0; _shake_timer = 0; _shake_offset = 0; _celebration_timer = 0;
    for (auto& b : _bricks) b = 0xFF;
}

void BreakoutScreen::launch_ball() {
    _ball_vx = (next_rng() % 2 == 0) ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
    _ball_vy = -2; _state = State::ACTIVE;
}

void BreakoutScreen::check_paddle_collision() {
    if (_ball_y + BALL_H >= PADDLE_Y && _ball_y < PADDLE_Y + PADDLE_H) {
        if (_ball_x + BALL_W > _paddle_x && _ball_x < _paddle_x + PADDLE_W) {
            _ball_y = PADDLE_Y - BALL_H;
            const int16_t ball_cx = _ball_x + (BALL_W / 2);
            const int16_t paddle_cx = _paddle_x + (PADDLE_W / 2);
            const int16_t off = ball_cx - paddle_cx;
            for (int i = 0; i < 5; ++i) {
                if (off <= PADDLE_ZONES[i][0]) {
                    _ball_vx = static_cast<int8_t>(PADDLE_ZONES[i][1]);
                    _ball_vy = PADDLE_ZONE_VY[i];
                    break;
                }
            }
            _hit_spark_timer = HIT_SPARK_DURATION;
            _hit_spark_x = static_cast<int16_t>(_ball_x + BALL_W / 2);
        }
    }
}

bool BreakoutScreen::check_brick_collision() {
    for (int16_t row = 0; row < BRICK_ROWS; ++row) {
        uint8_t mask = _bricks[row];
        if (mask == 0) continue;
        for (int16_t col = 0; col < BRICKS_PER_ROW; ++col) {
            if (!(mask & (1U << static_cast<unsigned>(col)))) continue;
            const Rect br = brick_rect(row, col);
            if (_ball_x + BALL_W > br.x && _ball_x < br.x + br.width &&
                _ball_y + BALL_H > br.y && _ball_y < br.y + br.height) {
                _bricks[row] = static_cast<uint8_t>(mask & ~(1U << static_cast<unsigned>(col)));
                --_bricks_remaining; _score += BRICK_POINTS[row];
                _ball_vy = -_ball_vy;
                spawn_brick_particles(row, col);
                return true;
            }
        }
    }
    return false;
}

void BreakoutScreen::lose_life() {
    --_lives;
    if (_lives <= 0) _state = State::GAME_OVER;
    else { _state = State::DYING; _dying_timer = DYING_TIMER; _shake_timer = SHAKE_DURATION; }
}

void BreakoutScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();
    update_particles();
    if (_hit_spark_timer > 0) --_hit_spark_timer;
    if (_shake_timer > 0) { --_shake_timer; _shake_offset = static_cast<int8_t>((next_rng() % 3) - 1); }
    else _shake_offset = 0;
    if (_celebration_timer > 0) --_celebration_timer;

    // 暂停处理
    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
        return;
    }

    if (_state == State::GAME_OVER) {
        if (input.was_pressed(ButtonBits::START)) reset_game();
        return;
    }

    if ((_state == State::ATTACHED || _state == State::ACTIVE) && input.was_pressed(ButtonBits::START)) {
        _paused = true; return;
    }

    if (input.is_down(ButtonBits::LEFT))  _paddle_x -= 2;
    if (input.is_down(ButtonBits::RIGHT)) _paddle_x += 2;
    _paddle_x = std::max<int>(_paddle_x, 0);
    if (_paddle_x + PADDLE_W > 80) _paddle_x = static_cast<int16_t>(80 - PADDLE_W);

    if (_state == State::ATTACHED) {
        _ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2;
        _ball_y = PADDLE_Y - BALL_H;
        if (input.was_pressed(ButtonBits::A)) launch_ball();
        return;
    }

    if (_state == State::DYING) {
        if (_dying_timer > 0) --_dying_timer;
        else { _state = State::ATTACHED; _ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2; _ball_y = PADDLE_Y - BALL_H; }
        return;
    }

    if (_frame % 2 == 0) {
        _ball_trail[_trail_index].x = _ball_x; _ball_trail[_trail_index].y = _ball_y;
        _trail_index = static_cast<uint8_t>((_trail_index + 1) % 3);
    }

    _ball_x += _ball_vx; _ball_y += _ball_vy;
    if (_ball_x < 0) { _ball_x = 0; _ball_vx = -_ball_vx; }
    if (_ball_x + BALL_W > 80) { _ball_x = static_cast<int16_t>(80 - BALL_W); _ball_vx = -_ball_vx; }
    if (_ball_y < 0) { _ball_y = 0; _ball_vy = -_ball_vy; }
    if (_ball_y >= 80) { lose_life(); return; }
    check_paddle_collision();
    if (_bricks_remaining > 0) check_brick_collision();
    if (_bricks_remaining == 0) { _state = State::GAME_OVER; _celebration_timer = CELEBRATION_DURATION; }
}

void BreakoutScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG_COLOR);
    const int16_t shake = _shake_offset;

    display.fill_rect(Rect{0, 0, 80, 6}, BAR_COLOR);
    char sb[16];
    std::snprintf(sb, sizeof(sb), "SC:%d", _score);
    TextRenderer::draw_text(display, {2, 0}, sb, Color::WHITE, 1, COMPACT_FONT_3X5);
    for (int8_t i = 0; i < _lives; ++i) {
        display.draw_pixel(static_cast<int16_t>(66 + (i * 5)), 2, BALL_COLOR);
        display.draw_pixel(static_cast<int16_t>(67 + (i * 5)), 2, BALL_COLOR);
        display.draw_pixel(static_cast<int16_t>(66 + (i * 5)), 3, BALL_COLOR);
        display.draw_pixel(static_cast<int16_t>(67 + (i * 5)), 3, BALL_COLOR);
    }

    for (int16_t row = 0; row < BRICK_ROWS; ++row) {
        uint8_t mask = _bricks[row];
        if (mask == 0) continue;
        for (int16_t i = 0; i < BRICKS_PER_ROW; ++i) {
            if (!(mask & (1U << static_cast<unsigned>(i)))) continue;
            const Rect r = brick_rect(row, i);
            display.fill_rect(Rect{static_cast<int16_t>(r.x + 1), static_cast<int16_t>(r.y + shake),
                                   static_cast<int16_t>(r.width - 1), r.height}, BRICK_COLORS[row]);
        }
    }

    for (uint8_t ti = 0; ti < 3; ++ti) {
        uint8_t idx = static_cast<uint8_t>((_trail_index + 3 - ti - 1) % 3);
        if (_ball_trail[idx].x < 0 || ti > 0) continue;
        display.draw_pixel(_ball_trail[idx].x, _ball_trail[idx].y, TRAIL_COLOR);
        display.draw_pixel(static_cast<int16_t>(_ball_trail[idx].x + 1), _ball_trail[idx].y, TRAIL_COLOR);
    }

    for (uint8_t i = 0; i < _brick_particle_count; ++i)
        display.draw_pixel(_brick_particles[i].x, static_cast<int16_t>(_brick_particles[i].y + shake), _brick_particles[i].color);

    display.fill_rect(Rect{_paddle_x, static_cast<int16_t>(PADDLE_Y + shake), PADDLE_W, PADDLE_H}, PADDLE_COLOR);
    display.draw_h_line(_paddle_x, static_cast<int16_t>(PADDLE_Y + shake), PADDLE_W, PADDLE_EDGE);

    if (_hit_spark_timer > 0) {
        int16_t spark_r = (_hit_spark_x + 1 > 79) ? 79 : static_cast<int16_t>(_hit_spark_x + 1);
        for (int16_t sy = 0; sy < 3; ++sy) {
            display.draw_pixel(static_cast<int16_t>(_hit_spark_x - 1), static_cast<int16_t>(PADDLE_Y + shake - sy - 1), SPARK_COLOR);
            display.draw_pixel(spark_r, static_cast<int16_t>(PADDLE_Y + shake - sy - 1), SPARK_COLOR);
        }
    }

    if ((_state != State::DYING || (_dying_timer / 4) % 2 == 0) && _ball_y <= 77) {
        if (_state == State::ATTACHED && (_frame / 15) % 2 == 0) {
            display.fill_rect(Rect{static_cast<int16_t>(_ball_x - 1), static_cast<int16_t>(_ball_y + shake - 1),
                                   static_cast<int16_t>(BALL_W + 2), static_cast<int16_t>(BALL_H + 2)}, BALL_COLOR);
        } else {
            display.fill_rect(Rect{_ball_x, static_cast<int16_t>(_ball_y + shake), BALL_W, BALL_H}, BALL_COLOR);
        }
    }

    if (_celebration_timer > 0) {
        for (uint8_t i = 0; i < 8; ++i)
            display.draw_pixel(static_cast<int16_t>(next_rng() % 80), static_cast<int16_t>(next_rng() % 80), PARTICLE_COLORS[next_rng() % 5]);
    }

    if (_state == State::GAME_OVER) {
        display.fill_rect(Rect{10, 24, 60, 32}, BG_COLOR);
        bool won = (_bricks_remaining == 0);
        display.draw_rect(Rect{10, 24, 60, 32}, won ? WIN_COLOR : BRICK_COLORS[0]);
        TextRenderer::draw_text_centered(display, {40, 30}, won ? "YOU WIN!" : "GAME OVER",
                                         won ? WIN_COLOR : BRICK_COLORS[0], 1, BASIC_FONT_5X7);
        std::snprintf(sb, sizeof(sb), "SCORE: %d", _score);
        TextRenderer::draw_text_centered(display, {40, 44}, sb, Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    // 暂停覆盖层 (砖块主题)
    if (_paused) {
        display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
        display.draw_rect({10, 22, 60, 36}, PADDLE_COLOR);
        TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 54}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

}  // namespace handheld
