#include "scenes/pong/PongScreen.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/NumberRenderer.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

using namespace pong::cfg;

namespace {

// 1P 模式右侧 AI：根据球 y 方向推进挡板，慢于球速以保持可玩
// 输入：球中心 y，挡板当前 y 坐标
// 输出：建议的 y 移动量（已含抖动，每帧最多 ±AI_BASE_SPEED）
int16_t ai_step(int16_t ball_center_y, int16_t paddle_y, uint32_t& rng) {
    int16_t dy = static_cast<int16_t>(ball_center_y - (paddle_y + PADDLE_H / 2));
    int16_t step = 0;
    if (dy > 0) {
        step = AI_BASE_SPEED;
    } else if (dy < 0) {
        step = -AI_BASE_SPEED;
    }
    // 抖动：~25% 概率添加 ±1 像素噪声
    rng ^= rng << 13U;
    rng ^= rng >> 17U;
    rng ^= rng << 5U;
    if ((rng & 3U) == 0U) {
        step += (rng & 1U) ? 1 : -1;
    }
    return step;
}

} // namespace

// ── 生命周期 ────────────────────────────────────────

void PongScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(COURT_COLOR);
    reset_game();
    if (ENABLE_BGM) host.audio().set_bgm(sounds::BGM_PONG, sounds::BGM_PONG_COUNT);
}

void PongScreen::reset_game() {
    _mode = Mode::ONE_PLAYER;
    _phase = Phase::SERVE;
    _paused = false;
    _left_paddle_y = 30;
    _right_paddle_y = 30;
    _ball_x = BALL_START_X;
    _ball_y = BALL_START_Y;
    _ball_vx = 1;
    _ball_vy = -1;
    _left_score = 0;
    _right_score = 0;
    _serve_timer = SERVE_FRAMES;
    _rng = 12345;
    _frame = 0;
}

// 准备发球：球置于中心，速度向 dir_x 方向（+1 朝右，-1 朝左）
void PongScreen::start_serve(int16_t dir_x) {
    _ball_x = BALL_START_X;
    _ball_y = BALL_START_Y;
    _ball_vx = dir_x;
    // 随机选择初始 vy：-1 或 +1
    _ball_vy = (next_rng() & 1U) ? static_cast<int16_t>(-1) : static_cast<int16_t>(1);
    _phase = Phase::PLAY;
    _serve_timer = 0;
}

// ── 主循环 ──────────────────────────────────────────

void PongScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    IInput& input = platform.input();

    // 暂停：START 继续，B 返回菜单
    if (_paused) {
        if (input.was_pressed(ButtonBits::START) || input.was_pressed(ButtonBits::A)) _paused = false;
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    // 模式切换：任何阶段均可触发 SELECT
    if (input.was_pressed(ButtonBits::SELECT)) {
        _mode = (_mode == Mode::ONE_PLAYER) ? Mode::TWO_PLAYER : Mode::ONE_PLAYER;
        host.audio().play_sfx(sounds::SFX_MODE_SWITCH, sounds::SFX_MODE_SWITCH_COUNT);
    }

    // 暂停中按 START 暂停（仅 PLAY / SERVE 阶段）
    if (_phase != Phase::GAME_OVER && input.was_pressed(ButtonBits::START)) {
        _paused = true;
        return;
    }

    // B 返回菜单（任何阶段）
    if (input.was_pressed(ButtonBits::B)) {
        host.switch_to(ScreenType::MENU);
        return;
    }

    if (_phase == Phase::GAME_OVER) {
        if (input.was_pressed(ButtonBits::START)) {
            reset_game();
            return;
        }
        return;
    }

    // P1 输入（左挡板，UP/DOWN 持续按住连续移动）
    update_p1_input(input);

    // P2 输入（仅 2P 模式，A/B 单步）
    if (_mode == Mode::TWO_PLAYER) {
        update_p2_input(input);
    } else {
        // 1P：右侧 AI
        update_ai();
    }

    // 发球：手动（A 立即发）或自动（倒计时归零）
    if (_phase == Phase::SERVE) {
        if (input.was_pressed(ButtonBits::A)) {
            // 按 _ball_vx 当前方向发球（初始 1，后续由得分方决定）
            start_serve(_ball_vx);
            return;
        }
        if (_serve_timer > 0) {
            --_serve_timer;
            if (_serve_timer == 0) {
                // 自动发球：方向由当前 _ball_vx 决定
                start_serve(_ball_vx);
                return;
            }
        }
        return;
    }

    // 游戏中：移动球并处理碰撞
    move_ball(host);
}

// ── 输入 ────────────────────────────────────────────

void PongScreen::update_p1_input(IInput& input) {
    if (input.is_down(ButtonBits::UP)) _left_paddle_y -= PADDLE_SPEED;
    if (input.is_down(ButtonBits::DOWN)) _left_paddle_y += PADDLE_SPEED;
    if (_left_paddle_y < PADDLE_Y_MIN) _left_paddle_y = PADDLE_Y_MIN;
    if (_left_paddle_y > PADDLE_Y_MAX) _left_paddle_y = PADDLE_Y_MAX;
}

