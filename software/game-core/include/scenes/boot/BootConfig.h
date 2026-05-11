#ifndef SCENES_BOOT_BOOT_CONFIG_H
#define SCENES_BOOT_BOOT_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {
namespace boot {
namespace cfg {

// 颜色
constexpr Color BOOT_BG = rgb565(8, 12, 30);
constexpr Color BOOT_TITLE = rgb565(120, 200, 220);
constexpr Color BOOT_TITLE_GLOW = rgb565(180, 225, 240);
constexpr Color BOOT_HINT = rgb565(90, 120, 180);
constexpr Color BOOT_STAR_BRIGHT = rgb565(150, 190, 220);
constexpr Color BOOT_STAR_MID = rgb565(60, 80, 120);
constexpr Color BOOT_STAR_DIM = rgb565(30, 40, 65);
constexpr Color BOOT_BORDER = rgb565(25, 35, 60);

constexpr int16_t STAR_COUNT = 16;
constexpr const char* TITLE = "JLL Gamepad";
constexpr const char* HINT = "Press any key";

// 布局
constexpr int16_t TITLE_Y = 28;
constexpr int16_t GLOW_Y = 35;
constexpr int16_t RULE_Y = 43;
constexpr int16_t HINT_Y = 50;

}  // namespace cfg
}  // namespace boot
}  // namespace handheld

#endif
