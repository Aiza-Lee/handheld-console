#include "scenes/invaders/InvadersScreen.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include <algorithm>
#include "core/audio/Sounds.h"

namespace handheld {

using namespace invaders::cfg;

namespace {

Color enemy_row_color(int16_t row) {
    switch (row) {
        case 0: return INV_E_ROW0;
        case 1: return INV_E_ROW1;
        case 2: return INV_E_ROW2;
        default: return INV_E_ROW3;
    }
}

int16_t enemy_row_score(int16_t row) { return ROW_SCORES[row]; }

} // namespace

// ── 生命周期 ──────────────────────────────────────────────────────

void InvadersScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(Color::BLACK);
    _score = 0;
    _level = 1;
    reset_game();
}

void InvadersScreen::reset_game() {
    for (int16_t i = 0; i < TOTAL_ENEMIES; ++i) _enemy_alive[i] = true;
    _enemy_base_x = GRID_OX;
    _enemy_base_y = static_cast<int16_t>(GRID_OY + (_level - 1) * LEVEL_Y_OFFSET_PER_LEVEL);
    _enemy_dir_x = 1;
    _enemies_alive_count = TOTAL_ENEMIES;
    _player_x = PLAYER_START_X;
    _state = State::PLAYING;
    _paused = false;
    _bullet_active = false;
    for (auto& eb : _ebullets) eb.active = false;
    _saucer.alive = false;
    _saucer_score_display = 0;
    _saucer_timer = SAUCER_MIN_INTERVAL + (next_rng() % (SAUCER_MAX_INTERVAL - SAUCER_MIN_INTERVAL));
    _particle_count = 0;
    _lives = INIT_LIVES;
    _level_clear_timer = 0;
    _frame = 0;
    _move_timer = 0;
    int16_t interval = static_cast<int16_t>(INIT_MOVE_INTERVAL - (_level - 1) * LEVEL_SPEED_STEP);
    interval = std::max(interval, static_cast<int16_t>(MIN_MOVE_INTERVAL));
    _move_interval = static_cast<uint32_t>(interval);
    _shoot_timer = 0;
    _dying_timer = 0;
    _rng = 12345;
    init_shields();
}

void InvadersScreen::start_next_level() {
    ++_level;
    if (_level > MAX_VICTORY_LEVEL) {
        _state = State::GAME_OVER;
        _enemies_alive_count = 0;
        return;
    }
    reset_game();
}

void InvadersScreen::respawn_player() {
    _player_x = PLAYER_START_X;
    _bullet_active = false;
    for (auto& eb : _ebullets) eb.active = false;
    _dying_timer = 0;
    _state = State::PLAYING;
}

// ── 工具 ──────────────────────────────────────────────────────────

uint32_t InvadersScreen::next_rng() {
    _rng = _rng * 1103515245 + 12345;
    return _rng;
}

// ── 射击 ──────────────────────────────────────────────────────────

void InvadersScreen::player_shoot(IScreenHost& host) {
    if (!_bullet_active) {
        _bullet_x = static_cast<int16_t>(_player_x + PLAYER_W / 2);
        _bullet_y = PLAYER_Y - 1;
        _bullet_active = true;
        host.audio().play_sfx(sounds::SFX_SHOOT, sounds::SFX_SHOOT_COUNT);
    }
}

