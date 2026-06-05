#include "scenes/growball/GrowBallScreen.h"
#include "core/graphics/TextRenderer.h"
#include "core/graphics/Font.h"
#include "core/runtime/IScreenHost.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IInput.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "core/audio/Sounds.h"

namespace handheld {

using namespace growball::cfg;

// --- coordinate helpers ---

void GrowBallScreen::world_to_screen(float wx, float wy, int16_t& sx, int16_t& sy) const {
    sx = static_cast<int16_t>(std::lround((wx - _camera_x) * _zoom)) + CAMERA_OFFSET_X;
    sy = static_cast<int16_t>(std::lround((wy - _camera_y) * _zoom)) + CAMERA_OFFSET_Y;
}

// --- drawing ---

void GrowBallScreen::draw_filled_circle(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color) {
    if (r < 1) {
        display.draw_pixel(cx, cy, color);
        return;
    }
    for (int16_t dy = -r; dy <= r; ++dy) {
        auto half_w = static_cast<int16_t>(std::sqrt((r * r) - (dy * dy)));
        int16_t const sy = cy + dy;
        int16_t sx = cx - half_w;
        int16_t len = (half_w * 2) + 1;
        if (sy < 0 || sy >= SCREEN_HEIGHT) continue;
        if (sx < 0) {
            len += sx;
            sx = 0;
        }
        if (sx + len > SCREEN_WIDTH) len = SCREEN_WIDTH - sx;
        if (len <= 0) continue;
        display.draw_h_line(sx, sy, len, color);
    }
}

void GrowBallScreen::draw_circle_outline(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color) {
    if (r < 1) {
        display.draw_pixel(cx, cy, color);
        return;
    }
    int16_t x = 0, y = r, d = 1 - r;
    while (x <= y) {
        auto plot = [&](int16_t px, int16_t py) {
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) display.draw_pixel(px, py, color);
        };
        plot(cx + x, cy - y);
        plot(cx + y, cy - x);
        plot(cx + y, cy + x);
        plot(cx + x, cy + y);
        plot(cx - x, cy + y);
        plot(cx - y, cy + x);
        plot(cx - y, cy - x);
        plot(cx - x, cy - y);
        ++x;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            --y;
            d += 2 * (x - y) + 1;
        }
    }
}

// --- lifecycle ---

void GrowBallScreen::enter(IPlatform& platform, IScreenHost& host) {
    if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_GROWBALL, sounds::BGM_GROWBALL_COUNT);
    _world.init(platform.time().ticks_ms());
    _paused = false;
    _victory = false;
    _game_over = false;
}

void GrowBallScreen::update(IPlatform& platform, IScreenHost& host) {
    auto& input = platform.input();

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            _paused = false;
            host.audio().resume_bgm();
        }
        if (input.was_pressed(ButtonBits::B)) host.switch_to(ScreenType::MENU);
        return;
    }
    if (_victory) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            enter(platform, host);
            if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_GROWBALL, sounds::BGM_GROWBALL_COUNT);
        }
        if (input.was_pressed(ButtonBits::B)) host.switch_to(ScreenType::MENU);
        return;
    }
    if (_game_over) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            enter(platform, host);
            if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_GROWBALL, sounds::BGM_GROWBALL_COUNT);
        }
        if (input.was_pressed(ButtonBits::B)) host.switch_to(ScreenType::MENU);
        return;
    }

    // START = toggle pause
    if (input.was_pressed(ButtonBits::START)) {
        _paused = true;
        host.audio().pause_bgm();
        return;
    }

    // build input vector
    float const thrust = GrowBallWorld::speed_limit(_world.player().radius) * PLAYER_THRUST_MULT;
    float input_vx = 0, input_vy = 0;
    if (input.is_down(ButtonBits::UP)) input_vy -= thrust;
    if (input.is_down(ButtonBits::DOWN)) input_vy += thrust;
    if (input.is_down(ButtonBits::LEFT)) input_vx -= thrust;
    if (input.is_down(ButtonBits::RIGHT)) input_vx += thrust;

    // 推进器音效：方向键按住时每 8 帧播放一次
    bool const thrusting = (input_vx != 0.0f || input_vy != 0.0f);
    if (thrusting) {
        ++_thrust_timer;
        if (_thrust_timer >= 8) {
            _thrust_timer = 0;
            host.audio().play_sfx(sounds::SFX_THRUST, sounds::SFX_THRUST_COUNT);
        }
    } else {
        _thrust_timer = 0;
    }

    uint32_t const now_ms = platform.time().ticks_ms();
    float const radius_before = _world.player().radius;
    _world.simulate(input_vx, input_vy, now_ms, _victory, _game_over);

    if (_game_over) {
        host.audio().play_sfx(sounds::SFX_DEATH_GROWBALL, sounds::SFX_DEATH_GROWBALL_COUNT);
    } else if (_victory) {
        host.audio().play_sfx(sounds::SFX_VICTORY_GROWBALL, sounds::SFX_VICTORY_GROWBALL_COUNT);
    } else {
        float const growth = _world.player().radius - radius_before;
        if (growth > GROWTH_SFX_BIG_THRESHOLD) {
            host.audio().play_sfx(sounds::SFX_EAT_ENEMY_GROWBALL, sounds::SFX_EAT_ENEMY_GROWBALL_COUNT);
        } else if (growth > GROWTH_SFX_SMALL_THRESHOLD) {
            host.audio().play_sfx(sounds::SFX_EAT_GROWBALL, sounds::SFX_EAT_GROWBALL_COUNT);
        }
    }

    // camera
    _zoom = std::clamp(ZOOM_BASE / (_world.player().radius * ZOOM_DIVISOR), ZOOM_MIN, ZOOM_MAX);
    _camera_x = _world.player().x;
    _camera_y = _world.player().y;
}

