#include "scenes/growball/GrowBallScreen.h"
#include "scenes/growball/GrowBallConfig.h"
#include "core/graphics/TextRenderer.h"
#include "core/graphics/Font.h"
#include "core/runtime/IScreenHost.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IInput.h"
#include "core/audio/AudioMixer.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>

extern "C" [[gnu::weak]] const handheld::Tone _sound_BGM_GROWBALL[];
extern "C" [[gnu::weak]] const uint32_t _sound_BGM_GROWBALL_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_EAT_GROWBALL[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_EAT_GROWBALL_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_EAT_ENEMY_GROWBALL[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_EAT_ENEMY_GROWBALL_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_DEATH_GROWBALL[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_DEATH_GROWBALL_count;

namespace handheld {

using namespace growball::cfg;

// --- coordinate helpers ---

void GrowBallScreen::world_to_screen(float wx, float wy, int16_t& sx, int16_t& sy) const {
	sx = static_cast<int16_t>(((wx - _camera_x) * _zoom) + 40 + 0.5f);
	sy = static_cast<int16_t>(((wy - _camera_y) * _zoom) + 40 + 0.5f);
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
		if (sy < 0 || sy >= 80) continue;
		if (sx < 0) { len += sx; sx = 0; }
		if (sx + len > 80) len = 80 - sx;
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
			if (px >= 0 && px < 80 && py >= 0 && py < 80) display.draw_pixel(px, py, color);
		};
		plot(cx + x, cy - y); plot(cx + y, cy - x);
		plot(cx + y, cy + x); plot(cx + x, cy + y);
		plot(cx - x, cy + y); plot(cx - y, cy + x);
		plot(cx - y, cy - x); plot(cx - x, cy - y);
		++x;
		if (d < 0) { d += 2 * x + 1; }
		else { --y; d += 2 * (x - y) + 1; }
	}
}

// --- lifecycle ---

void GrowBallScreen::enter(IPlatform& platform, IScreenHost& host) {
	if (ENABLE_BGM && _sound_BGM_GROWBALL) host.mixer().set_bgm(_sound_BGM_GROWBALL, _sound_BGM_GROWBALL_count);
	_start_ms = platform.time().ticks_ms();
	_world.init(_start_ms);
	_paused = false;
	_victory = false;
	_game_over = false;
}

void GrowBallScreen::update(IPlatform& platform, IScreenHost& host) {
	auto& input = platform.input();

	if (_paused) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
		return;
	}
	if (_victory) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START))
			enter(platform, host);
		return;
	}
	if (_game_over) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START))
			enter(platform, host);
		return;
	}

	// START = toggle pause
	if (input.was_pressed(ButtonBits::START)) {
		_paused = true;
		return;
	}

	// build input vector
	float const thrust = GrowBallWorld::speed_limit(_world.player().radius) * 0.5F;
	float input_vx = 0, input_vy = 0;
	if (input.is_down(ButtonBits::UP))    input_vy -= thrust;
	if (input.is_down(ButtonBits::DOWN))  input_vy += thrust;
	if (input.is_down(ButtonBits::LEFT))  input_vx -= thrust;
	if (input.is_down(ButtonBits::RIGHT)) input_vx += thrust;

	uint32_t const now_ms = platform.time().ticks_ms();
	float const radius_before = _world.player().radius;
	_world.simulate(input_vx, input_vy, now_ms, _victory, _game_over);

	if (_game_over) {
		if (_sound_SFX_DEATH_GROWBALL) host.mixer().play_sfx(_sound_SFX_DEATH_GROWBALL, _sound_SFX_DEATH_GROWBALL_count);
	} else if (!_victory) {
		float const growth = _world.player().radius - radius_before;
		if (growth > 2.0f) {
			if (_sound_SFX_EAT_ENEMY_GROWBALL) host.mixer().play_sfx(_sound_SFX_EAT_ENEMY_GROWBALL, _sound_SFX_EAT_ENEMY_GROWBALL_count);
		} else if (growth > 0.01f) {
			if (_sound_SFX_EAT_GROWBALL) host.mixer().play_sfx(_sound_SFX_EAT_GROWBALL, _sound_SFX_EAT_GROWBALL_count);
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
	float const wx_start = _camera_x - (40 / _zoom);
	float const wy_start = _camera_y - (40 / _zoom);
	float const wx_end   = _camera_x + (40 / _zoom);
	float const wy_end   = _camera_y + (40 / _zoom);

	for (float wx = std::floor(wx_start / 20) * 20; wx <= wx_end; wx += 20) {
		int16_t sx = 0, sy0 = 0, sy1 = 0;
		world_to_screen(wx, wy_start, sx, sy0);
		world_to_screen(wx, wy_end, sx, sy1);
		if (sx >= 0 && sx < 80) display.draw_v_line(sx, sy0, sy1 - sy0, GRID_COLOR);
	}
	for (float wy = std::floor(wy_start / 20) * 20; wy <= wy_end; wy += 20) {
		int16_t sx0 = 0, sy = 0, sx1 = 0;
		world_to_screen(wx_start, wy, sx0, sy);
		world_to_screen(wx_end, wy, sx1, sy);
		if (sy >= 0 && sy < 80) display.draw_h_line(sx0, sy, sx1 - sx0, GRID_COLOR);
	}

	// world boundary
	{
		int16_t sx0, sy0, sx1, sy1;
		world_to_screen(0, 0, sx0, sy0);
		world_to_screen(WORLD_W, WORLD_H, sx1, sy1);
		display.draw_rect({sx0, sy0, static_cast<int16_t>(sx1 - sx0), static_cast<int16_t>(sy1 - sy0)}, BORDER_COLOR);
	}

	// food
	for (int i = 0; i < FOOD_COUNT; ++i) {
		const auto& f = _world.foods()[i];
		int16_t sx = 0, sy = 0;
		world_to_screen(f.x, f.y, sx, sy);
		auto sr = static_cast<int16_t>(f.radius * _zoom + 0.5f);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, f.color);
	}

	// AI balls
	for (int i = 0; i < AI_COUNT; ++i) {
		const auto& ai = _world.ai()[i];
		int16_t sx = 0, sy = 0;
		world_to_screen(ai.x, ai.y, sx, sy);
		auto sr = static_cast<int16_t>(ai.radius * _zoom + 0.5f);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, ai.color);
		draw_circle_outline(display, sx, sy, sr, Color::BLACK);
	}

	// player
	{
		const auto& player = _world.player();
		int16_t sx = 0, sy = 0;
		world_to_screen(player.x, player.y, sx, sy);
		auto sr = static_cast<int16_t>(player.radius * _zoom + 0.5f);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, player.color);
		draw_circle_outline(display, sx, sy, sr, Color::WHITE);
	}

	// HUD: score / target
	char buf[24];
	int const score = static_cast<int>(_world.player().radius * 10);
	int const target = TARGET_SCORE;
	snprintf(buf, sizeof(buf), "%d/%d", score, target);
	TextRenderer::draw_text(display, {0, 0}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);

	if (_paused) {
		display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
		TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
	}
	if (_victory) {
		display.fill_rect({0, 50, 80, 30}, VICTORY_BG);
		TextRenderer::draw_text_centered(display, {40, 60}, "YOU WIN!", VICTORY_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 70}, "A: play again", Color::WHITE, 1, COMPACT_FONT_3X5);
	}
	if (_game_over) {
		display.fill_rect({0, 26, 80, 28}, GAME_OVER_BG);
		TextRenderer::draw_text_centered(display, {40, 34}, "GAME OVER", GAME_OVER_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 44}, "A: retry", Color::WHITE, 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
