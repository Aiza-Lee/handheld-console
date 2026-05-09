#include "scenes/pacman/PacmanScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include <cstdio>

namespace handheld {

namespace {

constexpr Color WALL_COLOR = rgb565(20, 20, 80);
constexpr Color DOT_COLOR = rgb565(255, 200, 150);
constexpr Color POWER_COLOR = rgb565(255, 220, 180);
constexpr Color PAC_COLOR = rgb565(255, 220, 50);
constexpr Color BG_COLOR = rgb565(2, 2, 12);
constexpr Color GHOST1_COL = Color::RED;
constexpr Color GHOST2_COL = Color::MAGENTA;
constexpr Color VULN_COLOR = rgb565(60, 60, 200);
constexpr Color VULN_BLINK = Color::WHITE;

}  // namespace

void PacmanScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	reset_game();
	platform.display().clear(BG_COLOR);
}

uint32_t PacmanScreen::next_rng() {
	_rng ^= _rng << 13U;
	_rng ^= _rng >> 17U;
	_rng ^= _rng << 5U;
	return _rng;
}

bool PacmanScreen::is_valid_cell(int8_t x, int8_t y) {
	return x >= 0 && x < GRID && y >= 0 && y < GRID;
}

bool PacmanScreen::is_wall(int8_t x, int8_t y) const {
	return !is_valid_cell(x, y) || _MAZE[y][x] == 'W';
}

int8_t PacmanScreen::manhattan(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
	int16_t dx = static_cast<int16_t>(x1) - static_cast<int16_t>(x2);
	int16_t dy = static_cast<int16_t>(y1) - static_cast<int16_t>(y2);
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	return static_cast<int8_t>(dx + dy);
}

bool PacmanScreen::try_move(int8_t x, int8_t y, Dir dir, int8_t& nx, int8_t& ny) const {
	nx = x;
	ny = y;
	switch (dir) {
		case Dir::UP:    ny = static_cast<int8_t>(y - 1); break;
		case Dir::DOWN:  ny = static_cast<int8_t>(y + 1); break;
		case Dir::LEFT:  nx = static_cast<int8_t>(x - 1); break;
		case Dir::RIGHT: nx = static_cast<int8_t>(x + 1); break;
		default: return false;
	}
	return !is_wall(nx, ny);
}

int8_t PacmanScreen::ghost_target_x(const Ghost& g) const {
	if (g.state == GhostState::RETURNING) return 4;
	return _pac_x;
}

int8_t PacmanScreen::ghost_target_y(const Ghost& g) const {
	if (g.state == GhostState::RETURNING) return 4;
	return _pac_y;
}

void PacmanScreen::reset_game() {
	_pac_x = 5;
	_pac_y = 8;
	_pac_dir = Dir::LEFT;
	_pac_next = Dir::LEFT;
	_state = State::PLAYING;
	_score = 0;
	_lives = 3;
	_power_timer = 0;
	_pac_move_counter = 0;
	_ghost_move_counter = 0;
	_dying_timer = 0;
	_frame = 0;
	_rng = 12345;

	// 初始化豆子状态
	_dot_count = 0;
	for (int8_t y = 0; y < GRID; ++y) {
		for (int8_t x = 0; x < GRID; ++x) {
			char c = _MAZE[y][x];
			_dots[y][x] = (c == '.' || c == 'P');
			_power[y][x] = (c == 'P');
			if (_dots[y][x]) ++_dot_count;
		}
	}
	// 去掉幽灵起点处的豆子
	if (_dots[4][4]) { _dots[4][4] = false; --_dot_count; }
	if (_dots[5][4]) { _dots[5][4] = false; --_dot_count; }

	// 初始化幽灵
	_ghosts[0] = {4, 4, Dir::UP, GhostState::CHASE};
	_ghosts[1] = {5, 4, Dir::UP, GhostState::CHASE};
}