void InvadersScreen::enemy_shoot(IScreenHost& host) {
    int16_t slot = -1;
    for (int16_t i = 0; i < MAX_EBULLETS; ++i) {
        if (!_ebullets[i].active) {
            slot = i;
            break;
        }
    }
    if (slot < 0) return;

    int16_t bottom_row = -1;
    for (int16_t row = GRID_ROWS - 1; row >= 0; --row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (_enemy_alive[row * GRID_COLS + col]) {
                bottom_row = row;
                break;
            }
        }
        if (bottom_row >= 0) break;
    }
    if (bottom_row < 0) return;

    int16_t alive_cols[GRID_COLS];
    int16_t alive_count = 0;
    for (int16_t col = 0; col < GRID_COLS; ++col) {
        if (_enemy_alive[bottom_row * GRID_COLS + col]) alive_cols[alive_count++] = col;
    }
    if (alive_count == 0) return;

    int16_t col = alive_cols[next_rng() % alive_count];
    _ebullets[slot].x = static_cast<int16_t>(_enemy_base_x + col * CELL_W + ENEMY_W / 2);
    _ebullets[slot].y = static_cast<int16_t>(_enemy_base_y + bottom_row * CELL_H + ENEMY_H);
    _ebullets[slot].active = true;
    host.audio().play_sfx(sounds::SFX_ENEMY_SHOOT, sounds::SFX_ENEMY_SHOOT_COUNT);
}

// ── 敌人移动 ──────────────────────────────────────────────────────

void InvadersScreen::move_enemies(IScreenHost&  /*host*/) {
    ++_move_timer;
    if (_move_timer < _move_interval) return;
    _move_timer = 0;

    // 以存活敌人的最外沿判断边界——边缘敌人被消灭后队形自动收窄，可移动范围增大
    bool hit_edge = false;
    for (int16_t row = 0; row < GRID_ROWS && !hit_edge; ++row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (!_enemy_alive[row * GRID_COLS + col]) continue;
            int16_t ex = _enemy_base_x + col * CELL_W;
            if (ex + ENEMY_W >= ENEMY_AREA_RIGHT || ex <= PLAY_AREA_LEFT) {
                hit_edge = true;
                break;
            }
        }
    }
    if (hit_edge) {
        _enemy_dir_x = static_cast<int8_t>(-_enemy_dir_x);
        _enemy_base_y += ENEMY_DROP;
        _enemy_base_x += _enemy_dir_x; // 离开边界，避免下一帧重复触边
    } else {
        _enemy_base_x += _enemy_dir_x;
    }

    // 护盾被敌人侵蚀（底部行敌人接触到护盾时破坏护盾顶部）
    int16_t shield_top = SHIELD_Y;
    for (int16_t row = 0; row < GRID_ROWS; ++row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (!_enemy_alive[row * GRID_COLS + col]) continue;
            int16_t ey = static_cast<int16_t>(_enemy_base_y + row * CELL_H + ENEMY_H);
            if (ey >= shield_top) {
                // 敌人已到达护盾高度，清除所有护盾像素
                for (int16_t s = 0; s < SHIELD_COUNT; ++s)
                    for (int16_t sy = 0; sy < SHIELD_H; ++sy)
                        for (int16_t sx = 0; sx < SHIELD_W; ++sx) _shields[s][sy][sx] = false;
            }
        }
    }

    // 敌人到达玩家位置
    for (int16_t row = 0; row < GRID_ROWS; ++row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (!_enemy_alive[row * GRID_COLS + col]) continue;
            if (_enemy_base_y + row * CELL_H + ENEMY_H >= PLAYER_Y) {
                _state = State::GAME_OVER;
                return;
            }
        }
    }
}

// ── 子弹移动 ──────────────────────────────────────────────────────

