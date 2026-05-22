#include "scenes/growball/GrowBallScreen.h"
#include "platform/interfaces/IDisplay.h"
#include "core/graphics/Color.h"
#include "core/common/ButtonBits.h"
#include "scenes/growball/GrowBallConfig.h"
#include "core/graphics/TextRenderer.h"
#include "core/graphics/Font.h"
#include "core/runtime/IScreenHost.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IInput.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace handheld {

using namespace growball::cfg;

static float rand_float(float min, float max) {
	float const t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return min + (t * (max - min));
}

static float clamp_f(float v, float lo, float hi) {
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

// --- world <-> screen ---

void GrowBallScreen::world_to_screen(float wx, float wy, int16_t& sx, int16_t& sy) const {
	sx = static_cast<int16_t>(((wx - _camera_x) * _zoom) + 40);
	sy = static_cast<int16_t>(((wy - _camera_y) * _zoom) + 40);
}

float GrowBallScreen::screen_to_world_size(float screen_size) const {
	return screen_size / _zoom;
}

void GrowBallScreen::clamp_to_world(Ball& ball) {
	ball.x = clamp_f(ball.x, ball.radius, WORLD_W - ball.radius);
	ball.y = clamp_f(ball.y, ball.radius, WORLD_H - ball.radius);
}

float GrowBallScreen::ball_speed_limit(float radius) {
	return PLAYER_MAX_SPEED * (1.0F - clamp_f(radius / PLAYER_MAX_RADIUS, 0.0F, 0.8F));
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
		if (sx < 0) {
			len += sx;
			sx = 0;
		}
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
	int16_t x = 0;
	int16_t y = r;
	int16_t d = 1 - r;
	while (x <= y) {
		auto plot = [&](int16_t px, int16_t py) {
			if (px >= 0 && px < 80 && py >= 0 && py < 80) display.draw_pixel(px, py, color);
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

// --- food ---

static void spawn_one_food(GrowBallScreen::Food& f) {
	f.x = rand_float(5, WORLD_W - 5);
	f.y = rand_float(5, WORLD_H - 5);
	f.radius = rand_float(FOOD_MIN_RADIUS, FOOD_MAX_RADIUS);
	f.color = FOOD_COLOR;
}

void GrowBallScreen::spawn_food(int index) {
	spawn_one_food(_foods[index]);
}

// --- AI ---

void GrowBallScreen::spawn_ai(int index, uint32_t now_ms) {
	auto& ai = _ai[index];
	ai.x = rand_float(10, WORLD_W - 10);
	ai.y = rand_float(10, WORLD_H - 10);
	ai.vx = 0;
	ai.vy = 0;
	ai.radius = rand_float(AI_MIN_RADIUS, AI_MAX_RADIUS);
	ai.color = AI_COLORS[index % 5];
	ai.wander_angle = rand_float(0, 6.28F);
	ai.last_change_ms = now_ms;
}

// --- lifecycle ---

void GrowBallScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	_player.x = WORLD_W / 2.0F;
	_player.y = WORLD_H / 2.0F;
	_player.vx = 0;
	_player.vy = 0;
	_player.radius = PLAYER_INIT_RADIUS;
	_player.color = PLAYER_COLOR;
	_game_over = false;
	_start_ms = platform.time().ticks_ms();

	for (int i = 0; i < FOOD_COUNT; ++i) spawn_food(i);
	for (int i = 0; i < AI_COUNT; ++i) spawn_ai(i, _start_ms);
}

void GrowBallScreen::update(IPlatform& platform, IScreenHost& host) {
	auto& input = platform.input();

	if (_game_over) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
			enter(platform, host);
		}
		return;
	}

	// player input
	float const thrust = ball_speed_limit(_player.radius) * 0.5F;
	if (input.is_down(ButtonBits::UP))    _player.vy -= thrust;
	if (input.is_down(ButtonBits::DOWN))  _player.vy += thrust;
	if (input.is_down(ButtonBits::LEFT))  _player.vx -= thrust;
	if (input.is_down(ButtonBits::RIGHT)) _player.vx += thrust;

	// player physics
	_player.vx *= PLAYER_FRICTION;
	_player.vy *= PLAYER_FRICTION;
	float const spd = std::sqrt((_player.vx * _player.vx) + (_player.vy * _player.vy));
	float const limit = ball_speed_limit(_player.radius);
	if (spd > limit) {
		_player.vx *= limit / spd;
		_player.vy *= limit / spd;
	}
	_player.x += _player.vx;
	_player.y += _player.vy;
	clamp_to_world(_player);

	// player eats food
	for (auto& f : _foods) {
		float const dx = f.x - _player.x;
		float const dy = f.y - _player.y;
		float const dist = std::sqrt((dx * dx) + (dy * dy));
		if (dist < _player.radius + f.radius && _player.radius > f.radius * 0.5F) {
			_player.radius += FOOD_GROWTH;
			_player.radius = std::min(_player.radius, PLAYER_MAX_RADIUS);
			spawn_one_food(f);
		}
	}

	// AI update
	uint32_t const now_ms = platform.time().ticks_ms();
	for (int i = 0; i < AI_COUNT; ++i) {
		auto& ai = _ai[i];

		// find nearest smaller and larger ball (player or other AI)
		float nearest_smaller_dist = 9999;
		float nearest_larger_dist = 9999;
		float smaller_dx = 0;
		float smaller_dy = 0;
		float larger_dx = 0;
		float larger_dy = 0;

		auto check_target = [&](float tx, float ty, float tr) {
			float const dx = tx - ai.x;
			float const dy = ty - ai.y;
			float const dist = std::sqrt((dx * dx) + (dy * dy));
			if (dist < 60 && ai.radius > tr * 1.1F && dist < nearest_smaller_dist) {
				nearest_smaller_dist = dist;
				smaller_dx = dx;
				smaller_dy = dy;
			}
			if (dist < 60 && tr > ai.radius * 1.1F && dist < nearest_larger_dist) {
				nearest_larger_dist = dist;
				larger_dx = dx;
				larger_dy = dy;
			}
		};

		check_target(_player.x, _player.y, _player.radius);
		for (int j = 0; j < AI_COUNT; ++j) {
			if (i == j) continue;
			check_target(_ai[j].x, _ai[j].y, _ai[j].radius);
		}

		// behavior: chase smaller, flee larger, or wander
		float target_dx = 0;
		float target_dy = 0;
		if (nearest_smaller_dist < 50) {
			target_dx = smaller_dx;
			target_dy = smaller_dy;
		} else if (nearest_larger_dist < 40) {
			target_dx = -larger_dx;
			target_dy = -larger_dy;
		} else {
			// wander
			if (now_ms - ai.last_change_ms > static_cast<uint32_t>(AI_WANDER_CHANGE_MS)) {
				ai.wander_angle += rand_float(-1.0F, 1.0F);
				ai.last_change_ms = now_ms;
			}
			target_dx = std::cos(ai.wander_angle) * 30;
			target_dy = std::sin(ai.wander_angle) * 30;
		}

		float const tdist = std::sqrt((target_dx * target_dx) + (target_dy * target_dy));
		if (tdist > 0.01F) {
			ai.vx += (target_dx / tdist) * AI_THRUST;
			ai.vy += (target_dy / tdist) * AI_THRUST;
		}

		ai.vx *= AI_FRICTION;
		ai.vy *= AI_FRICTION;
		float const ai_spd = std::sqrt((ai.vx * ai.vx) + (ai.vy * ai.vy));
		if (ai_spd > AI_MAX_SPEED) {
			ai.vx *= AI_MAX_SPEED / ai_spd;
			ai.vy *= AI_MAX_SPEED / ai_spd;
		}
		ai.x += ai.vx;
		ai.y += ai.vy;
		ai.x = clamp_f(ai.x, ai.radius, WORLD_W - ai.radius);
		ai.y = clamp_f(ai.y, ai.radius, WORLD_H - ai.radius);

		// AI eats food
		for (auto& f : _foods) {
			float const dx = f.x - ai.x;
			float const dy = f.y - ai.y;
			float const dist = std::sqrt((dx * dx) + (dy * dy));
			if (dist < ai.radius + f.radius && ai.radius > f.radius) {
				ai.radius += FOOD_GROWTH * 0.7F;
				spawn_one_food(f);
			}
		}
	}

	// AI vs AI
	for (int i = 0; i < AI_COUNT; ++i) {
		for (int j = i + 1; j < AI_COUNT; ++j) {
			float const dx = _ai[j].x - _ai[i].x;
			float const dy = _ai[j].y - _ai[i].y;
			float const dist = std::sqrt((dx * dx) + (dy * dy));
			float const overlap = _ai[i].radius + _ai[j].radius;
			if (dist < overlap && dist > 0.01F) {
				if (_ai[i].radius > _ai[j].radius * AI_EAT_RATIO) {
					_ai[i].radius += _ai[j].radius * 0.3F;
					spawn_ai(j, now_ms);
				} else if (_ai[j].radius > _ai[i].radius * AI_EAT_RATIO) {
					_ai[j].radius += _ai[i].radius * 0.3F;
					spawn_ai(i, now_ms);
				}
			}
		}
	}

	// player vs AI
	for (int i = 0; i < AI_COUNT; ++i) {
		float const dx = _ai[i].x - _player.x;
		float const dy = _ai[i].y - _player.y;
		float const dist = std::sqrt((dx * dx) + (dy * dy));
		if (dist < _player.radius + _ai[i].radius) {
			if (_player.radius > _ai[i].radius * 1.1F) {
				_player.radius += _ai[i].radius * 0.4F;
				_player.radius = std::min(_player.radius, PLAYER_MAX_RADIUS);
				spawn_ai(i, now_ms);
			} else if (_ai[i].radius > _player.radius * 1.1F) {
				_game_over = true;
			}
		}
	}

	// camera
	_zoom = clamp_f(ZOOM_BASE / (_player.radius * ZOOM_DIVISOR), ZOOM_MIN, ZOOM_MAX);
	_camera_x = _player.x;
	_camera_y = _player.y;
}

void GrowBallScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	auto& display = platform.display();
	display.clear(BG_COLOR);

	// grid
	auto grid_step = static_cast<int16_t>(20 * _zoom);
	grid_step = std::max<int>(grid_step, 3);
	float const wx_start = _camera_x - (40 / _zoom);
	float const wy_start = _camera_y - (40 / _zoom);
	float const wx_end = _camera_x + (40 / _zoom);
	float const wy_end = _camera_y + (40 / _zoom);

	for (float wx = std::floor(wx_start / 20) * 20; wx <= wx_end; wx += 20) {
		int16_t sx = 0;
		int16_t sy0 = 0;
		int16_t sy1 = 0;
		world_to_screen(wx, wy_start, sx, sy0);
		world_to_screen(wx, wy_end, sx, sy1);
		if (sx >= 0 && sx < 80) display.draw_v_line(sx, sy0, sy1 - sy0, GRID_COLOR);
	}
	for (float wy = std::floor(wy_start / 20) * 20; wy <= wy_end; wy += 20) {
		int16_t sx0 = 0;
		int16_t sy = 0;
		int16_t sx1 = 0;
		world_to_screen(wx_start, wy, sx0, sy);
		world_to_screen(wx_end, wy, sx1, sy);
		if (sy >= 0 && sy < 80) display.draw_h_line(sx0, sy, sx1 - sx0, GRID_COLOR);
	}

	// food
	for (const auto& f : _foods) {
		int16_t sx = 0;
		int16_t sy = 0;
		world_to_screen(f.x, f.y, sx, sy);
		auto sr = static_cast<int16_t>(f.radius * _zoom);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, f.color);
	}

	// AI balls
	for (const auto& ai : _ai) {
		int16_t sx = 0;
		int16_t sy = 0;
		world_to_screen(ai.x, ai.y, sx, sy);
		auto sr = static_cast<int16_t>(ai.radius * _zoom);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, ai.color);
		draw_circle_outline(display, sx, sy, sr, Color::BLACK);
	}

	// player
	{
		int16_t sx = 0;
		int16_t sy = 0;
		world_to_screen(_player.x, _player.y, sx, sy);
		auto sr = static_cast<int16_t>(_player.radius * _zoom);
		sr = std::max<int>(sr, 1);
		draw_filled_circle(display, sx, sy, sr, _player.color);
		draw_circle_outline(display, sx, sy, sr, Color::WHITE);
	}

	// HUD: score = player radius * 10
	char buf[16];
	int const score = static_cast<int>(_player.radius * 10);
	snprintf(buf, sizeof(buf), "%d", score);
	TextRenderer::draw_text(display, {1, 1}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);

	if (_game_over) {
		display.fill_rect({0, 26, 80, 28}, GAME_OVER_BG);
		TextRenderer::draw_text_centered(display, {40, 34}, "GAME OVER", GAME_OVER_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 44}, "A: retry", Color::WHITE, 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
