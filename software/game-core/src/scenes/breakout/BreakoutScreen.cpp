#include "scenes/breakout/BreakoutScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include <algorithm>
#include <cstdio>

namespace handheld {

namespace {

constexpr Color BRICK_COLORS[5] = {
	Color::RED,                         // 行 0: 红 50分
	rgb565(255, 120, 0),                // 行 1: 橙 40分
	Color::YELLOW,                      // 行 2: 黄 30分
	Color::GREEN,                       // 行 3: 绿 20分
	Color::BLUE,                        // 行 4: 蓝 10分
};
constexpr int16_t BRICK_POINTS[5] = {50, 40, 30, 20, 10};
constexpr Color BG_COLOR = rgb565(4, 6, 12);
constexpr Color PADDLE_COLOR = rgb565(180, 200, 255);
constexpr Color BALL_COLOR = Color::WHITE;
constexpr Color BAR_COLOR = rgb565(8, 12, 20);

}  // namespace

void BreakoutScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	reset_game();
	platform.display().clear(BG_COLOR);
}

uint32_t BreakoutScreen::next_rng() {
	_rng ^= _rng << 13U;
	_rng ^= _rng >> 17U;
	_rng ^= _rng << 5U;
	return _rng;
}

Rect BreakoutScreen::brick_rect(int16_t row, int16_t col) const {
	return {
		static_cast<int16_t>(1 + (col * (BRICK_W + BRICK_GAP_X)) + (BRICK_GAP_X / 2)),
		static_cast<int16_t>(BRICK_OY + (row * (BRICK_H + BRICK_GAP_Y))),
		BRICK_W, BRICK_H,
	};
}

void BreakoutScreen::reset_game() {
	_state = State::ATTACHED;
	_paddle_x = 32;  // (80 - 16) / 2 = 32
	_ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2;
	_ball_y = PADDLE_Y - BALL_H;
	_ball_vx = 0;
	_ball_vy = 0;
	_score = 0;
	_lives = 3;
	_dying_timer = 0;
	_frame = 0;
	_rng = 12345;
	_bricks_remaining = BRICK_ROWS * BRICKS_PER_ROW;

	for (unsigned char & _brick : _bricks) {
		_brick = 0xFF;  // all bits set = all bricks alive
	}
}

void BreakoutScreen::launch_ball() {
	_ball_vx = (next_rng() % 2 == 0) ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
	_ball_vy = -2;
	_state = State::ACTIVE;
}

void BreakoutScreen::check_paddle_collision() {
	// 球在拍子的 Y 范围内
	if (_ball_y + BALL_H >= PADDLE_Y && _ball_y < PADDLE_Y + PADDLE_H) {
		if (_ball_x + BALL_W > _paddle_x && _ball_x < _paddle_x + PADDLE_W) {
			// 击中拍子
			_ball_y = PADDLE_Y - BALL_H;

			// 计算击中位置（相对于拍子中心）
			const int16_t ball_cx = _ball_x + (BALL_W / 2);
			const int16_t paddle_cx = _paddle_x + (PADDLE_W / 2);
			const int16_t off = ball_cx - paddle_cx;  // -8..+8

			if (off <= -5)              { _ball_vx = -2; _ball_vy = -1; }
			else if (off <= -2)         { _ball_vx = -1; _ball_vy = -2; }
			else if (off >= 5)          { _ball_vx = 2;  _ball_vy = -1; }
			else if (off >= 2)          { _ball_vx = 1;  _ball_vy = -2; }
			else                        { _ball_vx = 0;  _ball_vy = -2; }
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
			// AABB intersect
			if (_ball_x + BALL_W > br.x && _ball_x < br.x + br.width &&
				_ball_y + BALL_H > br.y && _ball_y < br.y + br.height) {
				_bricks[row] = static_cast<uint8_t>(mask & ~(1U << static_cast<unsigned>(col)));
				--_bricks_remaining;
				_score += BRICK_POINTS[row];
				_ball_vy = -_ball_vy;
				return true;
			}
		}
	}
	return false;
}

void BreakoutScreen::lose_life() {
	--_lives;
	if (_lives <= 0) {
		_state = State::GAME_OVER;
	} else {
		_state = State::DYING;
		_dying_timer = 15;
	}
}

