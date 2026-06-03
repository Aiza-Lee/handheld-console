#ifndef SCENES_INVADERS_INVADERS_SCREEN_H
#define SCENES_INVADERS_INVADERS_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::invaders::cfg {

// ── 音频 ─────────────────────────────────────────────────────────
constexpr bool ENABLE_BGM = true;

// ── 屏幕 ─────────────────────────────────────────────────────────
constexpr int16_t SCREEN_WIDTH = 80;
constexpr int16_t SCREEN_HEIGHT = 80;

// ── 颜色 ─────────────────────────────────────────────────────────
constexpr Color INV_BG = rgb565(4, 8, 20);
constexpr Color INV_PLAYER = rgb565(100, 200, 220);
constexpr Color INV_PLAYER_ENGINE = rgb565(200, 160, 120);
constexpr Color INV_BULLET = rgb565(220, 240, 255);
constexpr Color INV_E_ROW0 = rgb565(180, 80, 120);
constexpr Color INV_E_ROW1 = rgb565(140, 100, 200);
constexpr Color INV_E_ROW2 = rgb565(80, 160, 100);
constexpr Color INV_E_ROW3 = rgb565(200, 140, 60);
constexpr Color INV_EBULLET = rgb565(255, 145, 190);
constexpr Color INV_EXPLOSION = rgb565(255, 200, 100);
constexpr Color INV_STATUS_BG = rgb565(6, 14, 30);
constexpr Color INV_GROUND = rgb565(20, 30, 50);
constexpr Color INV_TEXT = rgb565(180, 200, 230);
constexpr Color INV_LIFE_COLOR = rgb565(100, 200, 220);
constexpr Color INV_WARN_COLOR = rgb565(255, 160, 180);
constexpr Color INV_HINT_COLOR = rgb565(120, 140, 180);
constexpr Color PAUSE_BG = rgb565(5, 10, 25);
constexpr Color PAUSE_TEXT = rgb565(150, 210, 230);
constexpr Color SHIELD_COLOR = rgb565(60, 140, 220);
constexpr Color SAUCER_COLOR = rgb565(220, 60, 180);

// ── 覆盖层尺寸 ───────────────────────────────────────────────────
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr int16_t END_RECT_OY = 28;
constexpr int16_t END_RECT_WIN_H = 38;
constexpr int16_t END_RECT_LOSE_H = 42;

// ── 布局 ─────────────────────────────────────────────────────────
constexpr int16_t GRID_COLS = 6;
constexpr int16_t GRID_ROWS = 4;
constexpr int16_t TOTAL_ENEMIES = GRID_COLS * GRID_ROWS;
constexpr int16_t ENEMY_W = 6;
constexpr int16_t ENEMY_H = 5;
constexpr int16_t CELL_W = 7;
constexpr int16_t CELL_H = 8;
constexpr int16_t GRID_OX = 19;
constexpr int16_t GRID_OY = 12;
constexpr int16_t PLAYER_W = 7;
constexpr int16_t PLAYER_H = 5;
constexpr int16_t PLAYER_Y = 72;
constexpr int16_t STATUS_H = 8;
constexpr int16_t PLAY_AREA_LEFT = 0;
constexpr int16_t PLAY_AREA_RIGHT = SCREEN_WIDTH - PLAYER_W;
constexpr int16_t ENEMY_AREA_RIGHT = SCREEN_WIDTH - ENEMY_W;

// ── 游戏参数 (20 FPS 基准) ───────────────────────────────────────
constexpr uint32_t INIT_MOVE_INTERVAL = 6;
constexpr uint32_t MIN_MOVE_INTERVAL = 2;
constexpr int8_t INIT_LIVES = 3;
constexpr int16_t PLAYER_SPEED = 2;
constexpr int16_t ENEMY_DROP = 6;
constexpr int16_t PLAYER_START_X = 36;
constexpr uint32_t DYING_TIMER = 15;

// ── 子弹 ─────────────────────────────────────────────────────────
constexpr int16_t PLAYER_BULLET_SPEED = 5;
constexpr int16_t ENEMY_BULLET_SPEED = 3;
constexpr int16_t PLAYER_BULLET_TRAIL = 4;       // 玩家子弹拖尾长度
constexpr int16_t ENEMY_BULLET_TRAIL = 3;        // 敌人子弹拖尾长度
constexpr uint32_t SHOOT_INTERVAL_BASE = 8;      // 敌人射击间隔基础值
constexpr uint32_t SHOOT_INTERVAL_PER_ENEMY = 1; // 每个存活敌人增加的射击间隔
constexpr uint32_t MIN_SHOOT_BASE = 4;           // 射击间隔基础值下限
constexpr int16_t MAX_PARTICLES = 12;
constexpr int16_t MAX_EBULLETS = 2;
constexpr int16_t MAX_PARTICLES_PER_EXPLOSION = 6;

// ── 护盾 ─────────────────────────────────────────────────────────
constexpr int16_t SHIELD_COUNT = 4;
constexpr int16_t SHIELD_W = 8;
constexpr int16_t SHIELD_H = 6;
constexpr int16_t SHIELD_Y = 60;  // 护盾顶部 Y
constexpr int16_t SHIELD_GAP = 8; // 护盾间距
constexpr int16_t SHIELD_OX = (SCREEN_WIDTH - SHIELD_COUNT * SHIELD_W - (SHIELD_COUNT - 1) * SHIELD_GAP) / 2;