void InvadersScreen::move_bullets(IScreenHost& host) {
    // 玩家子弹
    if (_bullet_active) {
        _bullet_y -= PLAYER_BULLET_SPEED;
        if (_bullet_y < STATUS_H) _bullet_active = false;
    }
    if (_bullet_active) {
        // 检查是否击中敌人
        for (int16_t row = 0; row < GRID_ROWS; ++row) {
            for (int16_t col = 0; col < GRID_COLS; ++col) {
                if (!_enemy_alive[row * GRID_COLS + col]) continue;
                int16_t ex = _enemy_base_x + col * CELL_W;
                int16_t ey = _enemy_base_y + row * CELL_H;
                if (_bullet_x >= ex && _bullet_x < ex + ENEMY_W && _bullet_y >= ey && _bullet_y < ey + ENEMY_H) {
                    _enemy_alive[row * GRID_COLS + col] = false;
                    --_enemies_alive_count;
                    _score += enemy_row_score(row);
                    _bullet_active = false;
                    spawn_particles(static_cast<int16_t>(ex + ENEMY_W / 2), static_cast<int16_t>(ey + ENEMY_H / 2));
                    host.audio().play_sfx(sounds::SFX_EXPLOSION, sounds::SFX_EXPLOSION_COUNT);
                    _move_interval =
                        INIT_MOVE_INTERVAL - static_cast<uint32_t>((TOTAL_ENEMIES - _enemies_alive_count) / 4);
                    _move_interval = std::max(_move_interval, MIN_MOVE_INTERVAL);
                    break;
                }
            }
            if (!_bullet_active) break;
        }
        // 检查是否击中护盾（命中则停用子弹）
        if (_bullet_active && damage_shield(_bullet_x, _bullet_y)) {
            _bullet_active = false;
        }
    }

    // 检查是否击中神秘飞船
    if (_bullet_active && _saucer.alive) {
        if (_bullet_x >= _saucer.x && _bullet_x < _saucer.x + SAUCER_W && _bullet_y >= _saucer.y &&
            _bullet_y < _saucer.y + SAUCER_H) {
            _bullet_active = false;
            _saucer.alive = false;
            _score += _saucer.points;
            _saucer_score_display = SAUCER_SCORE_DISPLAY_FRAMES;
            _saucer_score_x = _saucer.x;
            _saucer_score_y = _saucer.y;
            spawn_particles(static_cast<int16_t>(_saucer.x + SAUCER_W / 2),
                            static_cast<int16_t>(_saucer.y + SAUCER_H / 2));
            host.audio().play_sfx(sounds::SFX_EXPLOSION, sounds::SFX_EXPLOSION_COUNT);
        }
    }

    // 敌人子弹
    for (auto& eb : _ebullets) {
        if (!eb.active) continue;
        eb.y += ENEMY_BULLET_SPEED;
        if (eb.y >= SCREEN_HEIGHT) {
            eb.active = false;
            continue;
        }

        // 击中护盾（命中则停用该子弹）
        if (damage_shield(eb.x, eb.y)) {
            eb.active = false;
            continue;
        }

        // 击中玩家
        if (_state == State::PLAYING && eb.x >= _player_x && eb.x < _player_x + PLAYER_W && eb.y >= PLAYER_Y &&
            eb.y < PLAYER_Y + PLAYER_H) {
            eb.active = false;
            spawn_particles(static_cast<int16_t>(_player_x + PLAYER_W / 2),
                            static_cast<int16_t>(PLAYER_Y + PLAYER_H / 2));
            host.audio().play_sfx(sounds::SFX_PLAYER_HIT, sounds::SFX_PLAYER_HIT_COUNT);
            --_lives;
            if (_lives <= 0) {
                _state = State::GAME_OVER;
            } else {
                _state = State::DYING;
                _dying_timer = DYING_TIMER;
            }
        }
    }
}

// ── 护盾 ──────────────────────────────────────────────────────────

void InvadersScreen::init_shields() {
    for (auto & _shield : _shields)
        for (int16_t sy = 0; sy < SHIELD_H; ++sy)
            for (int16_t sx = 0; sx < SHIELD_W; ++sx) _shield[sy][sx] = true;
}

bool InvadersScreen::damage_shield(int16_t bx, int16_t by) {
    int16_t total_w = SHIELD_COUNT * SHIELD_W + (SHIELD_COUNT - 1) * SHIELD_GAP;
    int16_t start_x = (SCREEN_WIDTH - total_w) / 2;
    for (int16_t s = 0; s < SHIELD_COUNT; ++s) {
        int16_t sx = static_cast<int16_t>(start_x + s * (SHIELD_W + SHIELD_GAP));
        if (bx >= sx && bx < sx + SHIELD_W && by >= SHIELD_Y && by < SHIELD_Y + SHIELD_H) {
            int16_t lx = bx - sx;
            int16_t ly = by - SHIELD_Y;
            if (lx >= 0 && lx < SHIELD_W && ly >= 0 && ly < SHIELD_H && _shields[s][ly][lx]) {
                _shields[s][ly][lx] = false;
                return true;
            }
            return false;
        }
    }
    return false;
}

