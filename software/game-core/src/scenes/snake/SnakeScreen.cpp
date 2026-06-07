#include "scenes/snake/SnakeScreen.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/NumberRenderer.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "core/math/LookupTable.h"
#include "core/math/Prng.h"

namespace handheld {

namespace {

namespace C = snake::cfg;

Color body_color(int16_t i, int16_t len) {
    if (len <= 1) return C::HEAD_COLOR;
    auto r = static_cast<uint8_t>(C::HEAD_R + ((C::TAIL_R - C::HEAD_R) * i) / (len - 1));
    auto g = static_cast<uint8_t>(C::HEAD_G + ((C::TAIL_G - C::HEAD_G) * i) / (len - 1));
    auto b = static_cast<uint8_t>(C::HEAD_B + ((C::TAIL_B - C::HEAD_B) * i) / (len - 1));
    return rgb565(r, g, b);
}

} // namespace

// ── 方向 ────────────────────────────────────────

bool SnakeScreen::opposite_dir(Direction a, Direction b) {
    return (a == Direction::UP && b == Direction::DOWN) || (a == Direction::DOWN && b == Direction::UP) ||
           (a == Direction::LEFT && b == Direction::RIGHT) || (a == Direction::RIGHT && b == Direction::LEFT);
}

SnakeScreen::Direction SnakeScreen::dequeue_dir() {
    if (_q_head == _q_tail) return Direction::NONE;
    Direction d = _dir_q[_q_tail];
    _q_tail = (_q_tail + 1U) % Q_SIZE;
    return d;
}

void SnakeScreen::enqueue_dir(Direction d) {
    uint8_t n = (_q_head + 1U) % Q_SIZE;
    if (n == _q_tail) return;
    _dir_q[_q_head] = d;
    _q_head = n;
}

// ── 生命周期 ────────────────────────────────────

void SnakeScreen::enter(IPlatform& platform, IScreenHost& host) {
    reset_game();
    platform.display().clear(C::BG_COLOR);
}

void SnakeScreen::reset_game() {
    _dir = Direction::RIGHT;
    _q_head = 0;
    _q_tail = 0;
    _game_over = false;
    _won = false;
    _paused = false;
    _len = C::INITIAL_BODY_LENGTH;
    // 头在右，身体向左延伸（初始方向 RIGHT，头不会撞到自己）
    int8_t hx = static_cast<int8_t>(C::GRID_W / 2);
    int8_t hy = static_cast<int8_t>(C::GRID_H / 2);
    for (int16_t i = 0; i < _len; ++i) _body[i] = {static_cast<int8_t>(hx - i), hy};
    _score = 0;
    _speed = 0;
    _move_ctr = 0;
    _interval = C::INITIAL_INTERVAL;
    _rng = 12345;
    spawn_food();
}

uint32_t SnakeScreen::next_rng() {
    return xorshift32(_rng);
}

bool SnakeScreen::occupied(int8_t x, int8_t y) const {
    for (int16_t i = 0; i < _len; ++i)
        if (_body[i].x == x && _body[i].y == y) return true;
    return false;
}

void SnakeScreen::spawn_food() {
    int16_t n = 0;
    do {
        _food_x = static_cast<int8_t>(next_rng() % C::GRID_W);
        _food_y = static_cast<int8_t>(next_rng() % C::GRID_H);
    } while (occupied(_food_x, _food_y) && ++n < C::MAX_LENGTH);
}

// ── 移动 ────────────────────────────────────────

void SnakeScreen::move_snake(IScreenHost& host) {
    Direction next = dequeue_dir();
    if (next == Direction::NONE || opposite_dir(next, _dir)) next = _dir;
    _dir = next;

    int8_t nx = _body[0].x, ny = _body[0].y;
    switch (_dir) {
        case Direction::UP: --ny; break;
        case Direction::DOWN: ++ny; break;
        case Direction::LEFT: --nx; break;
        case Direction::RIGHT: ++nx; break;
        default: break;
    }

    if (nx < 0 || nx >= C::GRID_W || ny < 0 || ny >= C::GRID_H) {
        _game_over = true;
        host.audio().play_sfx(sounds::SFX_DEATH_SNAKE, sounds::SFX_DEATH_SNAKE_COUNT);
        return;
    }

    bool eating = (nx == _food_x && ny == _food_y);
    int16_t check = eating ? _len : _len - 1;
    for (int16_t i = 0; i < check; ++i) {
        if (_body[i].x == nx && _body[i].y == ny) {
            _game_over = true;
            host.audio().play_sfx(sounds::SFX_DEATH_SNAKE, sounds::SFX_DEATH_SNAKE_COUNT);
            return;
        }
    }

    for (int16_t i = _len - 1; i > 0; --i) _body[i] = _body[i - 1];
    _body[0] = {nx, ny};

    if (eating) {
        ++_len;
        _body[_len - 1] = _body[_len - 2];
        ++_score;
        uint8_t old_speed = _speed;
        _speed = static_cast<uint8_t>(_score / C::SPEED_DIVISOR);
        if (_speed > old_speed) host.audio().play_sfx(sounds::SFX_SPEED_UP, sounds::SFX_SPEED_UP_COUNT);
		else {
			host.audio().play_sfx(sounds::SFX_EAT, sounds::SFX_EAT_COUNT);
		}

        uint32_t r = static_cast<uint32_t>(_speed);
        _interval = (r >= C::INITIAL_INTERVAL - C::MIN_INTERVAL) ? C::MIN_INTERVAL : C::INITIAL_INTERVAL - r;
        if (_len >= C::MAX_LENGTH) {
            host.audio().play_sfx(sounds::SFX_WIN, sounds::SFX_WIN_COUNT);
            _won = true;
            _game_over = true;
            return;
        }
        spawn_food();
    }
}

// ── 主循环 ──────────────────────────────────────

void SnakeScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    auto& input = platform.input();

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
    if (!_game_over && input.was_pressed(ButtonBits::START)) {
        _paused = true;
        return;
    }
    if (_game_over) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            reset_game();
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    if (input.was_pressed(ButtonBits::UP)) enqueue_dir(Direction::UP);
    if (input.was_pressed(ButtonBits::DOWN)) enqueue_dir(Direction::DOWN);
    if (input.was_pressed(ButtonBits::LEFT)) enqueue_dir(Direction::LEFT);
    if (input.was_pressed(ButtonBits::RIGHT)) enqueue_dir(Direction::RIGHT);

