#include "scenes/pacman/PacmanScreen.h"

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

constexpr int16_t S6 = 6;

// ── 精灵数据（6x6 像素）──────────────────────────────────────

// 幽灵身体：弧顶 + 三波谷裙边
constexpr char GHOST_SPRITE[S6][S6 + 1] = {
    "..XXX.", 
    ".XXXXX", 
    ".XXXXX", 
    ".XXXXX", 
    ".X.X.X", 
    "......",
};

// 吃豆人闭嘴（全方向通用）
constexpr char PAC_CLOSED[S6][S6 + 1] = {
    "..XXX.", 
    ".XXXXX", 
    ".XXXXX", 
    ".XXXXX", 
    "..XXX.", 
    "......",
};

// 吃豆人张嘴 — 右
constexpr char PAC_OPEN_R[S6][S6 + 1] = {
    "..XXXX", 
    ".XXXX.", 
    ".XXX..", 
    ".XXXX.", 
    "..XXXX", 
    "......",
};

// 吃豆人张嘴 — 左
constexpr char PAC_OPEN_L[S6][S6 + 1] = {
    ".XXXX.",
    "..XXXX",
    "...XXX",
    "..XXXX",
    ".XXXX.",
    "......",
};

// 吃豆人张嘴 — 上（居中窄楔形）
constexpr char PAC_OPEN_U[S6][S6 + 1] = {
    ".X...X",
    ".XX.XX",
    ".XXXXX",
    ".XXXXX",
    "..XXX.",
    "......",
};

// 吃豆人张嘴 — 下
constexpr char PAC_OPEN_D[S6][S6 + 1] = {
    "..XXX.",
    ".XXXXX",
    ".XXXXX",
    ".XX.XX",
    ".X...X",
    "......",
};

namespace {

void draw_sprite(IDisplay& display, int16_t x, int16_t y, const char sprite[S6][S6 + 1], Color color) {
    for (int16_t dy = 0; dy < S6; ++dy)
        for (int16_t dx = 0; dx < S6; ++dx)
            if (sprite[dy][dx] == 'X')
                display.draw_pixel(static_cast<int16_t>(x + dx), static_cast<int16_t>(y + dy), color);
}

} // namespace
} // namespace

void PacmanScreen::enter(IPlatform& platform, IScreenHost& host) {
    _level = 0;
    reset_game();
    platform.display().clear(BG_COLOR);
    if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_PACMAN, sounds::BGM_PACMAN_COUNT);
}

uint32_t PacmanScreen::next_rng() {
    _rng ^= _rng << 13U;
    _rng ^= _rng >> 17U;
    _rng ^= _rng << 5U;
    return _rng;
}

