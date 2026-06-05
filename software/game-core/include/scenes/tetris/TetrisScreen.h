#ifndef SCENES_TETRIS_TETRIS_SCREEN_H
#define SCENES_TETRIS_TETRIS_SCREEN_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"

#include <cstdint>

namespace handheld::tetris::cfg {

constexpr bool ENABLE_BGM = true;

// ── 网格尺寸 ──────────────────────────────────────────────
constexpr int16_t COLS = 10;
constexpr int16_t ROWS = 18;
constexpr int16_t CELL = 4;
constexpr int16_t AREA_W = COLS * CELL;  // 40
constexpr int16_t AREA_H = ROWS * CELL;  // 72
constexpr int16_t AREA_X = 20;
constexpr int16_t AREA_Y = 8;

// ── HUD ──────────────────────────────────────────────────
constexpr int16_t STATUS_H = 8;
constexpr int16_t SCREEN_W = 80;
constexpr int16_t SCREEN_H = 80;

// ── 7-bag 随机源 ─────────────────────────────────────────
constexpr uint8_t PIECE_COUNT = 7;

// ── 重力（每行所需逻辑帧数；越小越快）───────────────────
constexpr uint32_t INIT_INTERVAL = 30;
constexpr uint32_t MIN_INTERVAL = 5;
constexpr uint8_t SPEED_STEP_LINES = 10;
constexpr int8_t SPEED_DECREMENT = 2;

// ── 计分表（1-4 行同时消除的得分，乘以 level）───────────
constexpr uint16_t SCORE_TABLE[5] = {0, 100, 300, 500, 800};

// ── 方块配色 ──────────────────────────────────────────────
constexpr Color COLOR_I = rgb565(0, 240, 240);
constexpr Color COLOR_O = rgb565(240, 240, 0);
constexpr Color COLOR_T = rgb565(160, 0, 240);
constexpr Color COLOR_S = rgb565(0, 240, 0);
constexpr Color COLOR_Z = rgb565(240, 0, 0);
constexpr Color COLOR_L = rgb565(240, 160, 0);
constexpr Color COLOR_J = rgb565(0, 0, 240);

constexpr Color COLOR_BG = rgb565(8, 12, 16);
constexpr Color COLOR_GRID = rgb565(20, 24, 28);
constexpr Color COLOR_BORDER = rgb565(50, 60, 70);
constexpr Color COLOR_HUD = rgb565(180, 200, 220);
constexpr Color COLOR_HINT = rgb565(80, 100, 120);
constexpr Color COLOR_GAMEOVER = rgb565(255, 145, 190);
constexpr Color COLOR_PAUSE = rgb565(100, 200, 140);
constexpr Color COLOR_OVERLAY = rgb565(8, 12, 16);

// ── 覆盖层尺寸 ────────────────────────────────────────────
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 16;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 48;

// ── 方块位掩码（4×4，低 4 位有效；bit 0 = 最左列）───────
struct PieceDef {
    uint16_t rows[4][4]; // [rotation][row] = 该行 4 格的位掩码
    Color color;
};

constexpr PieceDef PIECES[7] = {
    // I（青色，4×4 旋转盒）
    { { {0x0, 0xF, 0x0, 0x0},
        {0x2, 0x2, 0x2, 0x2},
        {0x0, 0xF, 0x0, 0x0},
        {0x2, 0x2, 0x2, 0x2} }, COLOR_I },
    // O（黄色，2×2 居中）
    { { {0x0, 0x6, 0x6, 0x0},
        {0x0, 0x6, 0x6, 0x0},
        {0x0, 0x6, 0x6, 0x0},
        {0x0, 0x6, 0x6, 0x0} }, COLOR_O },
    // T（紫色，3×3）
    { { {0x2, 0x7, 0x0, 0x0},
        {0x2, 0x6, 0x2, 0x0},
        {0x0, 0x7, 0x2, 0x0},
        {0x2, 0x3, 0x2, 0x0} }, COLOR_T },
    // S（绿色，3×3）
    { { {0x6, 0xC, 0x0, 0x0},
        {0x1, 0x3, 0x2, 0x0},
        {0x6, 0xC, 0x0, 0x0},
        {0x1, 0x3, 0x2, 0x0} }, COLOR_S },
    // Z（红色，3×3）
    { { {0x6, 0x3, 0x0, 0x0},
        {0x2, 0x3, 0x1, 0x0},
        {0x6, 0x3, 0x0, 0x0},
        {0x2, 0x3, 0x1, 0x0} }, COLOR_Z },
    // L（橙色，3×3）
    { { {0x4, 0x7, 0x0, 0x0},
        {0x2, 0x2, 0x3, 0x0},
        {0x0, 0x7, 0x4, 0x0},
        {0x6, 0x2, 0x2, 0x0} }, COLOR_L },
    // J（蓝色，3×3）
    { { {0x1, 0x7, 0x0, 0x0},
        {0x3, 0x2, 0x2, 0x0},
        {0x0, 0x7, 0x1, 0x0},
        {0x2, 0x2, 0x6, 0x0} }, COLOR_J },
};

} // namespace handheld::tetris::cfg

namespace handheld {

class TetrisScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    // 堆叠区（0 = 空，1..7 = 颜色索引 = 方块类型 + 1）
    uint8_t _stack[tetris::cfg::COLS * tetris::cfg::ROWS]{};

    // 7-bag 随机源
    uint8_t _bag[tetris::cfg::PIECE_COUNT]{};
    uint8_t _bag_idx = 0;

    // 当前与下一个方块
    uint8_t _current_piece = 0;
    uint8_t _current_rot = 0;
    uint8_t _next_piece = 0;
    int8_t _current_x = 0;
    int8_t _current_y = 0;

    // 状态
    uint16_t _score = 0;
    uint8_t _level = 1;
    uint8_t _lines = 0;
    uint32_t _gravity_counter = 0;
    uint32_t _gravity_interval = tetris::cfg::INIT_INTERVAL;
    uint32_t _rng = 12345;
    uint32_t _frame = 0;
    bool _paused = false;
    bool _game_over = false;

    void reset_game();
    void refill_bag();
    uint8_t pop_bag();
    void spawn_piece();
    void lock_piece(IScreenHost& host);

    [[nodiscard]] bool piece_cell(uint8_t piece, uint8_t rot, int row, int col) const;
    [[nodiscard]] bool collides(int8_t x, int8_t y, uint8_t piece, uint8_t rot) const;
    void try_rotate();
    uint32_t next_rng();

    void draw_cell(IDisplay& d, int col, int row, Color color) const;
};

} // namespace handheld

#endif