void BreakoutScreen::update(IPlatform& platform, IScreenHost& /*host*/) {
	++_frame;
	auto& input = platform.input();

	if (_state == State::GAME_OVER) {
		if (input.was_pressed(ButtonBits::START)) {
			reset_game();
		}
		return;
	}

	// 拍子移动（通用）
	if (input.is_down(ButtonBits::LEFT))  _paddle_x = static_cast<int16_t>(_paddle_x - 2);
	if (input.is_down(ButtonBits::RIGHT)) _paddle_x = static_cast<int16_t>(_paddle_x + 2);
	_paddle_x = std::max<int>(_paddle_x, 0);
	if (_paddle_x + PADDLE_W > 80) _paddle_x = static_cast<int16_t>(80 - PADDLE_W);

	if (_state == State::ATTACHED) {
		// 球跟随拍子
		_ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2;
		_ball_y = PADDLE_Y - BALL_H;

		if (input.was_pressed(ButtonBits::A)) {
			launch_ball();
		}
		return;
	}

	if (_state == State::DYING) {
		if (_dying_timer > 0) {
			--_dying_timer;
		} else {
			_state = State::ATTACHED;
			_ball_x = _paddle_x + PADDLE_W / 2 - BALL_W / 2;
			_ball_y = PADDLE_Y - BALL_H;
		}
		return;
	}

	// === ACTIVE 状态 ===

	// 移动球
	_ball_x = static_cast<int16_t>(_ball_x + _ball_vx);
	_ball_y = static_cast<int16_t>(_ball_y + _ball_vy);

	// 墙壁反弹
	if (_ball_x < 0) { _ball_x = 0; _ball_vx = -_ball_vx; }
	if (_ball_x + BALL_W > 80) { _ball_x = static_cast<int16_t>(80 - BALL_W); _ball_vx = -_ball_vx; }
	if (_ball_y < 0) { _ball_y = 0; _ball_vy = -_ball_vy; }

	// 判定失球（球超出底部）
	if (_ball_y >= 80) {
		lose_life();
		return;
	}

	// 拍子碰撞
	check_paddle_collision();

	// 砖块碰撞
	if (_bricks_remaining > 0) {
		check_brick_collision();
	}

	// 胜利检测
	if (_bricks_remaining == 0) {
		_state = State::GAME_OVER;
	}
}

void BreakoutScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(BG_COLOR);

	// 顶栏（分数 + 命数）
	display.fill_rect(Rect{0, 0, 80, 6}, BAR_COLOR);
	char sb[16];
	std::snprintf(sb, sizeof(sb), "%d", _score);
	TextRenderer::draw_text(display, {2, 0}, sb, Color::WHITE, 1, COMPACT_FONT_3X5);
	for (int8_t i = 0; i < _lives; ++i) {
		display.draw_pixel(static_cast<int16_t>(66 + (i * 5)), 2, BALL_COLOR);
		display.draw_pixel(static_cast<int16_t>(66 + (i * 5) + 1), 2, BALL_COLOR);
		display.draw_pixel(static_cast<int16_t>(66 + (i * 5)), 3, BALL_COLOR);
		display.draw_pixel(static_cast<int16_t>(66 + (i * 5) + 1), 3, BALL_COLOR);
	}

	// 砖块
	for (int16_t row = 0; row < BRICK_ROWS; ++row) {
		uint8_t mask = _bricks[row];
		if (mask == 0) continue;
		const Color col = BRICK_COLORS[row];
		for (int16_t i = 0; i < BRICKS_PER_ROW; ++i) {
			if (!(mask & (1U << static_cast<unsigned>(i)))) continue;
			const Rect r = brick_rect(row, i);
			// 砖块内缩 1px 制造间隙视觉效果
			display.fill_rect(Rect{
				static_cast<int16_t>(r.x + 1), r.y,
				static_cast<int16_t>(r.width - 1), r.height
			}, col);
		}
	}

	// 拍子
	display.fill_rect(Rect{_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);

	// 球
	if (_state != State::DYING || (_dying_timer / 4) % 2 == 0) {
		display.fill_rect(Rect{_ball_x, _ball_y, BALL_W, BALL_H}, BALL_COLOR);
	}

	// GAME OVER / WIN
	if (_state == State::GAME_OVER) {
		display.fill_rect(Rect{10, 24, 60, 32}, BG_COLOR);
		if (_bricks_remaining == 0) {
			display.draw_rect(Rect{10, 24, 60, 32}, Color::YELLOW);
			TextRenderer::draw_text_centered(display, {40, 30}, "YOU WIN!", Color::YELLOW, 1, BASIC_FONT_5X7);
		} else {
			display.draw_rect(Rect{10, 24, 60, 32}, Color::RED);
			TextRenderer::draw_text_centered(display, {40, 30}, "GAME OVER", Color::RED, 1, BASIC_FONT_5X7);
		}
		std::snprintf(sb, sizeof(sb), "SCORE: %d", _score);
		TextRenderer::draw_text_centered(display, {40, 44}, sb, Color::WHITE, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", rgb565(120, 140, 120), 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
