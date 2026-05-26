#include "scenes/growball/GrowBallWorld.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace handheld {

using namespace growball::cfg;

static float rand_float(float min, float max) {
	return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
}

static float clamp_f(float v, float lo, float hi) {
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

// --- helpers ---

float GrowBallWorld::speed_limit(float radius) {
	return PLAYER_MAX_SPEED * (1.0F - clamp_f(radius / PLAYER_MAX_RADIUS, 0.0F, 0.8F));
}

void GrowBallWorld::clamp_to_world(Ball& ball) {
	ball.x = clamp_f(ball.x, ball.radius, WORLD_W - ball.radius);
	ball.y = clamp_f(ball.y, ball.radius, WORLD_H - ball.radius);
}


static float add_area_growth(float radius, float added_area) {
	return std::sqrt((radius * radius) + added_area);
}

// --- spawning ---

void GrowBallWorld::spawn_food(int index) {
	auto& f = _foods[index];
	f.x = rand_float(5, WORLD_W - 5);
	f.y = rand_float(5, WORLD_H - 5);
	f.radius = rand_float(FOOD_MIN_RADIUS, FOOD_MAX_RADIUS);
	f.color = FOOD_COLOR;
}

void GrowBallWorld::spawn_ai(int index, uint32_t now_ms) {
	auto& ai = _ai[index];
	// 避免生成在玩家附近
	float const min_dist = _player.radius * AI_SPAWN_MIN_DIST_MULT;
	for (int retry = 0; retry < 10; ++retry) {
		ai.x = rand_float(10, WORLD_W - 10);
		ai.y = rand_float(10, WORLD_H - 10);
		float const dx = ai.x - _player.x;
		float const dy = ai.y - _player.y;
		if (std::sqrt((dx * dx) + (dy * dy)) > min_dist) break;
	}
	ai.vx = 0;
	ai.vy = 0;
	ai.radius = rand_float(AI_MIN_RADIUS, AI_MAX_RADIUS);
	ai.color = AI_COLORS[index % 5];
	ai.wander_angle = rand_float(0, 6.28F);
	ai.last_change_ms = now_ms;
}

// --- init ---

void GrowBallWorld::init(uint32_t now_ms) {
	_player.x = WORLD_W / 2.0F;
	_player.y = WORLD_H / 2.0F;
	_player.vx = 0;
	_player.vy = 0;
	_player.radius = PLAYER_INIT_RADIUS;
	_player.color = PLAYER_COLOR;

	for (int i = 0; i < FOOD_COUNT; ++i) spawn_food(i);
	for (int i = 0; i < AI_COUNT; ++i) spawn_ai(i, now_ms);
}

// --- AI update ---

void GrowBallWorld::update_ai(uint32_t now_ms) {
	for (int i = 0; i < AI_COUNT; ++i) {
		auto& ai = _ai[i];

		float nearest_smaller_dist = 9999;
		float nearest_larger_dist = 9999;
		float smaller_dx = 0, smaller_dy = 0;
		float larger_dx = 0, larger_dy = 0;

		auto check_target = [&](float tx, float ty, float tr) {
			float const dx = tx - ai.x;
			float const dy = ty - ai.y;
			float const dist = std::sqrt((dx * dx) + (dy * dy));
			if (dist < AI_DETECT_RANGE && ai.radius > tr * 1.1F && dist < nearest_smaller_dist) {
				nearest_smaller_dist = dist;
				smaller_dx = dx;
				smaller_dy = dy;
			}
			if (dist < AI_DETECT_RANGE && tr > ai.radius * 1.1F && dist < nearest_larger_dist) {
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

		float target_dx = 0, target_dy = 0;
		if (nearest_smaller_dist < AI_CHASE_RANGE) {
			target_dx = smaller_dx;
			target_dy = smaller_dy;
		} else if (nearest_larger_dist < AI_FLEE_RANGE) {
			target_dx = -larger_dx;
			target_dy = -larger_dy;
		} else {
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
	}
}

// --- collision resolution ---

void GrowBallWorld::resolve_collisions(uint32_t now_ms, bool& victory, bool& game_over) {
	// player eats food
	for (auto& f : _foods) {
		float const dx = f.x - _player.x;
		float const dy = f.y - _player.y;
		float const dist = std::sqrt((dx * dx) + (dy * dy));
		if (dist < _player.radius + f.radius && _player.radius > f.radius * 0.5F) {
			_player.radius = add_area_growth(_player.radius, FOOD_MASS);
			_player.radius = std::min(_player.radius, PLAYER_MAX_RADIUS);
			spawn_food(static_cast<int>(&f - _foods));
		}
	}

	// AI eats food
	for (int i = 0; i < AI_COUNT; ++i) {
		for (auto& f : _foods) {
			float const dx = f.x - _ai[i].x;
			float const dy = f.y - _ai[i].y;
			float const dist = std::sqrt((dx * dx) + (dy * dy));
			if (dist < _ai[i].radius + f.radius && _ai[i].radius > f.radius) {
				_ai[i].radius = add_area_growth(_ai[i].radius, FOOD_MASS * 0.7F);
				spawn_food(static_cast<int>(&f - _foods));
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
					_ai[i].radius = add_area_growth(_ai[i].radius, _ai[j].radius * _ai[j].radius * 0.3F);
					spawn_ai(j, now_ms);
				} else if (_ai[j].radius > _ai[i].radius * AI_EAT_RATIO) {
					_ai[j].radius = add_area_growth(_ai[j].radius, _ai[i].radius * _ai[i].radius * 0.3F);
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
		if (dist < (_player.radius + _ai[i].radius) * 0.92f) {
			if (_player.radius > _ai[i].radius * 1.1F) {
				_player.radius = add_area_growth(_player.radius, _ai[i].radius * _ai[i].radius * 0.4F);
				_player.radius = std::min(_player.radius, PLAYER_MAX_RADIUS);
				spawn_ai(i, now_ms);
				if (_player.radius >= PLAYER_MAX_RADIUS) {
					victory = true;
					return;
				}
			} else if (_ai[i].radius > _player.radius * 1.1F) {
				game_over = true;
				return;
			}
		}
	}
}

// --- simulate ---

void GrowBallWorld::simulate(float input_vx, float input_vy, uint32_t now_ms, bool& victory, bool& game_over) {
	victory = false;
	game_over = false;

	// player physics
	_player.vx += input_vx;
	_player.vy += input_vy;
	_player.vx *= PLAYER_FRICTION;
	_player.vy *= PLAYER_FRICTION;
	float const spd = std::sqrt((_player.vx * _player.vx) + (_player.vy * _player.vy));
	float const limit = speed_limit(_player.radius);
	if (spd > limit) {
		_player.vx *= limit / spd;
		_player.vy *= limit / spd;
	}
	_player.x += _player.vx;
	_player.y += _player.vy;
	clamp_to_world(_player);

	update_ai(now_ms);
	resolve_collisions(now_ms, victory, game_over);

	// food-only victory check (player may reach max without eating AI)
	if (!victory && !game_over && _player.radius >= PLAYER_MAX_RADIUS) {
		victory = true;
	}
}

}  // namespace handheld