void PacmanScreen::move_pacman() {
	// 尝试使用缓存的转向（禁止 180° 掉头）
	Dir nd = _pac_next;
	if ((nd == Dir::UP    && _pac_dir != Dir::DOWN) ||
		(nd == Dir::DOWN  && _pac_dir != Dir::UP) ||
		(nd == Dir::LEFT  && _pac_dir != Dir::RIGHT) ||
		(nd == Dir::RIGHT && _pac_dir != Dir::LEFT)) {
		int8_t nx = 0;
		int8_t ny = 0;
		if (try_move(_pac_x, _pac_y, nd, nx, ny)) {
			_pac_dir = nd;
		}
	}

	// 沿当前方向移动
	int8_t nx = 0;
	int8_t ny = 0;
	if (try_move(_pac_x, _pac_y, _pac_dir, nx, ny)) {
		_pac_x = nx;
		_pac_y = ny;
	}

	// 吃豆子
	if (_dots[_pac_y][_pac_x]) {
		_dots[_pac_y][_pac_x] = false;
		--_dot_count;
		_score += 10;

		if (_power[_pac_y][_pac_x]) {
			// 能量豆：幽灵反转 + 进入 VULNERABLE
			_power[_pac_y][_pac_x] = false;
			_power_timer = POWER_DURATION;
			for (auto& g : _ghosts) {
				if (g.state == GhostState::CHASE) {
					g.state = GhostState::VULNERABLE;
					// 反转方向
					switch (g.dir) {
						case Dir::UP:    g.dir = Dir::DOWN;  break;
						case Dir::DOWN:  g.dir = Dir::UP;    break;
						case Dir::LEFT:  g.dir = Dir::RIGHT; break;
						case Dir::RIGHT: g.dir = Dir::LEFT;  break;
						default: break;
					}
				}
			}
		}
	}
}

void PacmanScreen::move_ghosts() {
	for (auto& g : _ghosts) {
		if (g.state == GhostState::RETURNING && g.x == 4 && g.y == 4) {
			g.state = GhostState::CHASE;
		}

		// 幽灵移动速度：VULNERABLE 时更慢
		if (g.state == GhostState::VULNERABLE) {
			if ((_frame % 10) != 0) continue;
		}

		// 收集可走方向
		Dir valid[4];
		int8_t vcount = 0;
		Dir opposites[] = {Dir::DOWN, Dir::UP, Dir::RIGHT, Dir::LEFT};
		Dir all_dirs[] = {Dir::UP, Dir::DOWN, Dir::LEFT, Dir::RIGHT};

		for (auto & all_dir : all_dirs) {
			if (all_dir == opposites[static_cast<int>(g.dir)]) continue;
			int8_t nx = 0;
			int8_t ny = 0;
			if (try_move(g.x, g.y, all_dir, nx, ny)) {
				// 幽灵不能进入幽灵屋（O 单元格被当作墙处理）
				if (_MAZE[ny][nx] == 'O') continue;
				valid[vcount++] = all_dir;
			}
		}

		if (vcount == 0) {
			// 死胡同：强制反转
			for (auto & all_dir : all_dirs) {
				if (all_dir == opposites[static_cast<int>(g.dir)]) continue;
				int8_t nx = 0;
				int8_t ny = 0;
				if (try_move(g.x, g.y, all_dir, nx, ny)) {
					if (_MAZE[ny][nx] != 'O') {
						valid[vcount++] = all_dir;
						break;
					}
				}
			}
		}
		if (vcount == 0) continue;

		// 选择方向
		Dir best = valid[0];
		if (vcount > 1) {
			if (g.state == GhostState::VULNERABLE) {
				best = valid[next_rng() % vcount];
			} else {
				int8_t tx = ghost_target_x(g);
				int8_t ty = ghost_target_y(g);
				int8_t best_dist = 99;
				for (int i = 0; i < vcount; ++i) {
					int8_t nx = 0;
					int8_t ny = 0;
					try_move(g.x, g.y, valid[i], nx, ny);
					int8_t d = manhattan(nx, ny, tx, ty);
					if (d < best_dist) {
						best_dist = d;
						best = valid[i];
					}
				}
				// 50% 概率随机选择（给玩家喘息空间）
				if ((next_rng() % 2) == 0) {
					best = valid[next_rng() % vcount];
				}
			}
		}

		g.dir = best;
		switch (g.dir) {
			case Dir::UP:    g.y = static_cast<int8_t>(g.y - 1); break;
			case Dir::DOWN:  g.y = static_cast<int8_t>(g.y + 1); break;
			case Dir::LEFT:  g.x = static_cast<int8_t>(g.x - 1); break;
			case Dir::RIGHT: g.x = static_cast<int8_t>(g.x + 1); break;
			default: break;
		}
	}
}

