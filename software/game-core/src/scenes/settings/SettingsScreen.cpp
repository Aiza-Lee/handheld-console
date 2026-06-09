#include "scenes/settings/SettingsScreen.h"

#include "core/common/Algorithm.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "core/audio/Sounds.h"
#include <stdint.h>

namespace handheld {

namespace {

constexpr Color BG = rgb565(6, 10, 22);
constexpr Color TITLE_COLOR = rgb565(210, 180, 235);
constexpr Color ROW_SELECTED = rgb565(120, 200, 220);
constexpr Color ROW_DIM = rgb565(35, 45, 82);
constexpr Color ROW_TEXT = rgb565(160, 180, 210);
constexpr Color BAR_FILL = rgb565(80, 170, 230);
constexpr Color BAR_BG = rgb565(14, 24, 46);
constexpr Color HINT_COLOR = rgb565(90, 120, 180);
constexpr Color HIGHLIGHT = rgb565(170, 225, 240);

// 布局 — 重新计算避免拥挤
constexpr int16_t ROW_X = 5;
constexpr int16_t ROW_W = 70;
constexpr int16_t ROW_H = 14;
constexpr int16_t ROW_START_Y = 20;
constexpr int16_t ROW_GAP = 5;

constexpr int16_t BAR_X = 20;
constexpr int16_t BAR_W = 36;
constexpr int16_t BAR_H = 7;

constexpr uint8_t VOLUME_STEP = 5;

void draw_progress_bar(IDisplay& display, int16_t bx, int16_t by, uint8_t pct) {
    display.draw_rect(Rect{bx, by, BAR_W, BAR_H}, BAR_BG);
    auto fill = static_cast<int16_t>(BAR_W * pct / 100);
    if (fill > 0) {
        display.fill_rect(Rect{bx, by, fill, BAR_H}, BAR_FILL);
    }
}

} // namespace

void SettingsScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG);
    _selected_row = 0;
    _frame = 0;
}

void SettingsScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    const auto& input = platform.input();

    if (input.was_pressed(ButtonBits::UP)) {
        _selected_row = (_selected_row == 0) ? 1 : 0;
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }
    if (input.was_pressed(ButtonBits::DOWN)) {
        _selected_row = (_selected_row == 0) ? 1 : 0;
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }

    auto& audio = host.audio();

    // 长按连发：按住后延迟 18 帧开始，之后每 4 帧触发一次
    bool left = input.is_down(ButtonBits::LEFT);
    bool right = input.is_down(ButtonBits::RIGHT);
    bool pressed = input.was_pressed(ButtonBits::LEFT) || input.was_pressed(ButtonBits::RIGHT);

    if (left || right) {
        ++_hold_frames;
    } else {
        _hold_frames = 0;
    }

    bool act = pressed || ((_hold_frames >= 15) && ((_hold_frames - 15) % 2 == 0));
    int8_t dir = 0;
    if (left) dir = -1;
    else if (right) dir = 1;

    if (act && dir != 0) {
        uint8_t vol = (_selected_row == 0) ? audio.bgm_volume() : audio.sfx_volume();
        int16_t new_vol = handheld::min(handheld::max(static_cast<int16_t>(vol) + dir * VOLUME_STEP, 0), 100);
        if (_selected_row == 0) {
            audio.set_bgm_volume(static_cast<uint8_t>(new_vol));
        } else {
            audio.set_sfx_volume(static_cast<uint8_t>(new_vol));
        }
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }

    // A = 调整值（增量），同 RIGHT 方向（BUTTONS.md § 2）
    if (input.was_pressed(ButtonBits::A)) {
        uint8_t vol = (_selected_row == 0) ? audio.bgm_volume() : audio.sfx_volume();
        int16_t new_vol = handheld::min(handheld::max(static_cast<int16_t>(vol) + VOLUME_STEP, 0), 100);
        if (_selected_row == 0) {
            audio.set_bgm_volume(static_cast<uint8_t>(new_vol));
        } else {
            audio.set_sfx_volume(static_cast<uint8_t>(new_vol));
        }
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }

    // START = 退出（a11y 双键，与 B 等价；音量已实时生效）
    if (input.was_pressed(ButtonBits::START) || input.was_pressed(ButtonBits::B)) {
        host.audio().play_sfx(sounds::SFX_BACK, sounds::SFX_BACK_COUNT);
        host.switch_to(ScreenType::MENU);
        return;
    }
}

