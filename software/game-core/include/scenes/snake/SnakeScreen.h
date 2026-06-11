#ifndef SCENES_SNAKE_SNAKE_SCREEN_H
#define SCENES_SNAKE_SNAKE_SCREEN_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::snake::cfg {

// 地图
constexpr int16_t GRID_W = 13;
constexpr int16_t GRID_H = 12;
constexpr int16_t CELL = 6;
constexpr int16_t STATUS_H = 7;
constexpr int16_t AREA_W = GRID_W * CELL;
constexpr int16_t AREA_H = GRID_H * CELL;
constexpr int16_t MAX_LENGTH = GRID_W * GRID_H;

// 速度（值越小越快，单位：逻辑帧）
constexpr uint32_t INITIAL_INTERVAL = 8;
constexpr uint32_t MIN_INTERVAL = 3;
constexpr uint8_t SPEED_DIVISOR = 4;

// 初始长度
constexpr int16_t INITIAL_BODY_LENGTH = 3;

// 颜色
constexpr Color HEAD_COLOR = rgb565(60, 180, 80);
constexpr Color HEAD_EYE = rgb565(200, 235, 220);
constexpr Color FOOD_COLOR = rgb565(255, 55, 55);
constexpr Color BORDER_COLOR = rgb565(20, 40, 25);
constexpr Color GRID_LINE = rgb565(6, 12, 8);
constexpr Color BG_COLOR = rgb565(3, 8, 4);
constexpr Color BAR_COLOR = rgb565(4, 12, 6);
constexpr Color SCORE_COLOR = rgb565(140, 180, 150);
constexpr Color GAMEOVER_COLOR = rgb565(255, 145, 190);
constexpr Color OVERLAY_BG = rgb565(3, 8, 6);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(3, 10, 6);
constexpr Color PAUSE_TEXT = rgb565(100, 200, 140);

// 覆盖层尺寸
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 20;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 48;

// 渐变
constexpr uint8_t HEAD_R = 60, HEAD_G = 180, HEAD_B = 80;
constexpr uint8_t TAIL_R = 15, TAIL_G = 60, TAIL_B = 20;

} // namespace handheld::snake::cfg

namespace handheld {

class SnakeScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    enum class Direction : uint8_t { NONE = 0, UP, DOWN, LEFT, RIGHT };

    struct Segment {
        int8_t x, y;
    };

    Direction _dir = Direction::RIGHT;
    bool _game_over = false;
    bool _won = false;
    bool _paused = false;
    Segment _body[snake::cfg::MAX_LENGTH]{};
    int16_t _len = 0;
    int8_t _food_x = 0, _food_y = 0;
    int16_t _score = 0;
    uint8_t _speed = 0;
    uint32_t _move_ctr = 0;
    uint32_t _interval = snake::cfg::INITIAL_INTERVAL;
    uint32_t _frame = 0;

    // 方向队列
    static constexpr uint8_t Q_SIZE = 3;
    Direction _dir_q[Q_SIZE]{};
    uint8_t _q_head = 0, _q_tail = 0;

    void reset_game();
    void spawn_food();
    [[nodiscard]] bool occupied(int8_t x, int8_t y) const;
    void move_snake(IScreenHost& host);
    Direction dequeue_dir();
    void enqueue_dir(Direction d);
    [[nodiscard]] static bool opposite_dir(Direction a, Direction b);
};

} // namespace handheld

#endif
