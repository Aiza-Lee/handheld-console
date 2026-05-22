#ifndef SCENES_MENU_MENU_CONFIG_H
#define SCENES_MENU_MENU_CONFIG_H

#include "core/graphics/Color.h"
#include "core/runtime/ScreenType.h"
#include <cstdint>



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
struct MenuEntry {
    const char* name;
    ScreenType screen_type;
};
constexpr MenuEntry ENTRIES[] = {
    {"Playground", ScreenType::PLAYGROUND},
    {"Snake",     ScreenType::SNAKE},
    {"Pac-Man",   ScreenType::PACMAN},
    {"Breakout",  ScreenType::BREAKOUT},
    {"Invaders",  ScreenType::INVADERS},
    {"Grow Ball", ScreenType::GROW_BALL},
};
constexpr std::size_t ENTRY_COUNT = 6;

// 布局
constexpr int16_t BOX_X = 5;
constexpr int16_t BOX_W = 70;
constexpr int16_t BOX_H = 10;
constexpr int16_t BOX_START_Y = 18;
constexpr int16_t BOX_GAP = 3;
constexpr int16_t MAX_VISIBLE = 4;
constexpr int16_t STAR_COUNT = 12;

// 文本
constexpr const char* TITLE = "GAMES";
constexpr const char* HINT = "A/START: select";

} // namespace handheld::menu::cfg



#endif
