#ifndef SCENES_PONG_PONG_SCREEN_H
#define SCENES_PONG_PONG_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::pong::cfg {

constexpr bool ENABLE_BGM = false;

// 画布（与硬件一致）
constexpr int16_t CANVAS_W = 80;
constexpr int16_t CANVAS_H = 80;

// 挡板：2 宽 × 20 高，居中 60px 行程
constexpr int16_t PADDLE_W = 2;
constexpr int16_t PADDLE_H = 20;
constexpr int16_t PADDLE_X_LEFT = 2;
constexpr int16_t PADDLE_X_RIGHT = 76; // 76 + 2 = 78，恰好不超出画布
constexpr int16_t PADDLE_Y_MIN = 10;
constexpr int16_t PADDLE_Y_MAX = 50; // 50 + 20 = 70
constexpr int16_t PADDLE_SPEED = 2;

// 球：2×2 像素方块
constexpr int16_t BALL_SIZE = 2;
constexpr int16_t BALL_START_X = 40;
constexpr int16_t BALL_START_Y = 40;
constexpr int16_t BALL_VX_MAX = 4;
constexpr int16_t P2_STEP = 1;          // 2P 模式下 P2 每次按 A/B 步进的像素数
constexpr int16_t AI_BASE_SPEED = 1;    // 1P 模式下 AI 基础速度（像素/帧）
constexpr int16_t AI_JITTER = 1;        // AI 抖动幅度（像素）
constexpr int16_t SERVE_FRAMES = 30;    // 得分后到自动发球的等待帧数

// 模式
constexpr int16_t WIN_SCORE = 5;

// 颜色
constexpr Color PADDLE_COLOR = Color::WHITE;
constexpr Color BALL_COLOR = Color::WHITE;
constexpr Color COURT_COLOR = rgb565(6, 10, 16);
constexpr Color CENTER_DASH = rgb565(40, 50, 60);
constexpr Color SCORE_COLOR = rgb565(200, 215, 230);
constexpr Color MODE_COLOR = rgb565(120, 200, 255);
constexpr Color WIN_COLOR = rgb565(120, 220, 255);
constexpr Color LOSE_COLOR = rgb565(230, 100, 120);
constexpr Color HINT_COLOR = rgb565(140, 160, 180);
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

// 分数显示
constexpr int16_t SCORE_BASELINE_Y = 4; // 5×7 字体基线 y
constexpr int16_t SCORE_LEFT_CENTER_X = 15;
constexpr int16_t SCORE_RIGHT_CENTER_X = 65;
constexpr int16_t MODE_INDICATOR_X = 75; // 3×5 字体起点 x（顶右）
constexpr int16_t MODE_INDICATOR_Y = 4;

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
    enum class Phase : uint8_t { SERVE, PLAY, GAME_OVER };

    Mode _mode = Mode::ONE_PLAYER;
    Phase _phase = Phase::SERVE;
    bool _paused = false;

    // 挡板 y 坐标
    int16_t _left_paddle_y = 30;
    int16_t _right_paddle_y = 30;

    // 球
    int16_t _ball_x = pong::cfg::BALL_START_X;
    int16_t _ball_y = pong::cfg::BALL_START_Y;
    int16_t _ball_vx = 1;
    int16_t _ball_vy = -1;

    // 分数
    int16_t _left_score = 0;
    int16_t _right_score = 0;

    // 服务倒计时
    int16_t _serve_timer = pong::cfg::SERVE_FRAMES;

    // RNG
    uint32_t _rng = 12345;
    uint32_t _frame = 0;

    void reset_game();
    void start_serve(int16_t dir_x);
    void move_ball(IScreenHost& host);
    void update_ai();
    void update_p2_input(IInput& input);
    void update_p1_input(IInput& input);
    [[nodiscard]] uint32_t next_rng();
};

} // namespace handheld

#endif
