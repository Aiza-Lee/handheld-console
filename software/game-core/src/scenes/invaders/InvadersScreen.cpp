#include "scenes/invaders/InvadersScreen.h"
#include "scenes/invaders/InvadersConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/audio/AudioMixer.h"
#include "core/runtime/ScreenType.h"
#include <cstdio>

extern "C" [[gnu::weak]] const handheld::Tone _sound_BGM_INVADERS[];
extern "C" [[gnu::weak]] const uint32_t _sound_BGM_INVADERS_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_SHOOT[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_SHOOT_count;
extern "C" [[gnu::weak]] const handheld::Tone _sound_SFX_EXPLOSION[];
extern "C" [[gnu::weak]] const uint32_t _sound_SFX_EXPLOSION_count;

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

int16_t enemy_row_score(int16_t row) {
    return ROW_SCORES[row];
}

}  // namespace

void InvadersScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(Color::BLACK);
    _high_score = 0;
    reset_game();
    if (_sound_BGM_INVADERS) host.mixer().set_bgm(_sound_BGM_INVADERS, _sound_BGM_INVADERS_count);
}

void InvadersScreen::reset_game() {
    for (int16_t i = 0; i < GRID_COLS * GRID_ROWS; ++i) _enemy_alive[i] = true;
    _enemy_base_x = GRID_OX; _enemy_base_y = GRID_OY; _enemy_dir_x = 1;
    _enemies_alive_count = GRID_COLS * GRID_ROWS;
    _player_x = PLAYER_START_X;
    _state = State::PLAYING; _paused = false;
    _bullet_active = false;
    for (auto& eb : _ebullets) eb.active = false;
    _particle_count = 0;
    _score = 0; _lives = INIT_LIVES;
    _frame = 0; _move_timer = 0; _move_interval = INIT_MOVE_INTERVAL; _shoot_timer = 0; _dying_timer = 0; _rng = 12345;
}

void InvadersScreen::respawn_player() {
    _player_x = PLAYER_START_X; _bullet_active = false;
    for (auto& eb : _ebullets) eb.active = false;
    _dying_timer = 0; _state = State::PLAYING;
}

uint32_t InvadersScreen::next_rng() { _rng = _rng * 1103515245 + 12345; return _rng; }

void InvadersScreen::player_shoot(IScreenHost& host) {
    if (!_bullet_active) {
        _bullet_x = static_cast<int16_t>(_player_x + PLAYER_W / 2);
        _bullet_y = PLAYER_Y - 1;
        _bullet_active = true;
        if (_sound_SFX_SHOOT) host.mixer().play_sfx(_sound_SFX_SHOOT, _sound_SFX_SHOOT_count);
    }
}

void InvadersScreen::enemy_shoot() {
    int16_t slot = -1;
    for (int16_t i = 0; i < MAX_EBULLETS; ++i) { if (!_ebullets[i].active) { slot = i; break; } }
    if (slot < 0) return;
    int16_t bottom_row = -1;
    for (int16_t row = GRID_ROWS - 1; row >= 0; --row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) { if (_enemy_alive[row * GRID_COLS + col]) { bottom_row = row; break; } }
        if (bottom_row >= 0) break;
    }
    if (bottom_row < 0) return;
    int16_t alive_cols[GRID_COLS]; int16_t alive_count = 0;
    for (int16_t col = 0; col < GRID_COLS; ++col) {
        if (_enemy_alive[bottom_row * GRID_COLS + col]) alive_cols[alive_count++] = col;
    }
    if (alive_count == 0) return;
    int16_t col = alive_cols[next_rng() % alive_count];
    _ebullets[slot].x = static_cast<int16_t>(_enemy_base_x + col * CELL_W + ENEMY_W / 2);
    _ebullets[slot].y = static_cast<int16_t>(_enemy_base_y + bottom_row * CELL_H + ENEMY_H);
    _ebullets[slot].active = true;
}

void InvadersScreen::move_enemies() {
    ++_move_timer; if (_move_timer < _move_interval) return;
    _move_timer = 0;
    bool hit_edge = false;
    for (int16_t row = 0; row < GRID_ROWS; ++row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (!_enemy_alive[row * GRID_COLS + col]) continue;
            int16_t ex = _enemy_base_x + col * CELL_W;
            if (ex + ENEMY_W >= 78 || ex <= 1) { hit_edge = true; break; }
        }
        if (hit_edge) break;
    }
    if (hit_edge) { _enemy_dir_x = static_cast<int8_t>(-_enemy_dir_x); _enemy_base_y += ENEMY_DROP; }
    else _enemy_base_x += _enemy_dir_x;

    for (int16_t row = 0; row < GRID_ROWS; ++row) {
        for (int16_t col = 0; col < GRID_COLS; ++col) {
            if (!_enemy_alive[row * GRID_COLS + col]) continue;
            if (_enemy_base_y + row * CELL_H + ENEMY_H >= PLAYER_Y) {
                _state = State::GAME_OVER; if (_score > _high_score) _high_score = _score;
                return;
            }
        }
    }
}

