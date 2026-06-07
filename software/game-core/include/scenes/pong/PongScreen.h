#ifndef SCENES_PONG_PONG_SCREEN_H
#define SCENES_PONG_PONG_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::pong::cfg {

// 画布（与硬件一致）
constexpr int16_t CANVAS_W = 80;
constexpr int16_t CANVAS_H = 80;

// 16:9 中间区域（play area）
constexpr int16_t PLAY_Y_TOP = 18;      // 顶 HUD 占 0..17
constexpr int16_t PLAY_Y_BOTTOM = 62;   // 底 HUD 占 63..79
constexpr int16_t PLAY_H = PLAY_Y_BOTTOM - PLAY_Y_TOP + 1; // 45
constexpr int16_t CENTER_LINE_X = CANVAS_W / 2; // 40

// 挡板：2 宽 × 14 高，限定在 play area 内移动
constexpr int16_t PADDLE_W = 2;
constexpr int16_t PADDLE_H = 14;
constexpr int16_t PADDLE_X_LEFT = 4;
constexpr int16_t PADDLE_X_RIGHT = 74;  // 74 + 2 = 76
constexpr int16_t PADDLE_Y_MIN = PLAY_Y_TOP;
constexpr int16_t PADDLE_Y_MAX = PLAY_Y_BOTTOM - PADDLE_H + 1; // 49
constexpr int16_t PADDLE_SPEED = 2;

// 球：1×1 像素
constexpr int16_t BALL_SIZE = 1;
constexpr int16_t BALL_VX_MAX = 4;
constexpr int16_t P2_STEP = 1;          // 2P 模式下 P2 每次按 A/B 步进的像素数
constexpr int16_t AI_BASE_SPEED = 1;    // 1P 模式下 AI 基础速度（像素/帧）
constexpr int16_t AI_JITTER = 1;        // AI 抖动幅度（像素）
constexpr int16_t SERVE_FRAMES = 30;    // 得分后到自动发球的等待帧数

// 模式
constexpr int16_t WIN_SCORE = 5;

// 模式选择阶段布局
constexpr int16_t MODE_SELECT_TITLE_Y = 18;
constexpr int16_t MODE_SELECT_BOX_W = 30;
constexpr int16_t MODE_SELECT_BOX_H = 12;
constexpr int16_t MODE_SELECT_BOX_Y = 38;            // 框顶 y
constexpr int16_t MODE_SELECT_LEFT_CENTER_X = 23;   // 1P 框中心 x（8..38）
constexpr int16_t MODE_SELECT_RIGHT_CENTER_X = 57;  // 2P 框中心 x（42..72，留 4px 间距）
constexpr int16_t MODE_SELECT_HINT_Y = 64;

// 颜色
constexpr Color PADDLE_COLOR = Color::WHITE;
constexpr Color BALL_COLOR = Color::WHITE;
constexpr Color COURT_COLOR = rgb565(6, 10, 16);
constexpr Color COURT_LINE = rgb565(40, 50, 70);   // play area 上下边界
constexpr Color CENTER_DASH = rgb565(40, 50, 60);
constexpr Color SCORE_COLOR = rgb565(200, 215, 230);
constexpr Color MODE_COLOR = rgb565(120, 200, 255);
constexpr Color WIN_COLOR = rgb565(120, 220, 255);
constexpr Color LOSE_COLOR = rgb565(230, 100, 120);
constexpr Color HINT_COLOR = rgb565(140, 160, 180);
constexpr Color HUD_COLOR = rgb565(120, 160, 200);
constexpr Color OVERLAY_BG = rgb565(0, 0, 0);

// 覆盖层尺寸
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 20;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 40;

// 顶 HUD 分数
constexpr int16_t SCORE_BASELINE_Y = 4; // 5×7 字体基线 y
constexpr int16_t SCORE_LEFT_CENTER_X = 15;
constexpr int16_t SCORE_RIGHT_CENTER_X = 65;
constexpr int16_t MODE_INDICATOR_X = 72; // 3×5 字体起点 x（"2P" 8px 宽，结束于 x=80）
constexpr int16_t MODE_INDICATOR_Y = 4;

// 底 HUD（rally / speed 合并为单行 "RALLY 03  SPD 2"）
constexpr int16_t HUD_BASELINE_Y = 72;
constexpr int16_t HUD_CENTER_X = 40;

} // namespace handheld::pong::cfg

namespace handheld {

class PongScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    enum class Mode : uint8_t { ONE_PLAYER, TWO_PLAYER };
    enum class Phase : uint8_t { MODE_SELECT, SERVE, PLAY, GAME_OVER };
    enum class ServeSide : uint8_t { LEFT, RIGHT };

    Mode _mode = Mode::ONE_PLAYER;
    Phase _phase = Phase::MODE_SELECT;
    ServeSide _serve_side = ServeSide::LEFT;
    bool _paused = false;

    // 挡板 y 坐标（play area 中央）
    int16_t _left_paddle_y = (pong::cfg::PLAY_Y_TOP + pong::cfg::PLAY_Y_BOTTOM - pong::cfg::PADDLE_H) / 2;
    int16_t _right_paddle_y = (pong::cfg::PLAY_Y_TOP + pong::cfg::PLAY_Y_BOTTOM - pong::cfg::PADDLE_H) / 2;

    // 球
    int16_t _ball_x = 0;
    int16_t _ball_y = 0;
    int16_t _ball_vx = 1;
    int16_t _ball_vy = -1;

    // 分数
    int16_t _left_score = 0;
    int16_t _right_score = 0;

    // 当前 rally 内的 paddle 击中次数（每次得分重置）
    int16_t _rally_count = 0;
    // 当前球速绝对值（用于 HUD 显示）
    int16_t _ball_speed = 1;

    // 服务倒计时
    int16_t _serve_timer = pong::cfg::SERVE_FRAMES;

    // RNG
    uint32_t _rng = 12345;
    uint32_t _frame = 0;

    void reset_game();
    void start_serve();
    void move_ball(IScreenHost& host);
    void on_paddle_hit();  // paddle 击中处理：rally 计数 + 加速
    void update_ai();
    void update_p2_input(IInput& input);
    void update_p1_input(IInput& input);
    [[nodiscard]] uint32_t next_rng();
};

} // namespace handheld

#endif
