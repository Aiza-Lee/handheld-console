#ifndef SCENES_GAME2048_GAME_2048_SCREEN_H
#define SCENES_GAME2048_GAME_2048_SCREEN_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::game2048::cfg {

// ── 棋盘尺寸 ─────────────────────────────────────────
constexpr int8_t GRID = 4;        // 4x4
constexpr int16_t CELL = 16;      // 每格 16 像素
constexpr int16_t BOARD_W = GRID * CELL; // 64
constexpr int16_t BOARD_H = GRID * CELL; // 64
constexpr int16_t BOARD_X = (80 - BOARD_W) / 2; // 8
constexpr int16_t BOARD_Y = 14;   // 顶部留 14px 给 HUD
constexpr int16_t BORDER_X = BOARD_X - 2; // 6
constexpr int16_t BORDER_Y = BOARD_Y - 2; // 12
constexpr int16_t BORDER_W = BOARD_W + 4; // 68
constexpr int16_t BORDER_H = BOARD_H + 4; // 68

// ── HUD ──────────────────────────────────────────────
constexpr int16_t HUD_X = 2;
constexpr int16_t HUD_Y = 3;
constexpr int16_t HUD_W = 76;
constexpr int16_t HUD_H = 7;

// ── 覆盖层尺寸 ──────────────────────────────────────
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 22;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 36;

constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 18;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 44;

constexpr int16_t WIN_RECT_X = 10;
constexpr int16_t WIN_RECT_Y = 30;
constexpr int16_t WIN_RECT_W = 60;
constexpr int16_t WIN_RECT_H = 20;

// ── 颜色 ────────────────────────────────────────────
constexpr Color BG_COLOR = rgb565(20, 18, 30);
constexpr Color HUD_BG = rgb565(30, 26, 44);
constexpr Color HUD_TEXT = rgb565(180, 180, 200);
constexpr Color BOARD_BG = rgb565(40, 36, 56);
constexpr Color BORDER_COLOR = rgb565(80, 70, 110);
constexpr Color EMPTY_TILE = rgb565(50, 46, 70);
constexpr Color TILE_TEXT_DARK = rgb565(40, 30, 20);
constexpr Color TILE_TEXT_LIGHT = rgb565(245, 240, 230);
constexpr Color GAMEOVER_COLOR = rgb565(220, 80, 100);
constexpr Color WIN_COLOR = rgb565(120, 220, 160);
constexpr Color HINT_COLOR = rgb565(140, 140, 160);
constexpr Color OVERLAY_BG = rgb565(8, 6, 16);
constexpr Color PAUSE_BG = rgb565(8, 6, 16);
constexpr Color PAUSE_BORDER = rgb565(120, 140, 200);
constexpr Color PAUSE_TEXT = rgb565(180, 200, 255);

// ── 游戏参数 ───────────────────────────────────────
constexpr uint32_t RNG_SEED = 12345;
constexpr uint16_t WIN_VALUE = 2048;
constexpr int8_t MAX_LOG10 = 4; // 最大 4 位数 (2048)

} // namespace handheld::game2048::cfg

namespace handheld {

class Game2048Screen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    uint16_t _board[game2048::cfg::GRID][game2048::cfg::GRID]{};
    uint32_t _score = 0;
    uint32_t _rng = game2048::cfg::RNG_SEED;
    uint32_t _frame = 0;

    bool _game_over = false;
    bool _won = false;
    bool _paused = false;
    bool _keep_playing = false; // 胜利后继续玩

    uint32_t next_rng();
    void reset_game();
    void spawn_tile();
    bool slide(int8_t dir);
    bool can_move() const;
    [[nodiscard]] Color tile_color(uint16_t value) const;
    [[nodiscard]] bool tile_text_dark(uint16_t value) const;
    void draw_tile(IDisplay& d, int8_t row, int8_t col, uint16_t value) const;
    void draw_number(IDisplay& d, int16_t cx, int16_t cy, uint16_t value) const;
};

} // namespace handheld

#endif