// ── 神秘飞船 ──────────────────────────────────────────────────────

void InvadersScreen::update_saucer(IScreenHost& host) {
    if (_saucer_score_display > 0) {
        --_saucer_score_display;
    }

    if (_saucer.alive) {
        _saucer.x += _saucer.dir * SAUCER_SPEED;
        if (_saucer.x + SAUCER_W < PLAY_AREA_LEFT || _saucer.x > SCREEN_WIDTH) {
            _saucer.alive = false;
        }
        return;
    }

    if (_saucer_timer > 0) {
        --_saucer_timer;
        if (_saucer_timer == 0) {
            host.audio().play_sfx(sounds::SFX_SAUCER_APPEAR, sounds::SFX_SAUCER_APPEAR_COUNT);
            _saucer.alive = true;
            _saucer.dir = (next_rng() % 2 == 0) ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
            _saucer.y = SAUCER_Y;
            _saucer.x = (_saucer.dir > 0) ? PLAY_AREA_LEFT : SCREEN_WIDTH;
            _saucer.points = SAUCER_SCORES[next_rng() % 5];
            uint32_t saucer_min = SAUCER_MIN_INTERVAL - static_cast<uint32_t>(_level * 8);
            uint32_t saucer_max = SAUCER_MAX_INTERVAL - static_cast<uint32_t>(_level * 20);
            saucer_min = std::max<uint32_t>(saucer_min, 90);
            saucer_max = std::max(saucer_max, saucer_min + 60);
            _saucer_timer = saucer_min + (next_rng() % (saucer_max - saucer_min));
        }
    }
}

// ── 粒子 ──────────────────────────────────────────────────────────

void InvadersScreen::spawn_particles(int16_t x, int16_t y) {
    for (int16_t i = 0; i < MAX_PARTICLES_PER_EXPLOSION && _particle_count < MAX_PARTICLES; ++i) {
        int16_t idx = _particle_count++;
        _particles[idx].x = static_cast<int8_t>(x);
        _particles[idx].y = static_cast<int8_t>(y);
        _particles[idx].vx = static_cast<int8_t>((next_rng() % 5) - 2);
        _particles[idx].vy = static_cast<int8_t>((next_rng() % 5) - 2);
        _particles[idx].life = static_cast<uint8_t>(6 + (next_rng() % 6));
    }
}

void InvadersScreen::update_particles() {
    for (int16_t i = 0; i < _particle_count;) {
        _particles[i].x += _particles[i].vx;
        _particles[i].y += _particles[i].vy;
        if (_particles[i].life == 0) {
            _particles[i] = _particles[_particle_count - 1];
            --_particle_count;
        } else {
            --_particles[i].life;
            ++i;
        }
    }
}

// ── 渲染 ──────────────────────────────────────────────────────────

void InvadersScreen::draw_enemy(IDisplay& display, int16_t px, int16_t py, int16_t row, uint32_t frame) const {
    Color color = enemy_row_color(row);
    int anim = (frame / 20) % 2;
    const char* shape = ENEMY_SHAPES[row][anim];
    for (int16_t dy = 0; dy < ENEMY_H; ++dy)
        for (int16_t dx = 0; dx < ENEMY_W; ++dx)
            if (shape[dy * 6 + dx] == 'X')
                display.draw_pixel(static_cast<int16_t>(px + dx), static_cast<int16_t>(py + dy), color);
}

