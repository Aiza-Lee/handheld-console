#ifndef SCENES_DEVELOPER_DEVELOPER_SCREEN_H
#define SCENES_DEVELOPER_DEVELOPER_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include <cstddef>
#include <cstdint>

namespace handheld::developer::cfg {

constexpr int16_t SCROLL_PX_PER_FRAME = 1;     // ~30 px/sec at 30 fps
constexpr int16_t LINE_H = 8;                  // BASIC_FONT_5X7 line height
// 调色板：与 boot/menu 同一基调（深蓝底 + 浅青字），保持高对比
constexpr Color BG = rgb565(8, 12, 30);            // 同 BOOT_BG
constexpr Color FG = rgb565(170, 225, 240);        // 同 MENU_SELECTED_GLOW
constexpr Color ACCENT = rgb565(120, 200, 220);    // 同 BOOT_TITLE / MENU_SELECTED_BORDER

// 通用 credits 占位文本（短）。
inline constexpr const char* CREDITS[] = {
    "== HANDHELD ==",
    "",
    "CONSOLE V0.1",
    "",
    "",
    "Credits",
    "----",
    "Li Jie",
	"Jing Yicheng",
	"Liu Hongrun",
    "",
    "",
    "Special",
    "Thanks To",
    "----",
    "Yu Kaihao",
    "",
    "",
    "Press B",
    "to return",
};
inline constexpr size_t CREDITS_LINES = sizeof(CREDITS) / sizeof(CREDITS[0]);

} // namespace handheld::developer::cfg

namespace handheld {

// 隐藏开发人员 / 鸣谢屏幕 — 通过在 BOOT 屏幕 1.5 秒内连按 B 5 次触发
class DeveloperScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    int16_t _scroll_y = 0;
    uint32_t _frame = 0;
};

} // namespace handheld

#endif
