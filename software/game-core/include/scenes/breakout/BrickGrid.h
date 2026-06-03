#ifndef SCENES_BREAKOUT_BRICK_GRID_H
#define SCENES_BREAKOUT_BRICK_GRID_H

#include "core/graphics/Geometry.h"
#include <cstdint>

namespace handheld::breakout {

// 砖块网格：管理碰撞检测和销毁，无 tile cache（实时计算）
class BrickGrid {
public:
    static constexpr int16_t COLS = 15;
    static constexpr int16_t ROWS = 11;
    static constexpr int16_t OX = 2;
    static constexpr int16_t OY = 8;
    static constexpr int16_t CELL = 5; // SIZE + GAP
    static constexpr int16_t SIZE = 4; // 实际砖块像素
    static constexpr int16_t W = COLS * CELL;
    static constexpr int16_t H = ROWS * CELL;
    static constexpr uint8_t INDESTRUCTIBLE = 9;

    void load(const uint8_t level[ROWS][COLS]);

    // 像素坐标 (x,y) 是否命中砖块；返回砖块类型（0=未命中）
    [[nodiscard]] uint8_t hit_test(int16_t x, int16_t y) const;

    // 摧毁格子 (row,col) 的砖块；返回得分（0 表示不可摧毁或已空）
    int16_t destroy(int16_t row, int16_t col);

    // 砖块屏幕坐标矩形
    [[nodiscard]] static Rect brick_rect(int16_t row, int16_t col);

    [[nodiscard]] int16_t remaining() const { return _remaining; }
    [[nodiscard]] const uint8_t* row_data(int16_t r) const { return _grid[r]; }

private:
    uint8_t _grid[ROWS][COLS]{};
    int16_t _remaining = 0;
};

// ── 内联实现 ────────────────────────────────────

inline void BrickGrid::load(const uint8_t level[ROWS][COLS]) {
    _remaining = 0;
    for (int16_t r = 0; r < ROWS; ++r) {
        for (int16_t c = 0; c < COLS; ++c) {
            uint8_t t = level[r][c];
            _grid[r][c] = t;
            if (t >= 1 && t <= 5) ++_remaining;
        }
    }
}

inline uint8_t BrickGrid::hit_test(int16_t x, int16_t y) const {
    int16_t cx = x - OX;
    int16_t cy = y - OY;
    if (cx < 0 || cx >= W || cy < 0 || cy >= H) return 0;
    // 间隙检查：每格最后 1 像素是间隙
    if ((cx % CELL) >= SIZE || (cy % CELL) >= SIZE) return 0;
    return _grid[cy / CELL][cx / CELL];
}

inline int16_t BrickGrid::destroy(int16_t row, int16_t col) {
    uint8_t t = _grid[row][col];
    if (t < 1 || t > 5) return 0;
    _grid[row][col] = 0;
    --_remaining;
    return (6 - t) * 10;
}

inline Rect BrickGrid::brick_rect(int16_t row, int16_t col) {
    return {static_cast<int16_t>(OX + col * CELL), static_cast<int16_t>(OY + row * CELL), SIZE, SIZE};
}

} // namespace handheld::breakout

#endif