void PongScreen::update_p2_input(IInput& input) {
    // 2P：右侧 A=up, B=down 步进
    if (input.was_pressed(ButtonBits::A)) {
        _right_paddle_y -= P2_STEP;
    }
    if (input.was_pressed(ButtonBits::B)) {
        _right_paddle_y += P2_STEP;
    }
    if (_right_paddle_y < PADDLE_Y_MIN) _right_paddle_y = PADDLE_Y_MIN;
    if (_right_paddle_y > PADDLE_Y_MAX) _right_paddle_y = PADDLE_Y_MAX;
}

void PongScreen::update_ai() {
    int16_t ball_center_y = static_cast<int16_t>(_ball_y + BALL_SIZE / 2);
    int16_t step = ai_step(ball_center_y, _right_paddle_y, _rng);
    _right_paddle_y = static_cast<int16_t>(_right_paddle_y + step);
    if (_right_paddle_y < PADDLE_Y_MIN) _right_paddle_y = PADDLE_Y_MIN;
    if (_right_paddle_y > PADDLE_Y_MAX) _right_paddle_y = PADDLE_Y_MAX;
}

// ── 球物理 ──────────────────────────────────────────

void PongScreen::move_ball(IScreenHost& host) {
    _ball_x = static_cast<int16_t>(_ball_x + _ball_vx);
    _ball_y = static_cast<int16_t>(_ball_y + _ball_vy);

    // 顶/底墙反射
    if (_ball_y < 0) {
        _ball_y = 0;
        _ball_vy = static_cast<int16_t>(-_ball_vy);
        host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
    } else if (_ball_y > CANVAS_H - BALL_SIZE) {
        _ball_y = static_cast<int16_t>(CANVAS_H - BALL_SIZE);
        _ball_vy = static_cast<int16_t>(-_ball_vy);
        host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
    }

    // 左挡板碰撞（球向左移动，进入挡板 x 范围）
    if (_ball_vx < 0 && _ball_x + BALL_SIZE >= PADDLE_X_LEFT + PADDLE_W && _ball_x < PADDLE_X_LEFT + PADDLE_W) {
        // y 范围重叠
        if (_ball_y + BALL_SIZE > _left_paddle_y && _ball_y < _left_paddle_y + PADDLE_H) {
            _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
            _ball_vx = static_cast<int16_t>(-_ball_vx);
            int16_t diff = static_cast<int16_t>((_ball_y + BALL_SIZE / 2) - (_left_paddle_y + PADDLE_H / 2));
            int16_t new_vy = diff / 3;
            if (new_vy > 2) new_vy = 2;
            if (new_vy < -2) new_vy = -2;
            if (new_vy == 0) new_vy = (_ball_vy != 0) ? _ball_vy : 1;
            _ball_vy = new_vy;
            if (_ball_vx > BALL_VX_MAX) _ball_vx = BALL_VX_MAX;
            if (_ball_vx < -BALL_VX_MAX) _ball_vx = -BALL_VX_MAX;
            host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
        }
    }

    // 右挡板碰撞（球向右移动，进入挡板 x 范围）
    if (_ball_vx > 0 && _ball_x <= PADDLE_X_RIGHT && _ball_x + BALL_SIZE > PADDLE_X_RIGHT) {
        if (_ball_y + BALL_SIZE > _right_paddle_y && _ball_y < _right_paddle_y + PADDLE_H) {
            _ball_x = static_cast<int16_t>(PADDLE_X_RIGHT - BALL_SIZE);
            _ball_vx = static_cast<int16_t>(-_ball_vx);
            int16_t diff = static_cast<int16_t>((_ball_y + BALL_SIZE / 2) - (_right_paddle_y + PADDLE_H / 2));
            int16_t new_vy = diff / 3;
            if (new_vy > 2) new_vy = 2;
            if (new_vy < -2) new_vy = -2;
            if (new_vy == 0) new_vy = (_ball_vy != 0) ? _ball_vy : 1;
            _ball_vy = new_vy;
            if (_ball_vx > BALL_VX_MAX) _ball_vx = BALL_VX_MAX;
            if (_ball_vx < -BALL_VX_MAX) _ball_vx = -BALL_VX_MAX;
            host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
        }
    }

    // 计分：球越过任一侧出界
    if (_ball_x < 0) {
        ++_right_score;
        host.audio().play_sfx(sounds::SFX_PONG_SCORE, sounds::SFX_PONG_SCORE_COUNT);
        if (_right_score >= WIN_SCORE) {
            _phase = Phase::GAME_OVER;
            host.audio().play_sfx(sounds::SFX_PONG_WIN, sounds::SFX_PONG_WIN_COUNT);
            return;
        }
        // 自动发球（向右，朝向失败方）
        _ball_x = BALL_START_X;
        _ball_y = BALL_START_Y;
        _ball_vx = 1;
        _ball_vy = (next_rng() & 1U) ? static_cast<int16_t>(-1) : static_cast<int16_t>(1);
        _phase = Phase::SERVE;
        _serve_timer = SERVE_FRAMES;
    } else if (_ball_x > CANVAS_W - BALL_SIZE) {
        ++_left_score;
        host.audio().play_sfx(sounds::SFX_PONG_SCORE, sounds::SFX_PONG_SCORE_COUNT);
        if (_left_score >= WIN_SCORE) {
            _phase = Phase::GAME_OVER;
            host.audio().play_sfx(sounds::SFX_PONG_WIN, sounds::SFX_PONG_WIN_COUNT);
            return;
        }
        _ball_x = BALL_START_X;
        _ball_y = BALL_START_Y;
        _ball_vx = -1;
        _ball_vy = (next_rng() & 1U) ? static_cast<int16_t>(-1) : static_cast<int16_t>(1);
        _phase = Phase::SERVE;
        _serve_timer = SERVE_FRAMES;
    }
}