void SettingsScreen::render(IPlatform& platform, IScreenHost& host) {
    IDisplay& display = platform.display();
    display.clear(BG);
    const auto cx = static_cast<int16_t>(display.width() / 2);
    auto& audio = host.audio();

    TextRenderer::draw_text_centered(display, {cx, 4}, "SETTINGS", TITLE_COLOR, 1, BASIC_FONT_5X7);

    // 分隔线
    for (int16_t ux = 10; ux <= 70; ++ux) {
        if (static_cast<uint8_t>((ux + _frame) % 8) < 4) display.draw_pixel(ux, 12, ROW_SELECTED);
    }

    const char* labels[2] = {"BGM", "SFX"};
    uint8_t volumes[2] = {audio.bgm_volume(), audio.sfx_volume()};

    for (uint8_t i = 0; i < 2; ++i) {
        const auto ry = static_cast<int16_t>(ROW_START_Y + i * (ROW_H + ROW_GAP));
        const Rect row_box = {ROW_X, ry, ROW_W, ROW_H};
        bool sel = (_selected_row == i);

        display.draw_rect(row_box, sel ? ROW_SELECTED : ROW_DIM);
        if (sel) {
            display.draw_pixel(row_box.x, row_box.y, HIGHLIGHT);
            display.draw_pixel(static_cast<int16_t>(row_box.x + row_box.width - 1), row_box.y, HIGHLIGHT);
            display.draw_pixel(row_box.x, static_cast<int16_t>(row_box.y + row_box.height - 1), HIGHLIGHT);
            display.draw_pixel(static_cast<int16_t>(row_box.x + row_box.width - 1),
                               static_cast<int16_t>(row_box.y + row_box.height - 1), HIGHLIGHT);
        }

        const auto label_y = static_cast<int16_t>(ry + (ROW_H - COMPACT_FONT_3X5.glyph_size.height) / 2);
        TextRenderer::draw_text(display, {static_cast<int16_t>(ROW_X + 3), label_y}, labels[i],
                                sel ? ROW_SELECTED : ROW_TEXT, 1, COMPACT_FONT_3X5);

        const auto bar_y = static_cast<int16_t>(ry + (ROW_H - BAR_H) / 2);
        draw_progress_bar(display, BAR_X, bar_y, volumes[i]);

        TextRenderer::draw_uint(display, static_cast<int16_t>(BAR_X + BAR_W + 3), label_y, volumes[i],
                                sel ? ROW_SELECTED : ROW_TEXT, COMPACT_FONT_3X5);
    }

    const auto hint_y = static_cast<int16_t>(ROW_START_Y + 2 * (ROW_H + ROW_GAP));
    TextRenderer::draw_text_centered(display, {cx, hint_y}, "A/L/R: Adjust", HINT_COLOR, 1, COMPACT_FONT_3X5);
    TextRenderer::draw_text_centered(display, {cx, static_cast<int16_t>(hint_y + 6)}, "B/START: Exit", HINT_COLOR,
                                     1, COMPACT_FONT_3X5);
}

void SettingsScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t /*frame*/) {
    const auto cx = static_cast<int16_t>(box.x + box.width / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);

    int xs[] = {-3, -2, -2, -3, -1, 0, 1, 1, 0, -1};
    int ys[] = {-2, -1, 0, 1, -3, -2, -1, 0, 1, 2};
    for (int16_t i = 0; i < 10; ++i) {
        display.draw_pixel(static_cast<int16_t>(cx + xs[i]), static_cast<int16_t>(cy + ys[i]), BAR_FILL);
    }
}

} // namespace handheld