// ── 神秘飞船 (UFO) ──────────────────────────────────────────────
constexpr int16_t SAUCER_Y = 10; // 飞船飞行高度
constexpr int16_t SAUCER_W = 14;
constexpr int16_t SAUCER_H = 5;
constexpr int16_t SAUCER_SPEED = 1;
constexpr uint32_t SAUCER_MIN_INTERVAL = 120; // 最短出现间隔（帧，6s @20fps）
constexpr uint32_t SAUCER_MAX_INTERVAL = 360; // 最长出现间隔（帧，18s @20fps）
constexpr int16_t SAUCER_SCORES[] = {50, 100, 150, 200, 300};
constexpr uint8_t SAUCER_SCORE_DISPLAY_FRAMES = 20; // 得分闪烁帧数 (1s @20fps)

// ── 关卡 ─────────────────────────────────────────────────────────
constexpr uint32_t LEVEL_CLEAR_DURATION = 30;        // 通关庆祝帧数 (1.5s @20fps)
constexpr int16_t LEVEL_Y_OFFSET_PER_LEVEL = 3;      // 每关敌人起始位置下移像素
constexpr int16_t LEVEL_SPEED_STEP = 1;              // 每关敌人移动间隔递减量
constexpr int16_t SHOOT_INTERVAL_STEP_PER_LEVEL = 2; // 每关射击间隔递减量
constexpr int16_t MAX_VICTORY_LEVEL = 6;             // 通关关卡数

// ── 分数 ─────────────────────────────────────────────────────────
constexpr int16_t ROW_SCORES[4] = {40, 30, 20, 10};

// ── 敌人形状 (6x5, 每行两个动画帧) ──────────────────────────────
constexpr const char* ENEMY_SHAPES[4][2] = {
    {
     " X  X "
     "  XX  "
     " XXXX "
     "X XX X"
     "X XX X", " X  X "
 " XXXX "
 " XXXX "
 "X XX X"
 " XX XX", },
    {
     "X    X"
     " XX XX"
     " XXXX "
     " XX XX"
     "X XX X", "X    X"
 "XX  XX"
 " XXXX "
 "X XX X"
 " X X X", },
    {
     "  XX  "
     " XXXX "
     "XXXXXX"
     "X XX X"
     "X XX X", " XXXX "
 " XXXX "
 "XXXXXX"
 "X XX X"
 "X XX X", },
    {
     "  XX  "
     " XXXX "
     "XXXXXX"
     "X XX X"
     "X XX X", " XXXX "
 " XXXX "
 "XXXXXX"
 "X XX X"
 "X XX X", },
};

} // namespace handheld::invaders::cfg

namespace handheld {

// 太空侵略者屏幕 — 经典射击游戏
class InvadersScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    enum class State : uint8_t { PLAYING, DYING, LEVEL_CLEAR, GAME_OVER };

    struct EnemyBullet {
        int16_t x, y;
        bool active;
    };

    struct Particle {
        int8_t x, y;
        int8_t vx, vy;
        uint8_t life;
    };

    struct Saucer {
        int16_t x, y;
        int8_t dir;
        uint16_t points;
        bool alive;
    };

    // 敌人状态
    bool _enemy_alive[invaders::cfg::GRID_COLS * invaders::cfg::GRID_ROWS];
    int16_t _enemy_base_x;
    int16_t _enemy_base_y;
    int8_t _enemy_dir_x;
    int16_t _enemies_alive_count;

    // 玩家
    int16_t _player_x;
    State _state;
    bool _paused = false;

    // 子弹
    int16_t _bullet_x, _bullet_y;
    bool _bullet_active;
    EnemyBullet _ebullets[invaders::cfg::MAX_EBULLETS];

    // 护盾
    bool _shields[invaders::cfg::SHIELD_COUNT][invaders::cfg::SHIELD_H][invaders::cfg::SHIELD_W];

    // 神秘飞船
    Saucer _saucer;
    uint32_t _saucer_timer;
    uint8_t _saucer_score_display;
    int16_t _saucer_score_x, _saucer_score_y;

    // 爆炸粒子
    Particle _particles[invaders::cfg::MAX_PARTICLES];
    int16_t _particle_count;

    // 分数 & 关卡
    int16_t _score;
    int8_t _lives;
    int16_t _level;
    uint32_t _level_clear_timer;

    // 计时
    uint32_t _frame;
    uint32_t _move_timer;
    uint32_t _move_interval;
    uint32_t _shoot_timer;
    uint32_t _dying_timer;
    uint32_t _rng;

    void reset_game();
    void start_next_level();
    void respawn_player();
    void player_shoot(IScreenHost& host);
    void enemy_shoot(IScreenHost& host);
    void move_enemies(IScreenHost& host);
    void move_bullets(IScreenHost& host);
    void update_saucer(IScreenHost& host);
    void init_shields();
    bool damage_shield(int16_t x, int16_t y);
    void spawn_particles(int16_t x, int16_t y);
    void update_particles();
    void draw_enemy(IDisplay& display, int16_t px, int16_t py, int16_t row, uint32_t frame) const;
    void draw_player(IDisplay& display, int16_t x, uint32_t frame) const;
    void draw_shields(IDisplay& display) const;
    void draw_saucer(IDisplay& display) const;
    uint32_t next_rng();
};

} // namespace handheld

#endif // SCENES_INVADERS_INVADERS_SCREEN_H