uint32_t PongScreen::next_rng() {
    _rng ^= _rng << 13U;
    _rng ^= _rng >> 17U;
    _rng ^= _rng << 5U;
    return _rng;
}

// ── 渲染 ────────────────────────────────────────────

void PongScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& d = platform.display();
    d.clear(COURT_COLOR);

    // 中央虚线分隔
    for (int16_t y = 0; y < CANVAS_H; y += 4) {
        d.draw_pixel(static_cast<int16_t>(CANVAS_W / 2), y, CENTER_DASH);
    }

    // 左挡板
    d.fill_rect(Rect{PADDLE_X_LEFT, _left_paddle_y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);
    // 右挡板
    d.fill_rect(Rect{PADDLE_X_RIGHT, _right_paddle_y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);

    // 球
    d.fill_rect(Rect{_ball_x, _ball_y, BALL_SIZE, BALL_SIZE}, BALL_COLOR);

    // 分数（顶部左右）
    char lbuf[4];
    char rbuf[4];
    itoa_dec(static_cast<uint16_t>(_left_score), lbuf, sizeof(lbuf));
    itoa_dec(static_cast<uint16_t>(_right_score), rbuf, sizeof(rbuf));
    TextRenderer::draw_text_centered(d, {SCORE_LEFT_CENTER_X, SCORE_BASELINE_Y}, lbuf, SCORE_COLOR, 1,
                                     BASIC_FONT_5X7);
    TextRenderer::draw_text_centered(d, {SCORE_RIGHT_CENTER_X, SCORE_BASELINE_Y}, rbuf, SCORE_COLOR, 1,
                                     BASIC_FONT_5X7);

    // 模式指示（顶部右侧）
    const char* mode_label = (_mode == Mode::ONE_PLAYER) ? "1P" : "2P";
    TextRenderer::draw_text(d, {MODE_INDICATOR_X, MODE_INDICATOR_Y}, mode_label, MODE_COLOR, 1, COMPACT_FONT_3X5);

    // 覆盖层：暂停 / 结束
    if (_paused) {
        d.fill_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, OVERLAY_BG);
        d.draw_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, MODE_COLOR);
        TextRenderer::draw_text_centered(d, {40, 28}, "PAUSED", MODE_COLOR, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(d, {40, 42}, "A: Resume", SCORE_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
    if (_phase == Phase::GAME_OVER) {
        d.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, OVERLAY_BG);
        bool left_won = (_left_score >= WIN_SCORE);
        Color title_c = left_won ? WIN_COLOR : LOSE_COLOR;
        const char* title = left_won ? "P1 WINS!" : "P2 WINS!";
        d.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, title_c);
        TextRenderer::draw_text_centered(d, {40, 28}, title, title_c, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(d, {40, 42}, "START: Again", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

void PongScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    // 10x10 预览：左右挡板 + 弹跳的小球
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    const auto left_x = static_cast<int16_t>(box.x + 1);
    const auto right_x = static_cast<int16_t>(box.x + box.width - 2);
    // 挡板（中央偏短）
    for (int dy = -3; dy <= 3; ++dy) {
        display.draw_pixel(left_x, static_cast<int16_t>(cy + dy), Color::WHITE);
        display.draw_pixel(static_cast<int16_t>(left_x + 1), static_cast<int16_t>(cy + dy), Color::WHITE);
        display.draw_pixel(right_x, static_cast<int16_t>(cy + dy), Color::WHITE);
        display.draw_pixel(static_cast<int16_t>(right_x - 1), static_cast<int16_t>(cy + dy), Color::WHITE);
    }
    // 小球：根据 frame 在中间往复
    int phase = static_cast<int>((frame / 3) % 12);
    int bx_off = (phase < 6) ? phase : (11 - phase); // 0..5..0
    auto bx = static_cast<int16_t>(box.x + 3 + bx_off);
    auto by = static_cast<int16_t>(cy + ((frame / 4) % 3) - 1);
    display.draw_pixel(bx, by, Color::WHITE);
    display.draw_pixel(static_cast<int16_t>(bx + 1), by, Color::WHITE);
    display.draw_pixel(bx, static_cast<int16_t>(by + 1), Color::WHITE);
    display.draw_pixel(static_cast<int16_t>(bx + 1), static_cast<int16_t>(by + 1), Color::WHITE);
}

} // namespace handheld
