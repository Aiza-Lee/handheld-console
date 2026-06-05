#include "scenes/developer/DeveloperScreen.h"

#include "scenes/menu/MenuScreen.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include <cstddef>
#include <cstdint>

namespace handheld {

using namespace developer::cfg;

void DeveloperScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG);
    // 从屏幕上方开始滚入；负值表示文字初始位于屏幕顶部之外
    _scroll_y = static_cast<int16_t>(-platform.display().height());
    _frame = 0;
    host.audio().stop_bgm();
}

void DeveloperScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    const auto& input = platform.input();

    // 任意时刻 B / A / START 都返回 MENU
    if (input.was_pressed(ButtonBits::B) ||
        input.was_pressed(ButtonBits::A) ||
        input.was_pressed(ButtonBits::START)) {
        host.switch_to(ScreenType::MENU);
        return;
    }

    _scroll_y = static_cast<int16_t>(_scroll_y + SCROLL_PX_PER_FRAME);
}

void DeveloperScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG);

    const int16_t total_lines_y = static_cast<int16_t>(CREDITS_LINES * LINE_H);
    const int16_t screen_h = display.height();

    // 全部滚出屏幕后停止滚动（停在末尾）
    if (_scroll_y > total_lines_y - LINE_H * 2 - 1) {
        _scroll_y = total_lines_y - LINE_H * 2 - 1;
    }

    // 逐行裁剪绘制：只有进入 [0, screen_h) 范围的行才画；每行水平居中
    for (size_t i = 0; i < CREDITS_LINES; ++i) {
        const int16_t y = static_cast<int16_t>(static_cast<int32_t>(i) * LINE_H - _scroll_y);
        if (y + LINE_H <= 0 || y >= screen_h) continue;
        TextRenderer::draw_text_centered(display, {40, static_cast<int16_t>(y + LINE_H / 2)}, CREDITS[i], FG, 1, BASIC_FONT_5X7);
    }

    // 底部游戏图标网格：跟文字一起向上滚，从屏幕下方滚入
    // 图标块顶 y = 最后一行的"自然位置" + 视觉间距，跟着 _scroll_y 走
    // 单元格 10x10（与 menu 的 BOX_H 一致，所有 preview 不用改即可正确显示）
    constexpr int16_t ICON_CELL = 10;
    constexpr int16_t ICON_GAP_X = 2;
    constexpr int16_t ICON_GAP_Y = 2;
    constexpr int16_t ICONS_PER_ROW = 4;
    constexpr int16_t GAP_AFTER_TEXT = 7;
    constexpr int16_t ICON_BLOCK_H = 3 * ICON_CELL + 2 * ICON_GAP_Y;  // 34
    const int16_t icons_block_y = static_cast<int16_t>(
        static_cast<int32_t>(CREDITS_LINES) * LINE_H - _scroll_y + GAP_AFTER_TEXT);

    // 图标块有部分进入屏幕时才绘制
    if (icons_block_y + ICON_BLOCK_H > 0 && icons_block_y < screen_h) {
        constexpr int16_t GRID_W = ICONS_PER_ROW * ICON_CELL + (ICONS_PER_ROW - 1) * ICON_GAP_X;
        const int16_t grid_x = static_cast<int16_t>((platform.display().width() - GRID_W) / 2);

        constexpr size_t N = sizeof(menu::cfg::ENTRIES) / sizeof(menu::cfg::ENTRIES[0]);
        for (size_t i = 0; i < N; ++i) {
            const auto& entry = menu::cfg::ENTRIES[i];
            const auto preview = render_menu_preview_for(entry.screen_type);
            if (!preview) continue;
            const int16_t col = static_cast<int16_t>(i % ICONS_PER_ROW);
            const int16_t row = static_cast<int16_t>(i / ICONS_PER_ROW);
            const int16_t x = static_cast<int16_t>(grid_x + col * (ICON_CELL + ICON_GAP_X));
            const int16_t y = static_cast<int16_t>(icons_block_y + row * (ICON_CELL + ICON_GAP_Y));
            // 跳过完全在屏幕外的图标
            if (y + ICON_CELL <= 0 || y >= screen_h) continue;
            preview(display, {x, y, ICON_CELL, ICON_CELL}, _frame);
        }
    }
}

} // namespace handheld
