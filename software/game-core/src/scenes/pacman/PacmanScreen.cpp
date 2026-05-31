#include "scenes/pacman/PacmanScreen.h"
#include "scenes/pacman/PacmanConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include <cstdio>
#include "core/audio/Sounds.h"

namespace handheld {

using namespace pacman::cfg;

namespace {

constexpr uint32_t RNG_SEED = 12345;

}  // namespace

void PacmanScreen::enter(IPlatform& platform, IScreenHost& host) {
	reset_game();
	platform.display().clear(BG_COLOR);
	host.audio().set_bgm(sounds::BGM_PACMAN, sounds::BGM_PACMAN_COUNT);
}

uint32_t PacmanScreen::next_rng() {
	_rng ^= _rng << 13U;
	_rng ^= _rng >> 17U;
	_rng ^= _rng << 5U;
	return _rng;
}

void PacmanScreen::update_particles() {
	for (uint8_t i = 0; i < _death_particle_count; ) {
		_death_particles[i].x += _death_particles[i].vx;
		_death_particles[i].y += _death_particles[i].vy;
		if (_death_particles[i].life == 0) {
			_death_particles[i] = _death_particles[_death_particle_count - 1];
			--_death_particle_count;
		} else {
			--_death_particles[i].life;
			++i;
		}
	}
}

bool PacmanScreen::is_valid_cell(int8_t x, int8_t y) {
	return x >= 0 && x < GRID && y >= 0 && y < GRID;
}

bool PacmanScreen::is_wall(int8_t x, int8_t y) const {
	return !is_valid_cell(x, y) || MAZE[y][x] == 'W';
}

int8_t PacmanScreen::manhattan(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
	int16_t dx = static_cast<int16_t>(x1) - static_cast<int16_t>(x2);
	int16_t dy = static_cast<int16_t>(y1) - static_cast<int16_t>(y2);
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	return static_cast<int8_t>(dx + dy);
}

bool PacmanScreen::try_move(int8_t x, int8_t y, Dir dir, int8_t& nx, int8_t& ny) const {
	nx = x; ny = y;
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
	_pac_x = PAC_START_X; _pac_y = PAC_START_Y;
	_pac_dir = Dir::LEFT; _pac_next = Dir::LEFT;
	_state = State::PLAYING;
	_paused = false;
	_score = 0; _lives = INIT_LIVES;
	_power_timer = 0;
	_pac_move_counter = 0; _ghost_move_counter = 0;
	_dying_timer = 0; _frame = 0; _rng = RNG_SEED;
	_death_particle_count = 0; _celebration_timer = 0;

	_dot_count = 0;
	for (int8_t y = 0; y < GRID; ++y) {
		for (int8_t x = 0; x < GRID; ++x) {
			char c = MAZE[y][x];
			_dots[y][x] = (c == '.' || c == 'P');
			_power[y][x] = (c == 'P');
			if (_dots[y][x]) ++_dot_count;
		}
	}
	if (_dots[4][4]) { _dots[4][4] = false; --_dot_count; }
	if (_dots[5][4]) { _dots[5][4] = false; --_dot_count; }

	_ghosts[0] = {GHOST0_X, GHOST0_Y, Dir::UP, GhostState::CHASE};
	_ghosts[1] = {GHOST1_X, GHOST1_Y, Dir::UP, GhostState::CHASE};
}

void PacmanScreen::move_pacman(IScreenHost& host) {
	Dir nd = _pac_next;
	if ((nd == Dir::UP    && _pac_dir != Dir::DOWN) ||
		(nd == Dir::DOWN  && _pac_dir != Dir::UP) ||
		(nd == Dir::LEFT  && _pac_dir != Dir::RIGHT) ||
		(nd == Dir::RIGHT && _pac_dir != Dir::LEFT)) {
		int8_t nx = 0;
		int8_t ny = 0;
		if (try_move(_pac_x, _pac_y, nd, nx, ny)) _pac_dir = nd;
	}
	int8_t nx = 0;
	int8_t ny = 0;
	if (try_move(_pac_x, _pac_y, _pac_dir, nx, ny)) { _pac_x = nx; _pac_y = ny; }

	if (_dots[_pac_y][_pac_x]) {
		_dots[_pac_y][_pac_x] = false; --_dot_count;
		_score += DOT_SCORE;
		if (_power[_pac_y][_pac_x]) {
			_power[_pac_y][_pac_x] = false; _power_timer = POWER_DURATION;
			host.audio().play_sfx(sounds::SFX_POWER_PELLET, sounds::SFX_POWER_PELLET_COUNT);
			for (auto& g : _ghosts) {
				if (g.state == GhostState::CHASE) {
					g.state = GhostState::VULNERABLE;
					switch (g.dir) {
						case Dir::UP: g.dir = Dir::DOWN; break;
						case Dir::DOWN: g.dir = Dir::UP; break;
						case Dir::LEFT: g.dir = Dir::RIGHT; break;
						case Dir::RIGHT: g.dir = Dir::LEFT; break;
						default: break;
					}
				}
			}
		} else {
			host.audio().play_sfx(sounds::SFX_CHOMP, sounds::SFX_CHOMP_COUNT);
		}
	}
}

void PacmanScreen::move_ghosts() { /* unchanged logic */
	for (auto& g : _ghosts) {
		if (g.state == GhostState::RETURNING && g.x == 4 && g.y == 4) g.state = GhostState::CHASE;
		if (g.state == GhostState::VULNERABLE && (_frame % 10) != 0) continue;

		Dir valid[4]; int8_t vcount = 0;
		Dir opposites[] = {Dir::DOWN, Dir::UP, Dir::RIGHT, Dir::LEFT};
		Dir all_dirs[] = {Dir::UP, Dir::DOWN, Dir::LEFT, Dir::RIGHT};

		for (auto& ad : all_dirs) {
			if (ad == opposites[static_cast<int>(g.dir)]) continue;
			int8_t nx = 0;
			int8_t ny = 0;
			if (try_move(g.x, g.y, ad, nx, ny)) {
				if (MAZE[ny][nx] == 'O') continue;
				valid[vcount++] = ad;
			}
		}
		if (vcount == 0) continue;

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
					if (d < best_dist) { best_dist = d; best = valid[i]; }
				}
				if ((next_rng() % 2) == 0) best = valid[next_rng() % vcount];
			}
		}
		g.dir = best;
		switch (g.dir) {
			case Dir::UP: g.y--; break; case Dir::DOWN: g.y++; break;
			case Dir::LEFT: g.x--; break; case Dir::RIGHT: g.x++; break;
			default: break;
		}
	}
}