void PacmanScreen::check_ghost_collision() {
	for (auto& g : _ghosts) {
		if (g.x == _pac_x && g.y == _pac_y) {
			if (g.state == GhostState::VULNERABLE) {
				// 吃掉幽灵
				g.state = GhostState::RETURNING;
				g.x = 4;
				g.y = 4;
				g.dir = Dir::UP;
				_score += 50;
			} else if (g.state == GhostState::CHASE) {
				die();
				return;
			}
		}
	}
}

void PacmanScreen::die() {
	_state = State::DYING;
	_dying_timer = 20;
	--_lives;
	_power_timer = 0;
}

void PacmanScreen::update(IPlatform& platform, IScreenHost& /*host*/) {
	++_frame;
	auto& input = platform.input();

	if (_state == State::GAME_OVER) {
		if (input.was_pressed(ButtonBits::START)) {
			reset_game();
		}
		return;
	}

	if (_state == State::DYING) {
		if (_dying_timer > 0) {
			--_dying_timer;
		} else {
			if (_lives <= 0) {
				_state = State::GAME_OVER;
			} else {
				// 重生：重置位置，保留分数和剩余的豆子
				_pac_x = 5;
				_pac_y = 8;
				_pac_dir = Dir::LEFT;
				_pac_next = Dir::LEFT;
				_ghosts[0] = {4, 4, Dir::UP, GhostState::CHASE};
				_ghosts[1] = {5, 4, Dir::UP, GhostState::CHASE};
				_power_timer = 0;
				_pac_move_counter = 0;
				_ghost_move_counter = 0;
				_state = State::PLAYING;
			}
		}
		return;
	}

	// 方向输入
	if (input.was_pressed(ButtonBits::UP))       _pac_next = Dir::UP;
	else if (input.was_pressed(ButtonBits::DOWN))  _pac_next = Dir::DOWN;
	else if (input.was_pressed(ButtonBits::LEFT))  _pac_next = Dir::LEFT;
	else if (input.was_pressed(ButtonBits::RIGHT)) _pac_next = Dir::RIGHT;

	// 计时移动
	++_pac_move_counter;
	if (_pac_move_counter >= PAC_MOVE_INTERVAL) {
		_pac_move_counter = 0;
		move_pacman();
	}

	++_ghost_move_counter;
	if (_ghost_move_counter >= GHOST_MOVE_INTERVAL) {
		_ghost_move_counter = 0;
		move_ghosts();
	}

	// 减能量计时器
	if (_power_timer > 0) {
		--_power_timer;
		if (_power_timer == 0) {
			for (auto& g : _ghosts) {
				if (g.state == GhostState::VULNERABLE) {
					g.state = GhostState::CHASE;
				}
			}
		}
	}

	check_ghost_collision();
}

void PacmanScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(BG_COLOR);

	// 绘制迷宫 + 豆子
	for (int8_t y = 0; y < GRID; ++y) {
		for (int8_t x = 0; x < GRID; ++x) {
			const auto px = static_cast<int16_t>(x * CELL);
			const auto py = static_cast<int16_t>(y * CELL);

			if (_MAZE[y][x] == 'W') {
				display.fill_rect(Rect{px, py, CELL, CELL}, WALL_COLOR);
			}

			if (_dots[y][x]) {
				if (_power[y][x]) {
					// 能量豆闪烁
					if ((_frame / 8) % 2 == 0) {
						display.fill_rect(Rect{static_cast<int16_t>(px + 2), static_cast<int16_t>(py + 2),
											static_cast<int16_t>(CELL - 4), static_cast<int16_t>(CELL - 4)}, POWER_COLOR);
					}
				} else {
					display.fill_rect(Rect{static_cast<int16_t>(px + 3), static_cast<int16_t>(py + 3),
										static_cast<int16_t>(CELL - 6), static_cast<int16_t>(CELL - 6)}, DOT_COLOR);
				}
			}
		}
	}

	// 绘制幽灵
	for (const auto& g : _ghosts) {
		const auto gx = static_cast<int16_t>(g.x * CELL);
		const auto gy = static_cast<int16_t>(g.y * CELL);
		const auto gx1 = static_cast<int16_t>(gx + 1);
		const auto gy1 = static_cast<int16_t>(gy + 1);

		Color gcol;
		if (g.state == GhostState::RETURNING) {
			// RETURNING 只画眼睛（跳过身体）
			display.fill_rect(Rect{gx1, gy1, 6, 6}, BG_COLOR);
			display.draw_pixel(static_cast<int16_t>(gx + 2), static_cast<int16_t>(gy + 2), Color::WHITE);
			display.draw_pixel(static_cast<int16_t>(gx + 5), static_cast<int16_t>(gy + 2), Color::WHITE);
			continue;
		} if (g.state == GhostState::VULNERABLE) {
			if (_power_timer < 40 && (_frame / 8) % 2 == 0) {
				gcol = VULN_BLINK;
			} else {
				gcol = VULN_COLOR;
			}
		} else {
			gcol = (&g == &_ghosts[0]) ? GHOST1_COL : GHOST2_COL;
		}

		display.fill_rect(Rect{gx1, gy1, 6, 6}, gcol);
		// 眼睛
		display.draw_pixel(gx + 2, gy + 2, Color::WHITE);
		display.draw_pixel(gx + 5, gy + 2, Color::WHITE);
	}

	// 绘制吃豆人（死亡状态闪烁）
	if (_state != State::DYING || (_dying_timer / 4) % 2 == 0) {
		const auto px = static_cast<int16_t>(_pac_x * CELL);
		const auto py = static_cast<int16_t>(_pac_y * CELL);
		const auto px1 = static_cast<int16_t>(px + 1);
		const auto py1 = static_cast<int16_t>(py + 1);
		const auto px2 = static_cast<int16_t>(px + 2);
		const auto py2_ = static_cast<int16_t>(py + 2);
		const auto px5 = static_cast<int16_t>(px + 5);
		const auto py5 = static_cast<int16_t>(py + 5);
		display.fill_rect(Rect{px1, py1, 6, 6}, PAC_COLOR);

		// 嘴：根据方向擦除 2x2 区域
		const bool mouth_open = (_frame / 3) % 2 == 0;
		if (mouth_open) {
			switch (_pac_dir) {
				case Dir::RIGHT:
					display.fill_rect(Rect{px5, py2_, 2, 4}, BG_COLOR);
					break;
				case Dir::LEFT:
					display.fill_rect(Rect{px1, py2_, 2, 4}, BG_COLOR);
					break;
				case Dir::UP:
					display.fill_rect(Rect{px2, py1, 4, 2}, BG_COLOR);
					break;
				case Dir::DOWN:
					display.fill_rect(Rect{px2, py5, 4, 2}, BG_COLOR);
					break;
				default: break;
			}
		}
	}

	// 分数 + 命数
	char buf[20];
	std::snprintf(buf, sizeof(buf), "%d", _score);
	TextRenderer::draw_text(display, {2, 78}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);

	for (int8_t i = 0; i < _lives; ++i) {
		display.fill_rect(Rect{static_cast<int16_t>(66 + (i * 5)), 78, 3, 3}, PAC_COLOR);
	}

	// 豆子剩余数量
	if (_dot_count == 0) {
		// 胜利！
		display.fill_rect(Rect{10, 28, 60, 24}, BG_COLOR);
		display.draw_rect(Rect{10, 28, 60, 24}, PAC_COLOR);
		TextRenderer::draw_text_centered(display, {40, 34}, "YOU WIN!", PAC_COLOR, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 46}, "START=AGAIN", rgb565(120, 140, 120), 1, COMPACT_FONT_3X5);
		_state = State::GAME_OVER;
	}

	// GAME OVER
	if (_state == State::GAME_OVER && _dot_count > 0) {
		display.fill_rect(Rect{10, 24, 60, 32}, BG_COLOR);
		display.draw_rect(Rect{10, 24, 60, 32}, Color::RED);
		TextRenderer::draw_text_centered(display, {40, 30}, "GAME OVER", Color::RED, 1, BASIC_FONT_5X7);
		char sb[16];
		std::snprintf(sb, sizeof(sb), "SCORE: %d", _score);
		TextRenderer::draw_text_centered(display, {40, 44}, sb, Color::WHITE, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", rgb565(120, 140, 120), 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
