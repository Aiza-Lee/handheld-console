#ifndef SCENES_MENU_MENU_SCREEN_H
#define SCENES_MENU_MENU_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include "core/runtime/ScreenType.h"
#include <cstddef>
#include <cstdint>

class IDisplay;

namespace handheld::menu::cfg {

// 颜色
constexpr Color MENU_BG = rgb565(6, 10, 22);
constexpr Color MENU_TITLE = rgb565(210, 180, 235);
constexpr Color MENU_SELECTED_BORDER = rgb565(120, 200, 220);
constexpr Color MENU_SELECTED_GLOW = rgb565(170, 225, 240);
constexpr Color MENU_BOX_BORDER = rgb565(35, 45, 82);
constexpr Color MENU_BOX_TEXT = rgb565(120, 140, 180);
constexpr Color MENU_HINT = rgb565(90, 120, 180);
constexpr Color MENU_STAR_BRIGHT = rgb565(80, 100, 150);
constexpr Color MENU_STAR_DIM = rgb565(30, 40, 65);
constexpr Color MENU_SCROLL_ARROW = rgb565(180, 160, 210);

// 菜单项
using MenuPreviewFn = void (*)(IDisplay&, const Rect&, uint32_t frame);

struct MenuEntry {
    const char* name;
    ScreenType screen_type;
    MenuPreviewFn render_preview = nullptr; // nullptr = 使用默认文字渲染
};
constexpr MenuEntry ENTRIES[] = {
    {  "Settings",   ScreenType::SETTINGS},
    { "Playground", ScreenType::PLAYGROUND},
    {     "Snake",      ScreenType::SNAKE},
    {   "Pac-Man",     ScreenType::PACMAN},
    {  "Breakout",   ScreenType::BREAKOUT},
    {  "Invaders",   ScreenType::INVADERS},
    { "Grow Ball",  ScreenType::GROW_BALL},
    {   "Tetris",     ScreenType::TETRIS},
    {    "2048",   ScreenType::GAME_2048},
    {     "Pong",       ScreenType::PONG},
    {"MP3 Player",    ScreenType::MP3},
};
constexpr std::size_t ENTRY_COUNT = 11;

// 布局
constexpr int16_t BOX_X = 5;
constexpr int16_t BOX_W = 70;
constexpr int16_t BOX_H = 10;
constexpr int16_t BOX_START_Y = 18;
constexpr int16_t BOX_GAP = 3;
constexpr int16_t MAX_VISIBLE = 4;
constexpr int16_t STAR_COUNT = 12;

// 文本
constexpr const char* TITLE = "MENU";
constexpr const char* HINT = "A/START: select";

} // namespace handheld::menu::cfg

namespace handheld {

// 菜单屏幕 — 展示已注册游戏列表，光标选择后进入
class MenuScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    size_t _cursor = 0;
    size_t _scroll_offset = 0;

    // 背景星星动画
    struct Star {
        int16_t x, y;
        uint8_t speed;
        uint8_t layer;
    };
    Star _stars[menu::cfg::STAR_COUNT];
    uint32_t _frame;
    bool _stars_ready;

    void init_stars();
    void update_stars();
};

// 给定 ScreenType，返回对应的菜单预览回调；无预览时返回 nullptr
// 公开供其他屏幕（如 DeveloperScreen 鸣谢屏）复用
menu::cfg::MenuPreviewFn render_menu_preview_for(ScreenType type);

} // namespace handheld

#endif