void PacmanScreen::check_ghost_collision(IScreenHost& host) {
	for (auto& g : _ghosts) {
		if (g.x == _pac_x && g.y == _pac_y) {
			if (g.state == GhostState::VULNERABLE) {
				g.state = GhostState::RETURNING; g.x = 4; g.y = 4; g.dir = Dir::UP;
				_score += GHOST_SCORE;
			} else if (g.state == GhostState::CHASE) { die(host); return; }
		}
	}
}

void PacmanScreen::die(IScreenHost& host) {
	_state = State::DYING; _dying_timer = DYING_TIMER; --_lives; _power_timer = 0;
	host.audio().play_sfx(sounds::SFX_DEATH_PACMAN, sounds::SFX_DEATH_PACMAN_COUNT);
	auto px = static_cast<int16_t>((_pac_x * CELL) + (CELL / 2));
	auto py = static_cast<int16_t>((_pac_y * CELL) + (CELL / 2));
	for (int16_t i = 0; i < 8 && _death_particle_count < MAX_DEATH_PARTICLES; ++i) {
		uint8_t idx = _death_particle_count++;
		_death_particles[idx].x = static_cast<int8_t>(px);
		_death_particles[idx].y = static_cast<int8_t>(py);
		_death_particles[idx].vx = static_cast<int8_t>((next_rng() % 5) - 2);
		_death_particles[idx].vy = static_cast<int8_t>((next_rng() % 5) - 2);
		_death_particles[idx].life = static_cast<uint8_t>(5 + (next_rng() % 6));
	}
}

