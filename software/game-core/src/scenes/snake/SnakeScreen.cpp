#include "scenes/snake/SnakeScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include <cstdio>

namespace handheld {

namespace {

constexpr Color BODY_COLOR = rgb565(40, 180, 60);
constexpr Color HEAD_COLOR = rgb565(80, 230, 100);
constexpr Color FOOD_COLOR = Color::RED;
constexpr Color GRID_LINE = rgb565(10, 22, 12);
constexpr Color BG_COLOR = rgb565(4, 10, 5);
constexpr Color SCORE_COLOR = rgb565(180, 220, 180);
constexpr Color GAMEOVER_COLOR = Color::RED;
constexpr Color OVERLAY_BG = rgb565(2, 5, 3);

}  // namespace

void SnakeScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	reset_game();
	platform.display().clear(BG_COLOR);
}

void SnakeScreen::reset_game() {
	_direction = Direction::RIGHT;
	_next_direction = Direction::RIGHT;
	_game_over = false;
	_won = false;
	_body_length = 3;
	_body[0] = {2, 5};  // head
	_body[1] = {1, 5};
	_body[2] = {0, 5};
	_score = 0;
	_move_counter = 0;
	_move_interval = INITIAL_MOVE_INTERVAL;
	_frame = 0;
	_rng_state = 12345;

	spawn_food();
}

uint32_t SnakeScreen::next_rng() {
	_rng_state ^= _rng_state << 13U;
	_rng_state ^= _rng_state >> 17U;
	_rng_state ^= _rng_state << 5U;
	return _rng_state;
}

bool SnakeScreen::is_occupied(int8_t x, int8_t y) const {
	for (int16_t i = 0; i < _body_length; ++i) {
		if (_body[i].x == x && _body[i].y == y) {
			return true;
		}
	}
	return false;
}

void SnakeScreen::spawn_food() {
	do {
		_food_x = static_cast<int8_t>(next_rng() % GRID_SIZE);
		_food_y = static_cast<int8_t>(next_rng() % GRID_SIZE);
	} while (is_occupied(_food_x, _food_y));
}

void SnakeScreen::move_snake() {
	// 应用方向输入（禁止 180° 掉头）
	if (_next_direction == Direction::UP && _direction != Direction::DOWN) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::DOWN && _direction != Direction::UP) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::LEFT && _direction != Direction::RIGHT) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::RIGHT && _direction != Direction::LEFT) {
		_direction = _next_direction;
	}

	// 计算新头部位置
	int8_t new_x = _body[0].x;
	int8_t new_y = _body[0].y;
	switch (_direction) {
		case Direction::UP:
			new_y = static_cast<int8_t>(new_y - 1);
			break;
		case Direction::DOWN:
			new_y = static_cast<int8_t>(new_y + 1);
			break;
		case Direction::LEFT:
			new_x = static_cast<int8_t>(new_x - 1);
			break;
		case Direction::RIGHT:
			new_x = static_cast<int8_t>(new_x + 1);
			break;
	}

	// 墙壁碰撞检测
	if (new_x < 0 || new_x >= GRID_SIZE || new_y < 0 || new_y >= GRID_SIZE) {
		_game_over = true;
		return;
	}

	// 食物判定
	bool eating = (new_x == _food_x && new_y == _food_y);

	// 自身碰撞检测（不吃食物时允许占据尾部即将离开的位置）
	int16_t check_len = eating ? _body_length : static_cast<int16_t>(_body_length - 1);
	for (int16_t i = 0; i < check_len; ++i) {
		if (_body[i].x == new_x && _body[i].y == new_y) {
			_game_over = true;
			return;
		}
	}

	// 移动身体
	for (int16_t i = _body_length - 1; i > 0; --i) {
		_body[i] = _body[i - 1];
	}
	_body[0] = {new_x, new_y};

	// 吃到食物
	if (eating) {
		++_body_length;
		_body[_body_length - 1] = _body[_body_length - 2];
		++_score;

		// 加速
		const uint32_t speed_down = static_cast<uint32_t>(_score) / 5;
		_move_interval = (speed_down >= INITIAL_MOVE_INTERVAL - MIN_MOVE_INTERVAL)
							? MIN_MOVE_INTERVAL
							: INITIAL_MOVE_INTERVAL - speed_down;

		// 胜利判定（占满全部格子）
		if (_body_length >= MAX_LENGTH) {
			_won = true;
			_game_over = true;
			return;
		}

		spawn_food();
	}
}

