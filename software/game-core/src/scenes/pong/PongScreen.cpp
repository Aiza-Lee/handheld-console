#include "scenes/pong/PongScreen.h"

#include <algorithm>

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/NumberRenderer.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "core/math/Prng.h"

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

// 模式选择框：center_x 框中心 x，y 框顶 y，label 居中文字，selected 是否高亮
void draw_mode_box(IDisplay& d, int16_t center_x, int16_t y, const char* label, bool selected) {
    const int16_t x = static_cast<int16_t>(center_x - MODE_SELECT_BOX_W / 2);
    Color border = selected ? MODE_COLOR : HINT_COLOR;
    Color fill = selected ? OVERLAY_BG : COURT_COLOR;
    d.fill_rect(Rect{x, y, MODE_SELECT_BOX_W, MODE_SELECT_BOX_H}, fill);
    d.draw_rect(Rect{x, y, MODE_SELECT_BOX_W, MODE_SELECT_BOX_H}, border);
    TextRenderer::draw_text_centered(d, {center_x, static_cast<int16_t>(y + 2)}, label, border, 1,
                                     COMPACT_FONT_3X5);
}

} // namespace

// ── 生命周期 ────────────────────────────────────────

void PongScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(COURT_COLOR);
    reset_game();
    // 每次从菜单进入时先进入模式选择；reset_game() 默认 SERVE，
    // 由 enter() 覆盖为 MODE_SELECT；游戏内 START 重玩则保留 SERVE。
    _phase = Phase::MODE_SELECT;
}

void PongScreen::reset_game() {
    _mode = Mode::ONE_PLAYER;
    _phase = Phase::SERVE;
    _paused = false;
    _serve_side = ServeSide::LEFT;
    const int16_t cy = static_cast<int16_t>((PLAY_Y_TOP + PLAY_Y_BOTTOM - PADDLE_H) / 2);
    _left_paddle_y = cy;
    _right_paddle_y = cy;
    _left_score = 0;
    _right_score = 0;
    _rally_count = 0;
    _ball_speed = 1;
    _ball_vx = 1;
    _ball_vy = -1;
    // 初始球位置对齐 P1 paddle（P1 是首个发球方）
    _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
    _ball_y = static_cast<int16_t>(_left_paddle_y + PADDLE_H / 2);
    _serve_timer = SERVE_FRAMES;
    _rng = 12345;
    _frame = 0;
}

// 准备发球：球从 _serve_side 一侧的 paddle 处出球
void PongScreen::start_serve() {
    if (_serve_side == ServeSide::LEFT) {
        _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
        _ball_y = static_cast<int16_t>(_left_paddle_y + PADDLE_H / 2);
        _ball_vx = 1;
    } else {
        _ball_x = static_cast<int16_t>(PADDLE_X_RIGHT - 1);
        _ball_y = static_cast<int16_t>(_right_paddle_y + PADDLE_H / 2);
        _ball_vx = -1;
    }
    _ball_vy = (next_rng() & 1U) ? static_cast<int16_t>(-1) : static_cast<int16_t>(1);
    _phase = Phase::PLAY;
    _serve_timer = 0;
    _rally_count = 0;
    _ball_speed = 1;
}

// ── 主循环 ──────────────────────────────────────────

void PongScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    IInput& input = platform.input();

    // 暂停：START/A 继续，B 返回菜单
    if (_paused) {
        if (input.was_pressed(ButtonBits::START) || input.was_pressed(ButtonBits::A)) {
            _paused = false;
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    // 模式选择：LEFT/RIGHT 切换 1P/2P，A/START 确认进入 SERVE，B 返回菜单
    if (_phase == Phase::MODE_SELECT) {
        if (input.was_pressed(ButtonBits::LEFT) || input.was_pressed(ButtonBits::RIGHT)) {
            _mode = (_mode == Mode::ONE_PLAYER) ? Mode::TWO_PLAYER : Mode::ONE_PLAYER;
            host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
        }
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            _phase = Phase::SERVE;
            _serve_timer = SERVE_FRAMES;
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
            return;
        }
        return;
    }

    // 游戏中（PLAY / SERVE）：B 先暂停，暂停中再按 B 才退出
    // if (_phase == Phase::PLAY || _phase == Phase::SERVE) {
    //     if (input.was_pressed(ButtonBits::B)) {
    //         _paused = true;
    //         return;
    //     }
    // }

    // 模式实时切换：SELECT 在游戏中切换 1P ↔ 2P
    // if (input.was_pressed(ButtonBits::SELECT)) {
    //     _mode = (_mode == Mode::ONE_PLAYER) ? Mode::TWO_PLAYER : Mode::ONE_PLAYER;
    //     host.audio().play_sfx(sounds::SFX_MODE_SWITCH, sounds::SFX_MODE_SWITCH_COUNT);
    // }

    // START 暂停（仅 PLAY / SERVE 阶段）
    if (_phase != Phase::GAME_OVER && input.was_pressed(ButtonBits::START)) {
        _paused = true;
        return;
    }

    if (_phase == Phase::GAME_OVER) {
        if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
            reset_game();
            return;
        }
        if (input.was_pressed(ButtonBits::B)) {
            host.switch_to(ScreenType::MENU);
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

    // 发球：球贴在发球方 paddle 处（跟随 paddle 移动），倒计时归零后自动发
    if (_phase == Phase::SERVE) {
        if (_serve_side == ServeSide::LEFT) {
            _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
            _ball_y = static_cast<int16_t>(_left_paddle_y + PADDLE_H / 2);
        } else {
            _ball_x = static_cast<int16_t>(PADDLE_X_RIGHT - 1);
            _ball_y = static_cast<int16_t>(_right_paddle_y + PADDLE_H / 2);
        }
        if (_serve_timer > 0) {
            --_serve_timer;
            if (_serve_timer == 0) {
                // 自动发球：方向由 _serve_side 决定（失分方发球）
                start_serve();
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
    _left_paddle_y = std::max(_left_paddle_y, PADDLE_Y_MIN);
    _left_paddle_y = std::min(_left_paddle_y, PADDLE_Y_MAX);
}

void PongScreen::update_p2_input(IInput& input) {
    // 2P：右侧 A=up, B=down 步进
    if (input.is_down(ButtonBits::A)) {
        _right_paddle_y -= PADDLE_SPEED;
    }
    if (input.is_down(ButtonBits::B)) {
		_right_paddle_y += PADDLE_SPEED;
    }
    _right_paddle_y = std::max(_right_paddle_y, PADDLE_Y_MIN);
    _right_paddle_y = std::min(_right_paddle_y, PADDLE_Y_MAX);
}

void PongScreen::update_ai() {
    int16_t ball_center_y = static_cast<int16_t>(_ball_y + BALL_SIZE / 2);
    int16_t step = ai_step(ball_center_y, _right_paddle_y, _rng);
    _right_paddle_y = static_cast<int16_t>(_right_paddle_y + step);
    _right_paddle_y = std::max(_right_paddle_y, PADDLE_Y_MIN);
    _right_paddle_y = std::min(_right_paddle_y, PADDLE_Y_MAX);
}

// ── 球物理 ──────────────────────────────────────────

void PongScreen::on_paddle_hit() {
    ++_rally_count;
    int16_t new_speed = static_cast<int16_t>(1 + _rally_count / 2);
    if (new_speed > BALL_VX_MAX) new_speed = BALL_VX_MAX;
    _ball_speed = new_speed;
    _ball_vx = (_ball_vx < 0) ? static_cast<int16_t>(-new_speed) : static_cast<int16_t>(new_speed);
}

void PongScreen::move_ball(IScreenHost& host) {
    const int16_t prev_x = _ball_x;
    _ball_x = static_cast<int16_t>(_ball_x + _ball_vx);
    _ball_y = static_cast<int16_t>(_ball_y + _ball_vy);

    // 顶/底墙反射（限定在 play area 内）
    if (_ball_y < PLAY_Y_TOP) {
        _ball_y = PLAY_Y_TOP;
        _ball_vy = static_cast<int16_t>(-_ball_vy);
        host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
    } else if (_ball_y > PLAY_Y_BOTTOM) {
        _ball_y = PLAY_Y_BOTTOM;
        _ball_vy = static_cast<int16_t>(-_ball_vy);
        host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
    }

    // 左挡板碰撞（swept）：球向左移动 + 扫过 paddle 右边界
    if (_ball_vx < 0 && prev_x >= PADDLE_X_LEFT + PADDLE_W && _ball_x <= PADDLE_X_LEFT + PADDLE_W) {
        if (_ball_y + BALL_SIZE > _left_paddle_y && _ball_y < _left_paddle_y + PADDLE_H) {
            _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
            _ball_vx = static_cast<int16_t>(-_ball_vx);
            int16_t diff = static_cast<int16_t>((_ball_y + BALL_SIZE / 2) - (_left_paddle_y + PADDLE_H / 2));
            int16_t new_vy = diff / 3;
            new_vy = std::min<int>(new_vy, 2);
            new_vy = std::max<int>(new_vy, -2);
            if (new_vy == 0) new_vy = (_ball_vy != 0) ? _ball_vy : 1;
            _ball_vy = new_vy;
            on_paddle_hit();
            host.audio().play_sfx(sounds::SFX_PONG_HIT, sounds::SFX_PONG_HIT_COUNT);
        }
    }

    // 右挡板碰撞（swept）：球向右移动 + 扫过 paddle 左边界
    if (_ball_vx > 0 && prev_x <= PADDLE_X_RIGHT && _ball_x > PADDLE_X_RIGHT) {
        if (_ball_y + BALL_SIZE > _right_paddle_y && _ball_y < _right_paddle_y + PADDLE_H) {
            _ball_x = static_cast<int16_t>(PADDLE_X_RIGHT - 1);
            _ball_vx = static_cast<int16_t>(-_ball_vx);
            int16_t diff = static_cast<int16_t>((_ball_y + BALL_SIZE / 2) - (_right_paddle_y + PADDLE_H / 2));
            int16_t new_vy = diff / 3;
            new_vy = std::min<int>(new_vy, 2);
            new_vy = std::max<int>(new_vy, -2);
            if (new_vy == 0) new_vy = (_ball_vy != 0) ? _ball_vy : 1;
            _ball_vy = new_vy;
            on_paddle_hit();
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
        // 失分方发球：P1 失分 → P1 发球（球向 P1 paddle 出来，朝 +x 方向）
        _serve_side = ServeSide::LEFT;
        _phase = Phase::SERVE;
        _serve_timer = SERVE_FRAMES;
        _rally_count = 0;
        _ball_speed = 1;
        _ball_x = static_cast<int16_t>(PADDLE_X_LEFT + PADDLE_W);
        _ball_y = static_cast<int16_t>(_left_paddle_y + PADDLE_H / 2);
        _ball_vx = 0;
        _ball_vy = 0;
    } else if (_ball_x > CANVAS_W - BALL_SIZE) {
        ++_left_score;
        host.audio().play_sfx(sounds::SFX_PONG_SCORE, sounds::SFX_PONG_SCORE_COUNT);
        if (_left_score >= WIN_SCORE) {
            _phase = Phase::GAME_OVER;
            host.audio().play_sfx(sounds::SFX_PONG_WIN, sounds::SFX_PONG_WIN_COUNT);
            return;
        }
        // 失分方发球：P2 失分 → P2 发球（球向 P2 paddle 出来，朝 -x 方向）
        _serve_side = ServeSide::RIGHT;
        _phase = Phase::SERVE;
        _serve_timer = SERVE_FRAMES;
        _rally_count = 0;
        _ball_speed = 1;
        _ball_x = static_cast<int16_t>(PADDLE_X_RIGHT - 1);
        _ball_y = static_cast<int16_t>(_right_paddle_y + PADDLE_H / 2);
        _ball_vx = 0;
        _ball_vy = 0;
    }
}

uint32_t PongScreen::next_rng() {
    return xorshift32(_rng);
}

// ── 渲染 ────────────────────────────────────────────

void PongScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& d = platform.display();
    d.clear(COURT_COLOR);

    // 模式选择页：标题 + 两个选项框 + 操作提示
    if (_phase == Phase::MODE_SELECT) {
        TextRenderer::draw_text_centered(d, {40, MODE_SELECT_TITLE_Y}, "SELECT MODE", MODE_COLOR, 1,
                                         BASIC_FONT_5X7);
        draw_mode_box(d, MODE_SELECT_LEFT_CENTER_X, MODE_SELECT_BOX_Y, "1P",
                      _mode == Mode::ONE_PLAYER);
        draw_mode_box(d, MODE_SELECT_RIGHT_CENTER_X, MODE_SELECT_BOX_Y, "2P",
                      _mode == Mode::TWO_PLAYER);
        TextRenderer::draw_text_centered(d, {40, MODE_SELECT_HINT_Y}, "L/R: SWITCH", HINT_COLOR, 1,
                                         COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, static_cast<int16_t>(MODE_SELECT_HINT_Y + 8)},
                                         "A/START  B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
        return;
    }

    // play area 上下边界线
    d.draw_h_line(0, PLAY_Y_TOP, CANVAS_W, COURT_LINE);
    d.draw_h_line(0, PLAY_Y_BOTTOM, CANVAS_W, COURT_LINE);

    // 中央虚线分隔（仅在 play area 内）
    for (int16_t y = PLAY_Y_TOP + 2; y < PLAY_Y_BOTTOM; y += 4) {
        d.draw_pixel(CENTER_LINE_X, y, CENTER_DASH);
    }

    // 左挡板
    d.fill_rect(Rect{PADDLE_X_LEFT, _left_paddle_y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);
    // 右挡板
    d.fill_rect(Rect{PADDLE_X_RIGHT, _right_paddle_y, PADDLE_W, PADDLE_H}, PADDLE_COLOR);

    // 球（1 像素）
    d.draw_pixel(_ball_x, _ball_y, BALL_COLOR);

    // 顶 HUD：分数（左右）+ 模式指示（右上）
    char lbuf[4];
    char rbuf[4];
    itoa_dec(static_cast<uint16_t>(_left_score), lbuf);
    itoa_dec(static_cast<uint16_t>(_right_score), rbuf);
    TextRenderer::draw_text_centered(d, {SCORE_LEFT_CENTER_X, SCORE_BASELINE_Y}, lbuf, SCORE_COLOR, 1,
                                     BASIC_FONT_5X7);
    TextRenderer::draw_text_centered(d, {SCORE_RIGHT_CENTER_X, SCORE_BASELINE_Y}, rbuf, SCORE_COLOR, 1,
                                     BASIC_FONT_5X7);
    const char* mode_label = (_mode == Mode::ONE_PLAYER) ? "1P" : "2P";
    TextRenderer::draw_text(d, {MODE_INDICATOR_X, MODE_INDICATOR_Y}, mode_label, MODE_COLOR, 1, COMPACT_FONT_3X5);

    // 底 HUD：合并为单行 "RALLY 03  SPD 2" 居中
    char rally_buf[8];
    char speed_buf[8];
    itoa_dec(static_cast<uint16_t>(_rally_count), rally_buf);
    itoa_dec(static_cast<uint16_t>(_ball_speed), speed_buf);

    // 拼接 "RALLY " + rally + "  SPD " + speed
    char hud_line[24];
    int k = 0;
    const char* prefix_rally = "RALLY ";
    for (int i = 0; prefix_rally[i] != '\0' && k < static_cast<int>(sizeof(hud_line)) - 1; ++i) {
        hud_line[k++] = prefix_rally[i];
    }
    for (int i = 0; rally_buf[i] != '\0' && k < static_cast<int>(sizeof(hud_line)) - 1; ++i) {
        hud_line[k++] = rally_buf[i];
    }
    const char* mid = "  SPD ";
    for (int i = 0; mid[i] != '\0' && k < static_cast<int>(sizeof(hud_line)) - 1; ++i) {
        hud_line[k++] = mid[i];
    }
    for (int i = 0; speed_buf[i] != '\0' && k < static_cast<int>(sizeof(hud_line)) - 1; ++i) {
        hud_line[k++] = speed_buf[i];
    }
    hud_line[k] = '\0';

    TextRenderer::draw_text_centered(d, {HUD_CENTER_X, HUD_BASELINE_Y}, hud_line, HUD_COLOR, 1,
                                     COMPACT_FONT_3X5);

    // 覆盖层：暂停 / 结束
    if (_paused) {
        d.fill_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, OVERLAY_BG);
        d.draw_rect(Rect{PAUSE_RECT_X, PAUSE_RECT_Y, PAUSE_RECT_W, PAUSE_RECT_H}, MODE_COLOR);
        TextRenderer::draw_pause_overlay(d, 40, 28, MODE_COLOR, HINT_COLOR);
    }
    if (_phase == Phase::GAME_OVER) {
        d.fill_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, OVERLAY_BG);
        bool left_won = (_left_score >= WIN_SCORE);
        Color title_c = left_won ? WIN_COLOR : LOSE_COLOR;
        const char* title = left_won ? "P1 WINS!" : "P2 WINS!";
        d.draw_rect(Rect{END_RECT_X, END_RECT_Y, END_RECT_W, END_RECT_H}, title_c);
        TextRenderer::draw_text_centered(d, {40, 28}, title, title_c, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text_centered(d, {40, 42}, "A/START: Again", HINT_COLOR, 1, COMPACT_FONT_3X5);
        TextRenderer::draw_text_centered(d, {40, 52}, "B: Menu", HINT_COLOR, 1, COMPACT_FONT_3X5);
    }
}

void PongScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    // 10x10 预览：左右挡板 + 弹跳的小球
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    const auto left_x = static_cast<int16_t>(box.x + 1);
    const auto right_x = static_cast<int16_t>(box.x + box.width - 1);
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
    auto bx = static_cast<int16_t>(box.x + 2 + bx_off);
    auto by = static_cast<int16_t>(cy + ((frame / 4) % 3) - 1);
    display.draw_pixel(bx, by, Color::WHITE);
    display.draw_pixel(static_cast<int16_t>(bx + 1), by, Color::WHITE);
    display.draw_pixel(bx, static_cast<int16_t>(by + 1), Color::WHITE);
    display.draw_pixel(static_cast<int16_t>(bx + 1), static_cast<int16_t>(by + 1), Color::WHITE);
}

} // namespace handheld