void PacmanScreen::update(IPlatform& platform, IScreenHost& host) {
	++_frame;
	auto& input = platform.input();
	update_particles();
	if (_celebration_timer > 0) {
		--_celebration_timer;
		if (_celebration_timer == 0) _state = State::GAME_OVER;
	}

	if (_paused) {
		if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
		if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
		return;
	}

	if (_state == State::GAME_OVER) {
		if (input.was_pressed(ButtonBits::START)) reset_game();
		return;
	}

	if (_state == State::PLAYING && input.was_pressed(ButtonBits::START)) {
		_paused = true; return;
	}

	if (_state == State::DYING) {
		if (_dying_timer > 0) { --_dying_timer; }
		else {
			if (_lives <= 0) { _state = State::GAME_OVER; }
			else {
				_pac_x = PAC_START_X; _pac_y = PAC_START_Y;
				_pac_dir = Dir::LEFT; _pac_next = Dir::LEFT;
				_ghosts[0] = {GHOST0_X, GHOST0_Y, Dir::UP, GhostState::CHASE};
				_ghosts[1] = {GHOST1_X, GHOST1_Y, Dir::UP, GhostState::CHASE};
				_power_timer = 0; _pac_move_counter = 0; _ghost_move_counter = 0;
				_state = State::PLAYING;
			}
		}
		return;
	}

	if (input.was_pressed(ButtonBits::UP))       _pac_next = Dir::UP;
	else if (input.was_pressed(ButtonBits::DOWN))  _pac_next = Dir::DOWN;
	else if (input.was_pressed(ButtonBits::LEFT))  _pac_next = Dir::LEFT;
	else if (input.was_pressed(ButtonBits::RIGHT)) _pac_next = Dir::RIGHT;

	++_pac_move_counter;
	if (_pac_move_counter >= PAC_MOVE_INTERVAL) { _pac_move_counter = 0; move_pacman(host); }
	++_ghost_move_counter;
	if (_ghost_move_counter >= GHOST_MOVE_INTERVAL) { _ghost_move_counter = 0; move_ghosts(); }

	if (_power_timer > 0) {
		--_power_timer;
		if (_power_timer == 0) {
			for (auto& g : _ghosts) {
				if (g.state == GhostState::VULNERABLE) g.state = GhostState::CHASE;
			}
		}
	}
	check_ghost_collision(host);
}

void PacmanScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(BG_COLOR);
	display.draw_rect({0, 0, 80, 80}, MAZE_BORDER);

	for (int8_t y = 0; y < GRID; ++y) {
		for (int8_t x = 0; x < GRID; ++x) {
			const auto px = static_cast<int16_t>(x * CELL);
			const auto py = static_cast<int16_t>(y * CELL);
			if (MAZE[y][x] == 'W') display.fill_rect(Rect{px, py, CELL, CELL}, WALL_COLOR);
			if (_dots[y][x]) {
				if (_power[y][x]) {
					if ((_frame / 8) % 2 == 0) {
						display.fill_rect(Rect{static_cast<int16_t>(px + 2), static_cast<int16_t>(py + 2),
											static_cast<int16_t>(CELL - 4), static_cast<int16_t>(CELL - 4)}, POWER_COLOR);
}
					if ((_frame / 4) % 2 == 0) {
						display.draw_rect(Rect{static_cast<int16_t>(px + 1), static_cast<int16_t>(py + 1),
											static_cast<int16_t>(CELL - 2), static_cast<int16_t>(CELL - 2)}, AURA_COLOR);
}
				} else {
					display.fill_rect(Rect{static_cast<int16_t>(px + 3), static_cast<int16_t>(py + 3),
										static_cast<int16_t>(CELL - 6), static_cast<int16_t>(CELL - 6)}, DOT_COLOR);
				}
			}
		}
	}

	for (const auto& g : _ghosts) {
		const auto gx = static_cast<int16_t>(g.x * CELL);
		const auto gy = static_cast<int16_t>(g.y * CELL);
		if (g.state == GhostState::RETURNING) {
			display.draw_rect(Rect{static_cast<int16_t>(gx + 1), static_cast<int16_t>(gy + 1), 6, 6}, GHOST1_COL);
			display.draw_pixel(static_cast<int16_t>(gx + 2), static_cast<int16_t>(gy + 2), Color::WHITE);
			display.draw_pixel(static_cast<int16_t>(gx + 5), static_cast<int16_t>(gy + 2), Color::WHITE);
			continue;
		}
		Color gcol;
		if (g.state == GhostState::VULNERABLE) {
			gcol = (_power_timer < 40 && (_frame / 8) % 2 == 0) ? VULN_BLINK : VULN_COLOR;
		} else {
			gcol = (&g == &_ghosts[0]) ? GHOST1_COL : GHOST2_COL;
		}
		display.fill_rect(Rect{static_cast<int16_t>(gx + 1), static_cast<int16_t>(gy + 1), 6, 6}, gcol);
		int16_t ex1_x = gx + 2;
		int16_t ex1_y = gy + 2;
		int16_t ex2_x = gx + 5;
		int16_t ex2_y = gy + 2;
		switch (g.dir) {
			case Dir::UP:    ex1_y = gy + 1; ex2_y = gy + 1; break;
			case Dir::DOWN:  ex1_y = gy + 3; ex2_y = gy + 3; break;
			case Dir::LEFT:  ex1_x = gx + 1; ex2_x = gx + 4; break;
			case Dir::RIGHT: ex1_x = gx + 3; ex2_x = gx + 6; break;
			default: break;
		}
		display.draw_pixel(ex1_x, ex1_y, Color::WHITE);
		display.draw_pixel(ex2_x, ex2_y, Color::WHITE);
	}

	if (_state != State::DYING || (_dying_timer / 4) % 2 == 0) {
		const auto px = static_cast<int16_t>(_pac_x * CELL);
		const auto py = static_cast<int16_t>(_pac_y * CELL);
		display.fill_rect(Rect{static_cast<int16_t>(px + 1), static_cast<int16_t>(py + 1), 6, 6}, PAC_COLOR);
		if ((_frame / 3) % 2 == 0) {
			switch (_pac_dir) {
				case Dir::RIGHT: display.fill_rect(Rect{static_cast<int16_t>(px + 5), static_cast<int16_t>(py + 2), 2, 4}, BG_COLOR); break;
				case Dir::LEFT:  display.fill_rect(Rect{static_cast<int16_t>(px + 1), static_cast<int16_t>(py + 2), 2, 4}, BG_COLOR); break;
				case Dir::UP:    display.fill_rect(Rect{static_cast<int16_t>(px + 2), static_cast<int16_t>(py + 1), 4, 2}, BG_COLOR); break;
				case Dir::DOWN:  display.fill_rect(Rect{static_cast<int16_t>(px + 2), static_cast<int16_t>(py + 5), 4, 2}, BG_COLOR); break;
				default: break;
			}
		}
	}

	for (uint8_t i = 0; i < _death_particle_count; ++i) {
		Color pc = (_death_particles[i].life % 2 == 0) ? PARTICLE_COLOR : PAC_COLOR;
		display.draw_pixel(_death_particles[i].x, _death_particles[i].y, pc);
	}

	char buf[20];
	std::snprintf(buf, sizeof(buf), "%d", _score);
	TextRenderer::draw_text(display, {2, 74}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
	for (int8_t i = 0; i < _lives; ++i)
		display.fill_rect(Rect{static_cast<int16_t>(66 + (i * 5)), 74, 3, 3}, PAC_COLOR);

	if (_dot_count == 0 && _state != State::GAME_OVER) _celebration_timer = CELEBRATION_DURATION;
	if (_celebration_timer > 0) {
		for (uint8_t i = 0; i < 5; ++i) {
			auto sx = static_cast<int16_t>(next_rng() % 80);
			auto sy = static_cast<int16_t>(next_rng() % 80);
			display.draw_pixel(sx, sy, ((_frame + i) % 3 == 0) ? WIN_COLOR : PAC_COLOR);
		}
	}

	if (_dot_count == 0) {
		display.fill_rect(Rect{10, 28, 60, 24}, BG_COLOR);
		display.draw_rect(Rect{10, 28, 60, 24}, WIN_COLOR);
		TextRenderer::draw_text_centered(display, {40, 34}, "YOU WIN!", WIN_COLOR, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 46}, "START=AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
		_state = State::GAME_OVER;
	}

	if (_state == State::GAME_OVER && _dot_count > 0) {
		display.fill_rect(Rect{10, 24, 60, 32}, BG_COLOR);
		display.draw_rect(Rect{10, 24, 60, 32}, GHOST1_COL);
		TextRenderer::draw_text_centered(display, {40, 30}, "GAME OVER", GHOST1_COL, 1, BASIC_FONT_5X7);
		std::snprintf(buf, sizeof(buf), "SCORE: %d", _score);
		TextRenderer::draw_text_centered(display, {40, 44}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text_centered(display, {40, 54}, "START=AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
	}

	// 暂停覆盖层 (吃豆人主题: 黄蓝配色)
	if (_paused) {
		display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
		display.draw_rect({10, 22, 60, 36}, PAC_COLOR);
		TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
		TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text_centered(display, {40, 54}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
	}
}

}  // namespace handheld