void InvadersScreen::move_bullets(IScreenHost& host) {
    if (_bullet_active) { _bullet_y -= 2; if (_bullet_y < STATUS_H) _bullet_active = false; }
    if (_bullet_active) {
        for (int16_t row = 0; row < GRID_ROWS; ++row) {
            for (int16_t col = 0; col < GRID_COLS; ++col) {
                if (!_enemy_alive[row * GRID_COLS + col]) continue;
                int16_t ex = _enemy_base_x + col * CELL_W;
                int16_t ey = _enemy_base_y + row * CELL_H;
                if (_bullet_x >= ex && _bullet_x < ex + ENEMY_W && _bullet_y >= ey && _bullet_y < ey + ENEMY_H) {
                    _enemy_alive[row * GRID_COLS + col] = false; --_enemies_alive_count;
                    _score += enemy_row_score(row); if (_score > _high_score) _high_score = _score;
                    _bullet_active = false;
                    spawn_particles(static_cast<int16_t>(ex + ENEMY_W / 2), static_cast<int16_t>(ey + ENEMY_H / 2));
                    if (_sound_SFX_EXPLOSION) host.mixer().play_sfx(_sound_SFX_EXPLOSION, _sound_SFX_EXPLOSION_count);
                    _move_interval = INIT_MOVE_INTERVAL - static_cast<uint32_t>((GRID_COLS * GRID_ROWS - _enemies_alive_count) / 4);
                    if (_move_interval < MIN_MOVE_INTERVAL) _move_interval = MIN_MOVE_INTERVAL;
                    break;
                }
            }
            if (!_bullet_active) break;
        }
    }
    for (auto& eb : _ebullets) {
        if (!eb.active) continue;
        eb.y += 1; if (eb.y >= 80) { eb.active = false; continue; }
        if (_state == State::PLAYING && eb.x >= _player_x && eb.x < _player_x + PLAYER_W &&
            eb.y >= PLAYER_Y && eb.y < PLAYER_Y + PLAYER_H) {
            eb.active = false;
            spawn_particles(static_cast<int16_t>(_player_x + PLAYER_W / 2), static_cast<int16_t>(PLAYER_Y + PLAYER_H / 2));
            if (_sound_SFX_EXPLOSION) host.mixer().play_sfx(_sound_SFX_EXPLOSION, _sound_SFX_EXPLOSION_count);
            --_lives;
            if (_lives <= 0) { _state = State::GAME_OVER; if (_score > _high_score) _high_score = _score; }
            else { _state = State::DYING; _dying_timer = DYING_TIMER; }
        }
    }
}

void InvadersScreen::spawn_particles(int16_t x, int16_t y) {
    for (int16_t i = 0; i < 6 && _particle_count < MAX_PARTICLES; ++i) {
        int16_t idx = _particle_count++;
        _particles[idx].x = static_cast<int8_t>(x); _particles[idx].y = static_cast<int8_t>(y);
        _particles[idx].vx = static_cast<int8_t>((next_rng() % 5) - 2);
        _particles[idx].vy = static_cast<int8_t>((next_rng() % 5) - 2);
        _particles[idx].life = static_cast<uint8_t>(6 + (next_rng() % 6));
    }
}

void InvadersScreen::update_particles() {
    for (int16_t i = 0; i < _particle_count; ) {
        _particles[i].x += _particles[i].vx; _particles[i].y += _particles[i].vy;
        if (_particles[i].life == 0) { _particles[i] = _particles[_particle_count - 1]; --_particle_count; }
        else { --_particles[i].life; ++i; }
    }
}

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
    for (int16_t dx = -3; dx <= 3; ++dx) { if (dx == 0) continue; display.draw_pixel(static_cast<int16_t>(cx + dx), PLAYER_Y + 4, INV_PLAYER); }
    if ((frame / 6) % 2 == 0) {
        display.draw_pixel(static_cast<int16_t>(cx - 2), PLAYER_Y + 5, INV_PLAYER_ENGINE);
        display.draw_pixel(static_cast<int16_t>(cx + 2), PLAYER_Y + 5, INV_PLAYER_ENGINE);
    } else {
        display.draw_pixel(static_cast<int16_t>(cx - 1), PLAYER_Y + 5, INV_PLAYER_ENGINE);
        display.draw_pixel(static_cast<int16_t>(cx + 1), PLAYER_Y + 5, INV_PLAYER_ENGINE);
    }
}

void InvadersScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    const auto& input = platform.input();

    if (_paused) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
        return;
    }

    switch (_state) {
        case State::PLAYING: {
            if (input.is_down(ButtonBits::LEFT))  { _player_x -= PLAYER_SPEED; if (_player_x < 0) _player_x = 0; }
            if (input.is_down(ButtonBits::RIGHT)) { _player_x += PLAYER_SPEED; if (_player_x + PLAYER_W > 80) _player_x = 80 - PLAYER_W; }
            if (input.was_pressed(ButtonBits::A)) player_shoot(host);
            if (input.was_pressed(ButtonBits::START)) { _paused = true; return; }
            move_enemies(); if (_state != State::PLAYING) break;
            move_bullets(host); update_particles();
            ++_shoot_timer;
            if (_shoot_timer >= static_cast<uint32_t>(30 + _enemies_alive_count * 2)) { _shoot_timer = 0; enemy_shoot(); }
            if (_enemies_alive_count == 0) { _state = State::GAME_OVER; if (_score > _high_score) _high_score = _score; }
            break;
        }
        case State::DYING: { update_particles(); move_bullets(host); if (_dying_timer > 0) --_dying_timer; else respawn_player(); break; }
        case State::GAME_OVER: {
            update_particles();
            if (input.was_pressed(ButtonBits::START)) reset_game();
            if (input.was_pressed(ButtonBits::B)) { host.switch_to(ScreenType::MENU); return; }
            break;
        }
    }
}

void InvadersScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(INV_BG);
    display.fill_rect({0, 0, 80, STATUS_H}, INV_STATUS_BG);

    char buf[12];
    snprintf(buf, sizeof(buf), "%d", _score);
    TextRenderer::draw_text(display, {2, 1}, buf, INV_TEXT, 1, COMPACT_FONT_3X5);
    snprintf(buf, sizeof(buf), "HI:%d", _high_score);
    TextRenderer::draw_text(display, {28, 1}, buf, INV_TEXT, 1, COMPACT_FONT_3X5);

    for (int8_t i = 0; i < _lives; ++i) {
        int16_t lx = static_cast<int16_t>(60 + i * 7);
        display.draw_pixel(static_cast<int16_t>(lx + 1), 1, INV_LIFE_COLOR);
        for (int16_t dx = 0; dx < 3; ++dx) display.draw_pixel(static_cast<int16_t>(lx + dx), 2, INV_LIFE_COLOR);
        for (int16_t dx = -1; dx <= 1; ++dx) display.draw_pixel(static_cast<int16_t>(lx + 1 + dx), 3, INV_LIFE_COLOR);
    }

    for (int16_t gx = 0; gx < 80; gx += 4) { display.draw_pixel(gx, 71, INV_GROUND); display.draw_pixel(static_cast<int16_t>(gx + 1), 71, INV_GROUND); }

    for (int16_t row = 0; row < GRID_ROWS; ++row)
        for (int16_t col = 0; col < GRID_COLS; ++col)
            if (_enemy_alive[row * GRID_COLS + col])
                draw_enemy(display, static_cast<int16_t>(_enemy_base_x + col * CELL_W), static_cast<int16_t>(_enemy_base_y + row * CELL_H), row, _frame);

    if (_bullet_active) for (int16_t by = 0; by < 4; ++by) display.draw_pixel(_bullet_x, static_cast<int16_t>(_bullet_y - by), INV_BULLET);
    for (const auto& eb : _ebullets) if (eb.active) for (int16_t by = 0; by < 3; ++by) display.draw_pixel(eb.x, static_cast<int16_t>(eb.y - by), INV_EBULLET);
    for (int16_t i = 0; i < _particle_count; ++i) display.draw_pixel(_particles[i].x, _particles[i].y, (_particles[i].life % 2 == 0) ? INV_EXPLOSION : INV_WARN_COLOR);

    if (_state == State::PLAYING || (_state == State::DYING && (_dying_timer / 4) % 2 == 0))
        draw_player(display, _player_x, _frame);

    if (_state == State::GAME_OVER) {
        int16_t ox = 10, oy = 28, oh = (_enemies_alive_count == 0) ? 28 : 32;
        display.fill_rect({ox, oy, 60, oh}, INV_BG); display.draw_rect({ox, oy, 60, oh}, INV_TEXT);
        const char* msg = (_enemies_alive_count == 0) ? "YOU WIN!" : "GAME OVER";
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 10)}, msg,
                                         (_enemies_alive_count == 0) ? INV_BULLET : INV_WARN_COLOR, 1, BASIC_FONT_5X7);
        snprintf(buf, sizeof(buf), "SC:%d", _score);
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + 20)}, buf, INV_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(oy + oh - 6)}, "START=AGAIN", INV_HINT_COLOR, 1, COMPACT_FONT_3X5);
    }

    // 暂停覆盖层 (太空主题)
    if (_paused) {
        display.fill_rect({10, 22, 60, 36}, PAUSE_BG);
        display.draw_rect({10, 22, 60, 36}, INV_PLAYER);
        TextRenderer::draw_text_centered(display, {40, 30}, "PAUSED", PAUSE_TEXT, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(display, {40, 44}, "A: Resume", PAUSE_TEXT, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(display, {40, 54}, "B: Menu", INV_HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

}  // namespace handheld