void InvadersScreen::draw_player(IDisplay& display, int16_t x, uint32_t frame) const {
    int16_t cx = static_cast<int16_t>(x + PLAYER_W / 2);
    display.draw_pixel(cx, PLAYER_Y, INV_PLAYER);
    for (int16_t dx = -1; dx <= 1; ++dx) display.draw_pixel(static_cast<int16_t>(cx + dx), PLAYER_Y + 1, INV_PLAYER);
    for (int16_t dx = -2; dx <= 2; ++dx) display.draw_pixel(static_cast<int16_t>(cx + dx), PLAYER_Y + 2, INV_PLAYER);
    for (int16_t dx = -3; dx <= 3; ++dx) display.draw_pixel(static_cast<int16_t>(cx + dx), PLAYER_Y + 3, INV_PLAYER);
    for (int16_t dx = -3; dx <= 3; ++dx) {
        if (dx == 0) continue;
        display.draw_pixel(static_cast<int16_t>(cx + dx), PLAYER_Y + 4, INV_PLAYER);
    }
    if ((frame / 6) % 2 == 0) {
        display.draw_pixel(static_cast<int16_t>(cx - 2), PLAYER_Y + 5, INV_PLAYER_ENGINE);
        display.draw_pixel(static_cast<int16_t>(cx + 2), PLAYER_Y + 5, INV_PLAYER_ENGINE);
    } else {
        display.draw_pixel(static_cast<int16_t>(cx - 1), PLAYER_Y + 5, INV_PLAYER_ENGINE);
        display.draw_pixel(static_cast<int16_t>(cx + 1), PLAYER_Y + 5, INV_PLAYER_ENGINE);
    }
}

void InvadersScreen::draw_shields(IDisplay& display) const {
    int16_t total_w = SHIELD_COUNT * SHIELD_W + (SHIELD_COUNT - 1) * SHIELD_GAP;
    int16_t start_x = (SCREEN_WIDTH - total_w) / 2;
    for (int16_t s = 0; s < SHIELD_COUNT; ++s) {
        int16_t sx = static_cast<int16_t>(start_x + s * (SHIELD_W + SHIELD_GAP));
        for (int16_t sy = 0; sy < SHIELD_H; ++sy)
            for (int16_t lx = 0; lx < SHIELD_W; ++lx)
                if (_shields[s][sy][lx])
                    display.draw_pixel(static_cast<int16_t>(sx + lx), static_cast<int16_t>(SHIELD_Y + sy),
                                       SHIELD_COLOR);
    }
}

void InvadersScreen::draw_saucer(IDisplay& display) const {
    if (!_saucer.alive) return;
    int16_t sx = _saucer.x, sy = _saucer.y;
    for (int16_t dx = 3; dx <= 10; ++dx) display.draw_pixel(static_cast<int16_t>(sx + dx), sy, SAUCER_COLOR);
    for (int16_t dx = 2; dx <= 11; ++dx) display.draw_pixel(static_cast<int16_t>(sx + dx), sy + 1, SAUCER_COLOR);
    for (int16_t dx = 1; dx <= 12; ++dx) display.draw_pixel(static_cast<int16_t>(sx + dx), sy + 2, SAUCER_COLOR);
    display.draw_pixel(static_cast<int16_t>(sx + 2), sy + 3, SAUCER_COLOR);
    display.draw_pixel(static_cast<int16_t>(sx + 11), sy + 3, SAUCER_COLOR);
    for (int16_t dx = 4; dx <= 9; ++dx) display.draw_pixel(static_cast<int16_t>(sx + dx), sy + 3, SAUCER_COLOR);
    display.draw_pixel(static_cast<int16_t>(sx + 4), sy + 4, SAUCER_COLOR);
    display.draw_pixel(static_cast<int16_t>(sx + 9), sy + 4, SAUCER_COLOR);
    if ((_frame / 15) % 2 == 0) {
        display.draw_pixel(static_cast<int16_t>(sx + 6), sy + 1, Color::WHITE);
        display.draw_pixel(static_cast<int16_t>(sx + 7), sy + 1, Color::WHITE);
    }
}

// ── 主循环 ────────────────────────────────────────────────────────

void InvadersScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    const auto& input = platform.input();

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            _paused = false;
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    switch (_state) {
        case State::PLAYING: {
            if (input.is_down(ButtonBits::LEFT)) {
                _player_x -= PLAYER_SPEED;
                _player_x = std::max(_player_x, PLAY_AREA_LEFT);
            }
            if (input.is_down(ButtonBits::RIGHT)) {
                _player_x += PLAYER_SPEED;
                _player_x = std::min(_player_x, PLAY_AREA_RIGHT);
            }
            if (input.was_pressed(ButtonBits::A)) player_shoot(host);
            if (input.was_pressed(ButtonBits::START)) {
                _paused = true;
                return;
            }
            move_enemies(host);
            if (_state != State::PLAYING) break;
            move_bullets(host);
            update_particles();
            update_saucer(host);
            ++_shoot_timer;
            int16_t shoot_base =
                static_cast<int16_t>(SHOOT_INTERVAL_BASE - (_level - 1) * SHOOT_INTERVAL_STEP_PER_LEVEL);
            shoot_base = std::max<uint32_t>(shoot_base, MIN_SHOOT_BASE);
            uint32_t shoot_threshold =
                static_cast<uint32_t>(shoot_base + _enemies_alive_count * SHOOT_INTERVAL_PER_ENEMY);
            if (_shoot_timer >= shoot_threshold) {
                _shoot_timer = 0;
                enemy_shoot(host);
            }
            if (_enemies_alive_count == 0) {
                _state = State::LEVEL_CLEAR;
                host.audio().play_sfx(sounds::SFX_LEVEL_CLEAR_INVADERS, sounds::SFX_LEVEL_CLEAR_INVADERS_COUNT);
                _level_clear_timer = LEVEL_CLEAR_DURATION;
            }
            break;
        }
        case State::DYING: {
            update_particles();
            move_bullets(host);
            update_saucer(host);
            if (_dying_timer > 0) --_dying_timer;
            else respawn_player();
            break;
        }
        case State::LEVEL_CLEAR: {
            update_particles();
            if (_level_clear_timer > 0) --_level_clear_timer;
            else start_next_level();
            break;
        }
        case State::GAME_OVER: {
            update_particles();
            if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
                _level = 1;
                reset_game();
            }
            if (input.was_pressed(ButtonBits::B)) {
                host.switch_to(ScreenType::MENU);
                return;
            }
            break;
        }
    }
}

void InvadersScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(INV_BG);

    // 状态栏
    display.fill_rect({0, 0, SCREEN_WIDTH, STATUS_H}, INV_STATUS_BG);
    TextRenderer::draw_text(display, {2, 1}, "SC:", INV_TEXT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_int(display, 14, 1, _score, INV_TEXT, COMPACT_FONT_3X5);
    TextRenderer::draw_text(display, {46, 1}, "LV", INV_TEXT, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_int(display, 54, 1, _level, INV_TEXT, COMPACT_FONT_3X5);

    for (int8_t i = 0; i < _lives; ++i) {
        int16_t lx = static_cast<int16_t>(60 + i * 7);
        display.draw_pixel(static_cast<int16_t>(lx + 1), 1, INV_LIFE_COLOR);
        for (int16_t dx = 0; dx < 3; ++dx) display.draw_pixel(static_cast<int16_t>(lx + dx), 2, INV_LIFE_COLOR);
        for (int16_t dx = -1; dx <= 1; ++dx) display.draw_pixel(static_cast<int16_t>(lx + 1 + dx), 3, INV_LIFE_COLOR);
    }

    // 地面
    for (int16_t gx = 0; gx < SCREEN_WIDTH; gx += 4) {
        display.draw_pixel(gx, PLAYER_Y - 1, INV_GROUND);
        display.draw_pixel(static_cast<int16_t>(gx + 1), PLAYER_Y - 1, INV_GROUND);
    }

    // 护盾
    draw_shields(display);

    // 敌人编队
    for (int16_t row = 0; row < GRID_ROWS; ++row)
        for (int16_t col = 0; col < GRID_COLS; ++col)
            if (_enemy_alive[row * GRID_COLS + col])
                draw_enemy(display, static_cast<int16_t>(_enemy_base_x + col * CELL_W),
                           static_cast<int16_t>(_enemy_base_y + row * CELL_H), row, _frame);

    // 神秘飞船
    draw_saucer(display);
    if (_saucer_score_display > 0) {
        TextRenderer::draw_int(display, _saucer_score_x, _saucer_score_y, _saucer.points, SAUCER_COLOR, COMPACT_FONT_3X5);
    }

    // 子弹
    if (_bullet_active)
        for (int16_t by = 0; by < PLAYER_BULLET_TRAIL; ++by)
            display.draw_pixel(_bullet_x, static_cast<int16_t>(_bullet_y - by), INV_BULLET);
    for (const auto& eb : _ebullets)
        if (eb.active)
            for (int16_t by = 0; by < ENEMY_BULLET_TRAIL; ++by)
                display.draw_pixel(eb.x, static_cast<int16_t>(eb.y - by), INV_EBULLET);

    // 粒子
    for (int16_t i = 0; i < _particle_count; ++i)
        display.draw_pixel(_particles[i].x, _particles[i].y,
                           (_particles[i].life % 2 == 0) ? INV_EXPLOSION : INV_WARN_COLOR);

    // 玩家
    if (_state == State::PLAYING || _state == State::LEVEL_CLEAR ||
        (_state == State::DYING && (_dying_timer / 4) % 2 == 0))
        draw_player(display, _player_x, _frame);

    // 关卡清除覆盖层
    if (_state == State::LEVEL_CLEAR) {
        display.fill_rect({10, 20, 60, 28}, INV_BG);
        display.draw_rect({10, 20, 60, 28}, INV_BULLET);
        TextRenderer::draw_text(display, {16, 30}, "LEVEL", INV_BULLET, 1, BASIC_FONT_5X7);
        TextRenderer::draw_int(display, 46, 30, _level, INV_BULLET, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 42}, "CLEAR!", INV_TEXT, 1, COMPACT_FONT_3X5);
    }

    // 游戏结束覆盖层
    if (_state == State::GAME_OVER) {
        int16_t ox = 10, oy = END_RECT_OY;
        int16_t oh = (_enemies_alive_count == 0) ? END_RECT_WIN_H : END_RECT_LOSE_H;
        display.fill_rect({ox, oy, 60, oh}, INV_BG);
        display.draw_rect({ox, oy, 60, oh}, INV_TEXT);
        const char* msg = (_enemies_alive_count == 0) ? "YOU WIN!" : "GAME OVER";
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 10)}, msg,
                                         (_enemies_alive_count == 0) ? INV_BULLET : INV_WARN_COLOR, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text(display, {22, static_cast<int16_t>(oy + 18)}, "SC:", INV_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_int(display, 34, static_cast<int16_t>(oy + 18), _score, INV_TEXT, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + oh - 14)}, "A/START: Again",
                                         INV_HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + oh - 6)}, "B: Menu", INV_HINT_COLOR, 1,
                                         COMPACT_FONT_3X5);
    }

    // 暂停覆盖层
    if (_paused) {
        display.fill_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, PAUSE_BG);
        display.draw_rect({PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, INV_PLAYER);
        TextRenderer::draw_pause_overlay(display, 40, 28, PAUSE_TEXT, INV_HINT_COLOR);
    }
}

void InvadersScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t  /*frame*/) {
    const auto cx = static_cast<int16_t>(box.x + box.width / 2 - ENEMY_W / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2 - ENEMY_H / 2);
    const char* shape = ENEMY_SHAPES[0][0];
    Color color = enemy_row_color(0);
    for (int16_t dy = 0; dy < ENEMY_H; ++dy)
        for (int16_t dx = 0; dx < ENEMY_W; ++dx)
            if (shape[dy * 6 + dx] == 'X')
                display.draw_pixel(static_cast<int16_t>(cx + dx), static_cast<int16_t>(cy + dy), color);
}
} // namespace handheld