    if (++_move_ctr >= _interval) {
        _move_ctr = 0;
        move_snake(host);
    }
}

// ── 渲染 ────────────────────────────────────────

void SnakeScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& d = platform.display();
    d.clear(C::BG_COLOR);

    const auto G = C::STATUS_H;
    const auto C2 = static_cast<int16_t>(C::CELL - 1); // 内部填充尺寸

    // 状态栏
    d.fill_rect(Rect{0, 0, 80, C::STATUS_H}, C::BAR_COLOR);
    char buf[16];
    buf[0] = 'S';
    buf[1] = 'C';
    buf[2] = ':';
    itoa_dec(static_cast<uint16_t>(_score), buf + 3);
    TextRenderer::draw_text(d, {2, 1}, buf, C::SCORE_COLOR, 1, COMPACT_FONT_3X5);
    buf[0] = 'S';
    buf[1] = 'P';
    buf[2] = 'D';
    buf[3] = ':';
    itoa_dec(_speed, buf + 4);
    TextRenderer::draw_text(d, {42, 1}, buf, C::SCORE_COLOR, 1, COMPACT_FONT_3X5);

    // 网格
    for (int16_t i = 0; i <= C::GRID_W; ++i)
        d.draw_v_line(static_cast<int16_t>(i * C::CELL), G, C::AREA_H, C::GRID_LINE);
    for (int16_t i = 0; i <= C::GRID_H; ++i)
        d.draw_h_line(0, static_cast<int16_t>(G + i * C::CELL), C::AREA_W, C::GRID_LINE);

    // 边界
    d.draw_rect(Rect{0, G, static_cast<int16_t>(C::AREA_W + 1), static_cast<int16_t>(C::AREA_H + 1)}, C::BORDER_COLOR);

    // 食物（脉冲动效）
    int16_t pulse = (_frame / 8) & 1;
    d.fill_rect(Rect{static_cast<int16_t>(_food_x * C::CELL + 1 + pulse),
                     static_cast<int16_t>(G + _food_y * C::CELL + 1 + pulse), static_cast<int16_t>(C2 - pulse * 2),
                     static_cast<int16_t>(C2 - pulse * 2)},
                C::FOOD_COLOR);

    // 蛇身（尾→头）
    for (int16_t i = _len - 1; i >= 1; --i)
        d.fill_rect(Rect{static_cast<int16_t>(_body[i].x * C::CELL + 1),
                         static_cast<int16_t>(G + _body[i].y * C::CELL + 1), C2, C2},
                    body_color(i, _len));

    // 头 + 眼睛
    auto hx = static_cast<int16_t>(_body[0].x * C::CELL + 1);
    auto hy = static_cast<int16_t>(G + _body[0].y * C::CELL + 1);
    d.fill_rect(Rect{hx, hy, C2, C2}, C::HEAD_COLOR);
    if (_dir == Direction::RIGHT) {
        d.draw_pixel(hx + 3, hy + 1, C::HEAD_EYE);
        d.draw_pixel(hx + 3, hy + 3, C::HEAD_EYE);
    } else if (_dir == Direction::LEFT) {
        d.draw_pixel(hx + 1, hy + 1, C::HEAD_EYE);
        d.draw_pixel(hx + 1, hy + 3, C::HEAD_EYE);
    } else if (_dir == Direction::UP) {
        d.draw_pixel(hx + 1, hy + 1, C::HEAD_EYE);
        d.draw_pixel(hx + 3, hy + 1, C::HEAD_EYE);
    } else if (_dir == Direction::DOWN) {
        d.draw_pixel(hx + 1, hy + 3, C::HEAD_EYE);
        d.draw_pixel(hx + 3, hy + 3, C::HEAD_EYE);
    }

    // 结束 / 暂停
    if (_game_over) {
        d.fill_rect(Rect{C::END_RECT_X, C::END_RECT_Y, C::END_RECT_W, C::END_RECT_H}, C::OVERLAY_BG);
        d.draw_rect(Rect{C::END_RECT_X, C::END_RECT_Y, C::END_RECT_W, C::END_RECT_H},
                    _won ? C::WIN_COLOR : C::GAMEOVER_COLOR);
        auto tc = _won ? C::WIN_COLOR : C::GAMEOVER_COLOR;
        TextRenderer::draw_text_centered(d, {40, 28}, _won ? "YOU WIN!" : "GAME OVER", tc, 1, BASIC_FONT_5X7);
        buf[0] = 'S';
        buf[1] = 'C';
        buf[2] = 'O';
        buf[3] = 'R';
        buf[4] = 'E';
        buf[5] = ':';
        buf[6] = ' ';
        itoa_dec(static_cast<uint16_t>(_score), buf + 7);
        TextRenderer::draw_text_centered(d, {40, 42}, buf, C::SCORE_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 50}, "A/START: Again", C::HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 58}, "B: Menu", C::HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
    if (_paused) {
        d.fill_rect(Rect{C::PAUSE_RECT_X, C::PAUSE_RECT_Y, C::PAUSE_RECT_W, C::PAUSE_RECT_H}, C::PAUSE_BG);
        d.draw_rect(Rect{C::PAUSE_RECT_X, C::PAUSE_RECT_Y, C::PAUSE_RECT_W, C::PAUSE_RECT_H}, C::HEAD_COLOR);
        TextRenderer::draw_pause_overlay(d, 40, 28, C::PAUSE_TEXT, C::HINT_COLOR);
    }
}

void SnakeScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    const auto cx = static_cast<int16_t>(box.x + box.width / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    const Color body_color = rgb565(60, 180, 80);
    const Color head_color = rgb565(100, 220, 120);

    // 蛇身绕圈旋转，6 个身体段 + 1 个头
    double angle = static_cast<double>(frame) * 0.18;
    for (int seg = 0; seg < 7; ++seg) {
        double a = angle - seg * 0.35;
        auto sx = static_cast<int16_t>(cx + cos_lut_double(phase_from_radians(a)) * 2.5);
        auto sy = static_cast<int16_t>(cy + sin_lut_double(phase_from_radians(a)) * 2.5);
        display.draw_pixel(sx, sy, (seg == 0) ? head_color : body_color);
    }
    // 眼睛：紧贴头部前方
    double ea = angle + 0.25;
    auto ex = static_cast<int16_t>(cx + cos_lut_double(phase_from_radians(ea)) * 2.5);
    auto ey = static_cast<int16_t>(cy + sin_lut_double(phase_from_radians(ea)) * 2.5);
    display.draw_pixel(ex, ey, rgb565(200, 235, 220));
}
} // namespace handheld
