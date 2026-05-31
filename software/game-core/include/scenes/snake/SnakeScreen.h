#ifndef SCENES_SNAKE_SNAKE_SCREEN_H
#define SCENES_SNAKE_SNAKE_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include "scenes/snake/SnakeConfig.h"
#include <cstdint>

namespace handheld {

class SnakeScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    enum class Direction : uint8_t { NONE = 0, UP, DOWN, LEFT, RIGHT };

    struct Segment { int8_t x, y; };

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
    uint32_t _rng = 12345;
    uint32_t _frame = 0;

    // 方向队列
    static constexpr uint8_t Q_SIZE = 3;
    Direction _dir_q[Q_SIZE]{};
    uint8_t _q_head = 0, _q_tail = 0;

    void reset_game();
    void spawn_food();
    [[nodiscard]] bool occupied(int8_t x, int8_t y) const;
    uint32_t next_rng();
    void move_snake(IScreenHost& host);
    Direction dequeue_dir();
    void enqueue_dir(Direction d);
    [[nodiscard]] static bool opposite_dir(Direction a, Direction b);
};

}  // namespace handheld

#endif