void PacmanScreen::update_particles() {
    for (uint8_t i = 0; i < _death_particle_count;) {
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

bool PacmanScreen::is_valid_cell(int8_t x, int8_t y) { return x >= 0 && x < GRID && y >= 0 && y < GRID; }

bool PacmanScreen::is_wall(int8_t x, int8_t y) const {
    return !is_valid_cell(x, y) || LEVELS[_level].maze[y][x] == 'W';
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
        case PacmanScreen::Dir::UP: ny = static_cast<int8_t>(y - 1); break;
        case PacmanScreen::Dir::DOWN: ny = static_cast<int8_t>(y + 1); break;
        case PacmanScreen::Dir::LEFT: nx = static_cast<int8_t>(x - 1); break;
        case PacmanScreen::Dir::RIGHT: nx = static_cast<int8_t>(x + 1); break;
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
    _score = 0;
    _lives = INIT_LIVES;
    _rng = RNG_SEED;
    _frame = 0;
    _paused = false;
    _death_particle_count = 0;
    _celebration_timer = 0;
    _dying_timer = 0;

    load_level(_level);
}

void PacmanScreen::load_level(uint8_t level) {
    _dot_count = 0;
    for (int8_t y = 0; y < GRID; ++y) {
        for (int8_t x = 0; x < GRID; ++x) {
            char c = LEVELS[level].maze[y][x];
            _dots[y][x] = (c == '.' || c == 'P');
            _power[y][x] = (c == 'P');
            if (_dots[y][x]) ++_dot_count;
        }
    }
    if (_dots[4][4]) {
        _dots[4][4] = false;
        --_dot_count;
    }
    if (_dots[5][4]) {
        _dots[5][4] = false;
        --_dot_count;
    }

    _pac_x = PAC_START_X;
    _pac_y = PAC_START_Y;
    _pac_dir = Dir::LEFT;
    _pac_next = Dir::LEFT;
    _ghosts[0] = {GHOST0_X, GHOST0_Y, Dir::UP, GhostState::CHASE};
    _ghosts[1] = {GHOST1_X, GHOST1_Y, Dir::UP, GhostState::CHASE};
    _power_timer = 0;
    _pac_move_counter = 0;
    _ghost_move_counter = 0;
    _death_particle_count = 0;
	_lives = INIT_LIVES;
    _state = State::PLAYING;
}

void PacmanScreen::move_pacman(IScreenHost& host) {
    Dir nd = _pac_next;
    if ((nd == Dir::UP && _pac_dir != Dir::DOWN) || (nd == Dir::DOWN && _pac_dir != Dir::UP) ||
        (nd == Dir::LEFT && _pac_dir != Dir::RIGHT) || (nd == Dir::RIGHT && _pac_dir != Dir::LEFT)) {
        int8_t nx = 0;
        int8_t ny = 0;
        if (try_move(_pac_x, _pac_y, nd, nx, ny)) _pac_dir = nd;
    }
    int8_t nx = 0;
    int8_t ny = 0;
    if (try_move(_pac_x, _pac_y, _pac_dir, nx, ny)) {
        _pac_x = nx;
        _pac_y = ny;
    }

    if (_dots[_pac_y][_pac_x]) {
        _dots[_pac_y][_pac_x] = false;
        --_dot_count;
        _score += DOT_SCORE;
        if (_power[_pac_y][_pac_x]) {
            _power[_pac_y][_pac_x] = false;
            _power_timer = LEVELS[_level].power_duration;
            host.audio().play_sfx(sounds::SFX_POWER_PELLET, sounds::SFX_POWER_PELLET_COUNT);
            for (auto& g : _ghosts) {
                if (g.state == GhostState::CHASE) {
                    g.state = GhostState::VULNERABLE;
                    switch (g.dir) {
                        case PacmanScreen::Dir::UP: g.dir = Dir::DOWN; break;
                        case PacmanScreen::Dir::DOWN: g.dir = Dir::UP; break;
                        case PacmanScreen::Dir::LEFT: g.dir = Dir::RIGHT; break;
                        case PacmanScreen::Dir::RIGHT: g.dir = Dir::LEFT; break;
                        default: break;
                    }
                }
            }
        } else {
            host.audio().play_sfx(sounds::SFX_CHOMP, sounds::SFX_CHOMP_COUNT);
        }
        if (_dot_count == 0) host.audio().play_sfx(sounds::SFX_LEVEL_UP, sounds::SFX_LEVEL_UP_COUNT);
    }
}

void PacmanScreen::move_ghosts() { /* unchanged logic */
    for (auto& g : _ghosts) {
        if (g.state == GhostState::RETURNING && g.x == 4 && g.y == 4) g.state = GhostState::CHASE;
        if (g.state == GhostState::VULNERABLE && (_frame % 10) != 0) continue;

        Dir valid[4];
        int8_t vcount = 0;
        Dir opposites[] = {Dir::DOWN, Dir::UP, Dir::RIGHT, Dir::LEFT};
        Dir all_dirs[] = {Dir::UP, Dir::DOWN, Dir::LEFT, Dir::RIGHT};

        for (auto& ad : all_dirs) {
            if (ad == opposites[static_cast<int>(g.dir)]) continue;
            int8_t nx = 0;
            int8_t ny = 0;
            if (try_move(g.x, g.y, ad, nx, ny)) {
                if (LEVELS[_level].maze[ny][nx] == 'O') continue;
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
                    if (d < best_dist) {
                        best_dist = d;
                        best = valid[i];
                    }
                }
                if ((next_rng() % 2) == 0) best = valid[next_rng() % vcount];
            }
        }
        g.dir = best;
        switch (g.dir) {
            case PacmanScreen::Dir::UP: g.y--; break;
            case PacmanScreen::Dir::DOWN: g.y++; break;
            case PacmanScreen::Dir::LEFT: g.x--; break;
            case PacmanScreen::Dir::RIGHT: g.x++; break;
            default: break;
        }
    }
}

void PacmanScreen::check_ghost_collision(IScreenHost& host) {
    for (auto& g : _ghosts) {
        if (g.x == _pac_x && g.y == _pac_y) {
            if (g.state == GhostState::VULNERABLE) {
                g.state = GhostState::RETURNING;
                g.x = 4;
                g.y = 4;
                g.dir = Dir::UP;
                _score += GHOST_SCORE;
                host.audio().play_sfx(sounds::SFX_GHOST_EATEN, sounds::SFX_GHOST_EATEN_COUNT);
            } else if (g.state == GhostState::CHASE) {
                die(host);
                return;
            }
        }
    }
}

void PacmanScreen::die(IScreenHost& host) {
    _state = State::DYING;
    _dying_timer = DYING_TIMER;
    --_lives;
    _power_timer = 0;
    host.audio().play_sfx(sounds::SFX_DEATH_PACMAN, sounds::SFX_DEATH_PACMAN_COUNT);
    auto px = static_cast<int16_t>((_pac_x * CELL) + (CELL / 2));
    auto py = static_cast<int16_t>(STATUS_H + (_pac_y * CELL) + (CELL / 2));
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
        if (_celebration_timer == 0) {
            if (_level + 1 < LEVEL_COUNT) {
                ++_level;
                load_level(_level);
            } else {
                _state = State::GAME_OVER;
            }
        }
        return;
    }

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (_state == State::GAME_OVER) {
        if (input.was_pressed(ButtonBits::START)) {
            _level = 0;
            reset_game();
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (_state == State::PLAYING && input.was_pressed(ButtonBits::START)) {
        _paused = true;
        return;
    }

    if (_state == State::DYING) {
        if (_dying_timer > 0) {
            --_dying_timer;
        } else {
            if (_lives <= 0) {
                _state = State::GAME_OVER;
            } else {
                _pac_x = PAC_START_X;
                _pac_y = PAC_START_Y;
                _pac_dir = Dir::LEFT;
                _pac_next = Dir::LEFT;
                _ghosts[0] = {GHOST0_X, GHOST0_Y, Dir::UP, GhostState::CHASE};
                _ghosts[1] = {GHOST1_X, GHOST1_Y, Dir::UP, GhostState::CHASE};
                _power_timer = 0;
                _pac_move_counter = 0;
                _ghost_move_counter = 0;
                _state = State::PLAYING;
            }
        }
        return;
    }

    if (input.was_pressed(ButtonBits::UP)) _pac_next = Dir::UP;
    else if (input.was_pressed(ButtonBits::DOWN)) _pac_next = Dir::DOWN;
    else if (input.was_pressed(ButtonBits::LEFT)) _pac_next = Dir::LEFT;
    else if (input.was_pressed(ButtonBits::RIGHT)) _pac_next = Dir::RIGHT;

    ++_pac_move_counter;
    if (_pac_move_counter >= PAC_MOVE_INTERVAL) {
        _pac_move_counter = 0;
        move_pacman(host);
    }
    ++_ghost_move_counter;
    if (_ghost_move_counter >= LEVELS[_level].ghost_interval) {
        _ghost_move_counter = 0;
        move_ghosts();
    }

    if (_power_timer > 0) {
        --_power_timer;
        if (_power_timer == 0) {
            for (auto& g : _ghosts) {
                if (g.state == GhostState::VULNERABLE) g.state = GhostState::CHASE;
            }
        }
    }

    if (_dot_count == 0 && _state == State::PLAYING) {
        _celebration_timer = CELEBRATION_DURATION;
        return;
    }

    if (_dot_count > 0) check_ghost_collision(host);
}

void PacmanScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG_COLOR);

    // ── 迷宫 ──────────────────────────────────────────────
    display.draw_rect({0, STATUS_H, 80, 80}, MAZE_BORDER);

    for (int8_t y = 0; y < GRID; ++y) {
        for (int8_t x = 0; x < GRID; ++x) {
            const auto px = static_cast<int16_t>(x * CELL);
            const auto py = static_cast<int16_t>(STATUS_H / 2 + y * CELL);
            if (LEVELS[_level].maze[y][x] == 'W') display.fill_rect(Rect{px, py, CELL, CELL}, WALL_COLOR);
            if (_dots[y][x]) {
                if (_power[y][x]) {
                    if ((_frame / 8) % 2 == 0) {
                        display.fill_rect(Rect{static_cast<int16_t>(px + 2), static_cast<int16_t>(py + 2),
                                               static_cast<int16_t>(CELL - 4), static_cast<int16_t>(CELL - 4)},
                                          POWER_COLOR);
                    }
                    if ((_frame / 4) % 2 == 0) {
                        display.draw_rect(Rect{static_cast<int16_t>(px + 1), static_cast<int16_t>(py + 1),
                                               static_cast<int16_t>(CELL - 2), static_cast<int16_t>(CELL - 2)},
                                          AURA_COLOR);
                    }
                } else {
                    display.fill_rect(Rect{static_cast<int16_t>(px + 3), static_cast<int16_t>(py + 3),
                                           static_cast<int16_t>(CELL - 6), static_cast<int16_t>(CELL - 6)},
                                      DOT_COLOR);
                }
            }
        }
    }

    // ── 状态栏（顶部）──────────────────────────────────────
    // display.fill_rect(Rect{0, 0, 80, STATUS_H}, STATUS_BG);
    char buf[20];
    std::snprintf(buf, sizeof(buf), "%s %d", LEVELS[_level].name, _score);
    TextRenderer::draw_text(display, {2, 1}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
    for (int8_t i = 0; i < _lives; ++i)
        display.fill_rect(Rect{static_cast<int16_t>(66 + (i * 5)), 2, 3, 3}, PAC_COLOR);

    // ── 幽灵 ──────────────────────────────────────────────
    for (const auto& g : _ghosts) {
        const auto gx = static_cast<int16_t>(g.x * CELL + 1);
        const auto gy = static_cast<int16_t>(STATUS_H / 2 + g.y * CELL + 1);
        if (g.state == GhostState::RETURNING) {
            display.draw_rect(Rect{static_cast<int16_t>(gx), static_cast<int16_t>(gy), 6, 6}, GHOST1_COL);
            display.draw_pixel(static_cast<int16_t>(gx + 2), static_cast<int16_t>(gy + 2), Color::WHITE);
            continue;
        }
        Color gcol;
        if (g.state == GhostState::VULNERABLE) {
            gcol = (_power_timer < 40 && (_frame / 8) % 2 == 0) ? VULN_BLINK : VULN_COLOR;
        } else {
            gcol = (&g == &_ghosts[0]) ? GHOST1_COL : GHOST2_COL;
        }
        draw_sprite(display, gx, gy, GHOST_SPRITE, gcol);
        // 双像素白眼（随方向移动）
        int16_t lx = gx + 2, rx = gx + 4;
        int16_t ly = gy + 2, ry = gy + 2;
        switch (g.dir) {
            case PacmanScreen::Dir::UP:
                ly = gy + 1;
                ry = gy + 1;
                break;
            case PacmanScreen::Dir::DOWN:
                ly = gy + 3;
                ry = gy + 3;
                break;
            case PacmanScreen::Dir::LEFT:
                lx = gx + 1;
                rx = gx + 3;
                break;
            case PacmanScreen::Dir::RIGHT:
                lx = gx + 3;
                rx = gx + 5;
                break;
            default: break;
        }
        display.draw_pixel(lx, ly, Color::WHITE);
        display.draw_pixel(rx, ry, Color::WHITE);
    }

    // ── 吃豆人 ────────────────────────────────────────────
    if (_state != State::DYING || (_dying_timer / 4) % 2 == 0) {
        const auto px = static_cast<int16_t>(_pac_x * CELL + 1);
        const auto py = static_cast<int16_t>(STATUS_H / 2 + _pac_y * CELL + 1);
        bool mouth_open = (_frame / 3) % 2 == 0;
        const char(*sprite)[S6 + 1] = PAC_CLOSED;
        if (mouth_open) {
            switch (_pac_dir) {
                case Dir::RIGHT: sprite = PAC_OPEN_R; break;
                case Dir::LEFT: sprite = PAC_OPEN_L; break;
                case Dir::UP: sprite = PAC_OPEN_U; break;
                case Dir::DOWN: sprite = PAC_OPEN_D; break;
                default: break;
            }
        }
        draw_sprite(display, px, py, sprite, PAC_COLOR);
    }

    // ── 死亡粒子 ──────────────────────────────────────────
    for (uint8_t i = 0; i < _death_particle_count; ++i) {
        Color pc = (_death_particles[i].life % 2 == 0) ? PARTICLE_COLOR : PAC_COLOR;
        display.draw_pixel(_death_particles[i].x, _death_particles[i].y, pc);
    }

    // ── 庆祝粒子 ──────────────────────────────────────────
    if (_celebration_timer > 0) {
        for (uint8_t i = 0; i < 5; ++i) {
            auto sx = static_cast<int16_t>(next_rng() % 80);
            auto sy = static_cast<int16_t>(next_rng() % 80);
            display.draw_pixel(sx, sy, ((_frame + i) % 3 == 0) ? WIN_COLOR : PAC_COLOR);
        }
    }

    // ── 全部通关覆盖层 ────────────────────────────────────
    if (_state == State::GAME_OVER && _dot_count == 0) {
        display.fill_rect(Rect{WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, BG_COLOR);
        display.draw_rect(Rect{WIN_RECT_X, WIN_RECT_Y, WIN_RECT_W, WIN_RECT_H}, WIN_COLOR);
        TextRenderer::draw_text_centered(display, {40, 28}, "YOU WIN!", WIN_COLOR, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "START: AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    // ── 游戏结束覆盖层（生命耗尽）─────────────────────────
    if (_state == State::GAME_OVER && _dot_count > 0) {
        display.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, BG_COLOR);
        display.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, GHOST1_COL);
        TextRenderer::draw_text_centered(display, {40, 28}, "GAME OVER", GHOST1_COL, 1, BASIC_FONT_5X7);
        std::snprintf(buf, sizeof(buf), "SCORE: %d", _score);
        TextRenderer::draw_text_centered(display, {40, 42}, buf, Color::WHITE, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 50}, "START: AGAIN", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 58}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    // ── 暂停覆盖层 ────────────────────────────────────────
    if (_paused) {
        display.fill_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BG);
        display.draw_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAC_COLOR);
        TextRenderer::draw_text_centered(display, {40, 28}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

void PacmanScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    const auto cx = static_cast<int16_t>(box.x + box.width / 2 - 3);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2 - 3);
    const Color pac = rgb565(240, 210, 50);
    bool mouth_open = (frame / 3) % 2 == 0;
    const char(*sprite)[S6 + 1] = mouth_open ? PAC_OPEN_R : PAC_CLOSED;
    draw_sprite(display, cx, cy, sprite, pac);
}

} // namespace handheld
