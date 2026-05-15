#include "scenes/snake/SnakeScreen.h"
#include "scenes/snake/SnakeConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/audio/AudioMixer.h"
#include "core/runtime/ScreenType.h"
#include <cmath>
#include <cstdio>

extern "C" [[gnu::weak]] const handheld::Tone _sound_BGM_SNAKE[];
extern "C" [[gnu::weak]] const uint32_t _sound_BGM_SNAKE_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_EAT[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_EAT_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_DEATH_SNAKE[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_DEATH_SNAKE_count;

namespace handheld {

using namespace snake::cfg;

namespace {

Color body_gradient_color(int16_t i, int16_t body_length) {
	if (body_length <= 1) return HEAD_COLOR;
	float t = static_cast<float>(i) / static_cast<float>(body_length - 1);
	auto r = static_cast<uint8_t>(static_cast<float>(HEAD_R) + (static_cast<float>(TAIL_R - HEAD_R) * t));
	auto g = static_cast<uint8_t>(static_cast<float>(HEAD_G) + (static_cast<float>(TAIL_G - HEAD_G) * t));
	auto b = static_cast<uint8_t>(static_cast<float>(HEAD_B) + (static_cast<float>(TAIL_B - HEAD_B) * t));
	return rgb565(r, g, b);
}

}  // namespace

void SnakeScreen::enter(IPlatform& platform, IScreenHost& host) {
	reset_game();
	platform.display().clear(BG_COLOR);
	if (_sound_BGM_SNAKE) host.mixer().set_bgm(_sound_BGM_SNAKE, _sound_BGM_SNAKE_count);
}

void SnakeScreen::reset_game() {
	_direction = Direction::RIGHT;
	_next_direction = Direction::RIGHT;
	_game_over = false;
	_won = false;
	_paused = false;
	_body_length = INITIAL_BODY_LENGTH;
	_body[0] = {INIT_HEAD_X, INIT_HEAD_Y};
	_body[1] = {INIT_SEG2_X, INIT_SEG2_Y};
	_body[2] = {INIT_SEG3_X, INIT_SEG3_Y};
	_score = 0;
	_move_counter = 0;
	_move_interval = INITIAL_MOVE_INTERVAL;
	_frame = 0;
	_rng_state = 12345;

	_eat_effect_timer = 0;
	_shake_timer = 0;
	_shake_offset_x = 0;
	_shake_offset_y = 0;

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

void SnakeScreen::move_snake(IScreenHost& host) {
	if (_next_direction == Direction::UP && _direction != Direction::DOWN) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::DOWN && _direction != Direction::UP) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::LEFT && _direction != Direction::RIGHT) {
		_direction = _next_direction;
	} else if (_next_direction == Direction::RIGHT && _direction != Direction::LEFT) {
		_direction = _next_direction;
	}

	int8_t new_x = _body[0].x;
	int8_t new_y = _body[0].y;
	switch (_direction) {
		case Direction::UP:    new_y = static_cast<int8_t>(new_y - 1); break;
		case Direction::DOWN:  new_y = static_cast<int8_t>(new_y + 1); break;
		case Direction::LEFT:  new_x = static_cast<int8_t>(new_x - 1); break;
		case Direction::RIGHT: new_x = static_cast<int8_t>(new_x + 1); break;
	}

	if (new_x < 0 || new_x >= GRID_SIZE || new_y < 0 || new_y >= GRID_SIZE) {
		_game_over = true;
		_shake_timer = SHAKE_DURATION;
		if (_sound_SFX_DEATH_SNAKE) host.mixer().play_sfx(_sound_SFX_DEATH_SNAKE, _sound_SFX_DEATH_SNAKE_count);
		return;
	}

	bool eating = (new_x == _food_x && new_y == _food_y);

	int16_t check_len = eating ? _body_length : static_cast<int16_t>(_body_length - 1);
	for (int16_t i = 0; i < check_len; ++i) {
		if (_body[i].x == new_x && _body[i].y == new_y) {
			_game_over = true;
			_shake_timer = SHAKE_DURATION;
			if (_sound_SFX_DEATH_SNAKE) host.mixer().play_sfx(_sound_SFX_DEATH_SNAKE, _sound_SFX_DEATH_SNAKE_count);
			return;
		}
	}

	for (int16_t i = _body_length - 1; i > 0; --i) {
		_body[i] = _body[i - 1];
	}
	_body[0] = {new_x, new_y};

	if (eating) {
		++_body_length;
		_body[_body_length - 1] = _body[_body_length - 2];
		++_score;

		_eat_effect_timer = EAT_EFFECT_DURATION;
		_eat_effect_x = new_x;
		_eat_effect_y = new_y;
		if (_sound_SFX_EAT) host.mixer().play_sfx(_sound_SFX_EAT, _sound_SFX_EAT_count);

		const uint32_t speed_down = static_cast<uint32_t>(_score) / 5;
		_move_interval = (speed_down >= INITIAL_MOVE_INTERVAL - MIN_MOVE_INTERVAL)
							? MIN_MOVE_INTERVAL
							: INITIAL_MOVE_INTERVAL - speed_down;

		if (_body_length >= MAX_LENGTH) {
			_won = true;
			_game_over = true;
			return;
		}

		spawn_food();
	}
}

void SnakeScreen::update(IPlatform& platform, IScreenHost& host) {
	++_frame;

	if (_eat_effect_timer > 0) --_eat_effect_timer;
	if (_shake_timer > 0) {
		--_shake_timer;
		_shake_offset_x = static_cast<int8_t>((next_rng() % 3) - 1);
		_shake_offset_y = static_cast<int8_t>((next_rng() % 3) - 1);
	} else {
		_shake_offset_x = 0;
		_shake_offset_y = 0;
	}

	auto& input = platform.input();

	// 暂停处理
	if (_paused) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
			_paused = false;
		}
		if (input.was_pressed(ButtonBits::B)) {
			host.switch_to(ScreenType::MENU);
		}
		return;
	}

	// 暂停触发
	if (!_game_over && input.was_pressed(ButtonBits::START)) {
		_paused = true;
		return;
	}

	if (_game_over) {
		if (input.was_pressed(ButtonBits::START)) {
			reset_game();
		}
		return;
	}

	if (input.was_pressed(ButtonBits::UP)) {
		_next_direction = Direction::UP;
	} else if (input.was_pressed(ButtonBits::DOWN)) {
		_next_direction = Direction::DOWN;
	} else if (input.was_pressed(ButtonBits::LEFT)) {
		_next_direction = Direction::LEFT;
	} else if (input.was_pressed(ButtonBits::RIGHT)) {
		_next_direction = Direction::RIGHT;
	}

	++_move_counter;
	if (_move_counter >= _move_interval) {
		_move_counter = 0;
		move_snake(host);
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

	// 食物（脉冲）
	const int16_t pulse = ((_frame / 8) % 2 == 0) ? 0 : 1;
	const Rect food_rect{
		static_cast<int16_t>((_food_x * CELL_SIZE) + 1 + pulse),
		static_cast<int16_t>((_food_y * CELL_SIZE) + 1 + pulse),
		static_cast<int16_t>(CELL_SIZE - 2 - (pulse * 2)),
		static_cast<int16_t>(CELL_SIZE - 2 - (pulse * 2)),
	};
	display.fill_rect(food_rect, FOOD_COLOR);

	// 蛇身（渐变 + 正弦波动画，填满整个格子）
	for (int16_t i = 1; i < _body_length; ++i) {
		int16_t wave_x = 0;
		int16_t wave_y = 0;
		float wave = std::sin((static_cast<float>(_frame) * WAVE_FREQ) + (static_cast<float>(i) * WAVE_PHASE_PER_SEG));
		auto wave_offset = static_cast<int16_t>(wave);
		switch (_direction) {
			case Direction::LEFT:
			case Direction::RIGHT:
				wave_y = wave_offset; break;
			case Direction::UP:
			case Direction::DOWN:
				wave_x = wave_offset; break;
		}
		const Rect seg_rect{
			static_cast<int16_t>((_body[i].x * CELL_SIZE) + 1 + wave_x),
			static_cast<int16_t>((_body[i].y * CELL_SIZE) + 1 + wave_y),
			static_cast<int16_t>(CELL_SIZE - 2),
			static_cast<int16_t>(CELL_SIZE - 2),
		};
		display.fill_rect(seg_rect, body_gradient_color(i, _body_length));
	}

	// 蛇头
	const auto hx = static_cast<int16_t>(_body[0].x * CELL_SIZE);
	const auto hy = static_cast<int16_t>(_body[0].y * CELL_SIZE);
	display.fill_rect(Rect{static_cast<int16_t>(hx + 1), static_cast<int16_t>(hy + 1),
	                       static_cast<int16_t>(CELL_SIZE - 2), static_cast<int16_t>(CELL_SIZE - 2)}, HEAD_COLOR);

	// 眼睛（根据方向）
	int16_t ex1_x = 0;
	int16_t ex1_y = 0;
	int16_t ex2_x = 0;
	int16_t ex2_y = 0;
	switch (_direction) {
		case Direction::RIGHT: ex1_x = hx + 5; ex1_y = hy + 2; ex2_x = hx + 5; ex2_y = hy + 5; break;
		case Direction::LEFT:  ex1_x = hx + 2; ex1_y = hy + 2; ex2_x = hx + 2; ex2_y = hy + 5; break;
		case Direction::UP:    ex1_x = hx + 2; ex1_y = hy + 2; ex2_x = hx + 5; ex2_y = hy + 2; break;
		case Direction::DOWN:  ex1_x = hx + 2; ex1_y = hy + 5; ex2_x = hx + 5; ex2_y = hy + 5; break;
	}
	display.draw_pixel(ex1_x, ex1_y, HEAD_EYE);
	display.draw_pixel(ex2_x, ex2_y, HEAD_EYE);

	// 吃食物粒子
	if (_eat_effect_timer > 0) {
		const auto epx = static_cast<int16_t>((_eat_effect_x * CELL_SIZE) + (CELL_SIZE / 2));
		const auto epy = static_cast<int16_t>((_eat_effect_y * CELL_SIZE) + (CELL_SIZE / 2));
		uint8_t phase = EAT_EFFECT_DURATION - _eat_effect_timer;
		if (phase < 4) {
			display.draw_pixel(static_cast<int16_t>(epx - phase), epy, EAT_PARTICLE);
			display.draw_pixel(static_cast<int16_t>(epx + phase), epy, EAT_PARTICLE);
			display.draw_pixel(epx, static_cast<int16_t>(epy - phase), EAT_PARTICLE);
			display.draw_pixel(epx, static_cast<int16_t>(epy + phase), EAT_PARTICLE);
		}
	}

	// 分数
	char score_buf[16];
	std::snprintf(score_buf, sizeof(score_buf), "SC:%d", _score);
	TextRenderer::draw_text(display, {2, 1}, score_buf, SCORE_COLOR, 1, COMPACT_FONT_3X5);

	// 游戏结束 / 胜利 UI
	if (_game_over) {
		const auto ox = static_cast<int16_t>(8 + _shake_offset_x);
		const auto oy = static_cast<int16_t>(22 + _shake_offset_y);
		display.fill_rect(Rect{ox, oy, 64, 36}, OVERLAY_BG);
		display.draw_rect(Rect{ox, oy, 64, 36}, _won ? WIN_COLOR : GAMEOVER_COLOR);

		const char* title = _won ? "YOU WIN!" : "GAME OVER";
		Color title_color = _won ? WIN_COLOR : GAMEOVER_COLOR;
		TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 8)}, title, title_color, 1, BASIC_FONT_5X7);

		char final_buf[16];
		std::snprintf(final_buf, sizeof(final_buf), "SCORE: %d", _score);
		TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 20)}, final_buf, SCORE_COLOR, 1, COMPACT_FONT_3X5);

		TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 28)}, "START=AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
	}

	// 暂停覆盖层
	if (_paused) {
		display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
		display.draw_rect({10, 22, 60, 36}, HEAD_COLOR);
		TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text_centered(display, {40, 54}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