void GrowBallScreen::render(IPlatform& platform, IScreenHost& host) {
    auto& display = platform.display();
    display.clear(BG_COLOR);

    // grid
    float const wx_start = _camera_x - (CAMERA_OFFSET_X / _zoom);
    float const wy_start = _camera_y - (CAMERA_OFFSET_Y / _zoom);
    float const wx_end = _camera_x + (CAMERA_OFFSET_X / _zoom);
    float const wy_end = _camera_y + (CAMERA_OFFSET_Y / _zoom);

    for (float wx = std::floor(wx_start / GRID_SPACING) * GRID_SPACING; wx <= wx_end; wx += GRID_SPACING) {
        int16_t sx = 0, sy0 = 0, sy1 = 0;
        world_to_screen(wx, wy_start, sx, sy0);
        world_to_screen(wx, wy_end, sx, sy1);
        if (sx >= 0 && sx < SCREEN_WIDTH) display.draw_v_line(sx, sy0, sy1 - sy0, GRID_COLOR);
    }
    for (float wy = std::floor(wy_start / GRID_SPACING) * GRID_SPACING; wy <= wy_end; wy += GRID_SPACING) {
        int16_t sx0 = 0, sy = 0, sx1 = 0;
        world_to_screen(wx_start, wy, sx0, sy);
        world_to_screen(wx_end, wy, sx1, sy);
        if (sy >= 0 && sy < SCREEN_HEIGHT) display.draw_h_line(sx0, sy, sx1 - sx0, GRID_COLOR);
    }

    // world boundary — 逐边绘制并裁剪到屏幕范围，避免越界写入
    {
        int16_t sx0, sy0, sx1, sy1;
        world_to_screen(0, 0, sx0, sy0);
        world_to_screen(WORLD_W, WORLD_H, sx1, sy1);

        int16_t const cy0 = std::max<int16_t>(sy0, 0);
        int16_t const cy1 = std::min<int16_t>(sy1, SCREEN_HEIGHT);
        int16_t const cx0 = std::max<int16_t>(sx0, 0);
        int16_t const cx1 = std::min<int16_t>(sx1, SCREEN_WIDTH);

        if (sx0 >= 0 && sx0 < SCREEN_WIDTH && cy1 > cy0) display.draw_v_line(sx0, cy0, cy1 - cy0, BORDER_COLOR);
        if (sx1 >= 0 && sx1 < SCREEN_WIDTH && cy1 > cy0) display.draw_v_line(sx1, cy0, cy1 - cy0, BORDER_COLOR);
        if (sy0 >= 0 && sy0 < SCREEN_HEIGHT && cx1 > cx0) display.draw_h_line(cx0, sy0, cx1 - cx0, BORDER_COLOR);
        if (sy1 >= 0 && sy1 < SCREEN_HEIGHT && cx1 > cx0) display.draw_h_line(cx0, sy1, cx1 - cx0, BORDER_COLOR);
    }

    // 收集所有球体，按屏幕半径排序后绘制（小先大后，大的覆盖小的）
    struct BallDrawInfo {
        int16_t sx, sy, sr;
        Color fill;
        bool has_outline;
        Color outline;
    };
    BallDrawInfo draw_list[1 + AI_COUNT + FOOD_COUNT];
    int draw_count = 0;

    for (int i = 0; i < FOOD_COUNT; ++i) {
        const auto& f = _world.foods()[i];
        int16_t sx = 0, sy = 0;
        world_to_screen(f.x, f.y, sx, sy);
        auto sr = static_cast<int16_t>(f.radius * _zoom + 0.5f);
        draw_list[draw_count++] = {sx, sy, static_cast<int16_t>(std::max<int>(sr, 1)), f.color, false, Color::BLACK};
    }
    for (int i = 0; i < AI_COUNT; ++i) {
        const auto& ai = _world.ai()[i];
        int16_t sx = 0, sy = 0;
        world_to_screen(ai.x, ai.y, sx, sy);
        auto sr = static_cast<int16_t>(ai.radius * _zoom + 0.5f);
        draw_list[draw_count++] = {sx, sy, static_cast<int16_t>(std::max<int>(sr, 1)), ai.color, true, Color::BLACK};
    }
    {
        const auto& player = _world.player();
        int16_t sx = 0, sy = 0;
        world_to_screen(player.x, player.y, sx, sy);
        auto sr = static_cast<int16_t>(player.radius * _zoom + 0.5f);
        draw_list[draw_count++] = {sx,           sy,   static_cast<int16_t>(std::max<int>(sr, 1)),
                                   player.color, true, Color::WHITE};
    }

    std::sort(draw_list, draw_list + draw_count,
              [](const BallDrawInfo& a, const BallDrawInfo& b) { return a.sr < b.sr; });

    for (int i = 0; i < draw_count; ++i) {
        const auto& b = draw_list[i];
        draw_filled_circle(display, b.sx, b.sy, b.sr, b.fill);
        if (b.has_outline) {
            draw_circle_outline(display, b.sx, b.sy, b.sr, b.outline);
        }
    }

    // HUD: score / target
    char buf[24];
    int const score = static_cast<int>(_world.player().radius * 10);
    int const target = TARGET_SCORE;
    snprintf(buf, sizeof(buf), "%d/%d", score, target);
    TextRenderer::draw_text(display, {0, 0}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);

    if (_paused) {
        display.fill_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BG);
        display.draw_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_TEXT);
        TextRenderer::draw_text_centered(display, {40, 28}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "A/START: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
    }
    if (_victory) {
        display.fill_rect({WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, VICTORY_BG);
        display.draw_rect({WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, VICTORY_TEXT);
        TextRenderer::draw_text_centered(display, {40, 28}, "YOU WIN!", VICTORY_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "A/START: Again", Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", Color::WHITE, 1, COMPACT_FONT_3X5);
    }
    if (_game_over) {
        display.fill_rect({END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, GAME_OVER_BG);
        display.draw_rect({END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, GAME_OVER_TEXT);
        TextRenderer::draw_text_centered(display, {40, 28}, "GAME OVER", GAME_OVER_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "A/START: Retry", Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", Color::WHITE, 1, COMPACT_FONT_3X5);
    }
}

void GrowBallScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    const auto cx = static_cast<int16_t>(box.x + box.width / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    const Color c = rgb565(180, 140, 240);
    display.draw_pixel(cx, cy, rgb565(240, 200, 100));
    int r = 1 + static_cast<int>((frame / 15) % 3);
    if (r >= 1) {
        display.draw_pixel(static_cast<int16_t>(cx + 1), cy, c);
        display.draw_pixel(static_cast<int16_t>(cx - 1), cy, c);
        display.draw_pixel(cx, static_cast<int16_t>(cy + 1), c);
        display.draw_pixel(cx, static_cast<int16_t>(cy - 1), c);
    }
    if (r >= 2) {
        display.draw_pixel(static_cast<int16_t>(cx + 2), cy, rgb565(140, 100, 200));
        display.draw_pixel(static_cast<int16_t>(cx - 2), cy, rgb565(140, 100, 200));
        display.draw_pixel(cx, static_cast<int16_t>(cy + 2), rgb565(140, 100, 200));
        display.draw_pixel(cx, static_cast<int16_t>(cy - 2), rgb565(140, 100, 200));
    }
}
} // namespace handheld