void SnakeScreen::update(IPlatform& platform, IScreenHost& /*host*/) {
	++_frame;
	auto& input = platform.input();

	// 游戏结束状态 — 按 START 重新开始
	if (_game_over) {
		if (input.was_pressed(ButtonBits::START)) {
			reset_game();
		}
		return;
	}

	// 方向输入
	if (input.was_pressed(ButtonBits::UP)) {
		_next_direction = Direction::UP;
	} else if (input.was_pressed(ButtonBits::DOWN)) {
		_next_direction = Direction::DOWN;
	} else if (input.was_pressed(ButtonBits::LEFT)) {
		_next_direction = Direction::LEFT;
	} else if (input.was_pressed(ButtonBits::RIGHT)) {
		_next_direction = Direction::RIGHT;
	}

	// 自动移动计时
	++_move_counter;
	if (_move_counter >= _move_interval) {
		_move_counter = 0;
		move_snake();
	}
}

void SnakeScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(BG_COLOR);

	// 绘制网格
	const auto edge = static_cast<int16_t>((GRID_SIZE * CELL_SIZE) - 1);
	for (int16_t i = 1; i < GRID_SIZE; ++i) {
		const int16_t p = i * CELL_SIZE;
		display.draw_v_line(p, 0, edge, GRID_LINE);
		display.draw_h_line(0, p, edge, GRID_LINE);
	}

	// 绘制食物
	const Rect food_rect{
		static_cast<int16_t>((_food_x * CELL_SIZE) + 1),
		static_cast<int16_t>((_food_y * CELL_SIZE) + 1),
		static_cast<int16_t>(CELL_SIZE - 2),
		static_cast<int16_t>(CELL_SIZE - 2),
	};
	display.fill_rect(food_rect, FOOD_COLOR);

	// 绘制蛇身
	for (int16_t i = 1; i < _body_length; ++i) {
		const Rect seg_rect{
			static_cast<int16_t>((_body[i].x * CELL_SIZE) + 1),
			static_cast<int16_t>((_body[i].y * CELL_SIZE) + 1),
			static_cast<int16_t>(CELL_SIZE - 2),
			static_cast<int16_t>(CELL_SIZE - 2),
		};
		display.fill_rect(seg_rect, BODY_COLOR);
	}

	// 绘制蛇头（更亮）
	const Rect head_rect{
		static_cast<int16_t>((_body[0].x * CELL_SIZE) + 1),
		static_cast<int16_t>((_body[0].y * CELL_SIZE) + 1),
		static_cast<int16_t>(CELL_SIZE - 2),
		static_cast<int16_t>(CELL_SIZE - 2),
	};
	display.fill_rect(head_rect, HEAD_COLOR);

	// 绘制分数
	char score_buf[16];
	std::snprintf(score_buf, sizeof(score_buf), "%d", _score);
	TextRenderer::draw_text(display, {2, 1}, score_buf, SCORE_COLOR, 1, COMPACT_FONT_3X5);

	// 游戏结束 / 胜利 UI
	if (_game_over) {
		// 半透明遮罩
		display.fill_rect(Rect{8, 22, 64, 36}, OVERLAY_BG);
		display.draw_rect(Rect{8, 22, 64, 36}, GAMEOVER_COLOR);

		const char* title = _won ? "YOU WIN!" : "GAME OVER";
		TextRenderer::draw_text_centered(display, {40, 30}, title, GAMEOVER_COLOR, 1, BASIC_FONT_5X7);

		char final_buf[16];
		std::snprintf(final_buf, sizeof(final_buf), "SCORE: %d", _score);
		TextRenderer::draw_text_centered(display, {40, 44}, final_buf, SCORE_COLOR, 1, COMPACT_FONT_3X5);

		TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", rgb565(120, 140, 120), 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
