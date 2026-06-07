#include "scenes/menu/MenuScreen.h"
#include "scenes/breakout/BreakoutScreen.h"
#include "scenes/growball/GrowBallScreen.h"
#include "scenes/invaders/InvadersScreen.h"
#include "scenes/pacman/PacmanScreen.h"
#include "scenes/settings/SettingsScreen.h"
#include "scenes/snake/SnakeScreen.h"
#include "scenes/tetris/TetrisScreen.h"
#include "scenes/game2048/Game2048Screen.h"
#include "scenes/pong/PongScreen.h"
// #include "scenes/playground/PlaygroundScreen.h"
// #include "scenes/mp3/Mp3PlayerScreen.h"

#include <algorithm>
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/audio/Sounds.h"

namespace handheld {

using namespace menu::cfg;

namespace {

void draw_scroll_arrow(IDisplay& display, int16_t x, int16_t y, bool up, Color color) {
    if (up) {
        display.draw_pixel(x, static_cast<int16_t>(y + 1), color);
        display.draw_pixel(static_cast<int16_t>(x + 1), y, color);
        display.draw_pixel(static_cast<int16_t>(x + 2), static_cast<int16_t>(y + 1), color);
    } else {
        display.draw_pixel(x, y, color);
        display.draw_pixel(static_cast<int16_t>(x + 1), static_cast<int16_t>(y + 1), color);
        display.draw_pixel(static_cast<int16_t>(x + 2), y, color);
    }
}

} // namespace

// 场景 → 菜单预览回调（公开，供其他屏幕如 DeveloperScreen 复用）
menu::cfg::MenuPreviewFn render_menu_preview_for(ScreenType type) {
    switch (type) {
        case ScreenType::SETTINGS: return SettingsScreen::render_menu_preview;
        // case ScreenType::PLAYGROUND: return PlaygroundScreen::render_menu_preview;
        case ScreenType::SNAKE: return SnakeScreen::render_menu_preview;
        case ScreenType::PACMAN: return PacmanScreen::render_menu_preview;
        case ScreenType::BREAKOUT: return BreakoutScreen::render_menu_preview;
        case ScreenType::INVADERS: return InvadersScreen::render_menu_preview;
        case ScreenType::GROW_BALL: return GrowBallScreen::render_menu_preview;
        case ScreenType::TETRIS: return TetrisScreen::render_menu_preview;
        case ScreenType::GAME_2048: return Game2048Screen::render_menu_preview;
        case ScreenType::PONG: return PongScreen::render_menu_preview;
        // case ScreenType::MP3: return Mp3PlayerScreen::render_menu_preview;
        default: return nullptr;
    }
}

void MenuScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(Color::BLACK);
    init_stars();
    host.audio().set_bgm(sounds::BGM_MENU, sounds::BGM_MENU_COUNT);
}

void MenuScreen::init_stars() {
    _rng_state = 54321;
    for (auto& _star : _stars) {
        _star.x = static_cast<int16_t>(next_rng() % 80);
        _star.y = static_cast<int16_t>(next_rng() % 80);
        _star.speed = static_cast<uint8_t>(1 + (next_rng() % 2));
        _star.layer = static_cast<uint8_t>(next_rng() % 2);
    }
    _stars_ready = true;
    _frame = 0;
}

uint32_t MenuScreen::next_rng() {
    _rng_state = _rng_state * 1103515245 + 12345;
    return _rng_state;
}

void MenuScreen::update_stars() {
    for (auto& _star : _stars) {
        _star.y -= _star.speed;
        if (_star.y < 0) {
            _star.y = 79;
            _star.x = static_cast<int16_t>(next_rng() % 80);
        }
    }
}

void MenuScreen::update(IPlatform& platform, IScreenHost& host) {
    const auto& input = platform.input();
    ++_frame;
    update_stars();

    if (input.was_pressed(ButtonBits::UP)) {
        _cursor = (_cursor == 0) ? ENTRY_COUNT - 1 : _cursor - 1;
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }
    if (input.was_pressed(ButtonBits::DOWN)) {
        _cursor = (_cursor + 1 >= ENTRY_COUNT) ? 0 : _cursor + 1;
        host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
    }
    if (input.was_pressed(ButtonBits::B)) {
        host.audio().stop_bgm();
        host.switch_to(ScreenType::BOOT);
        return;
    }
    if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
        host.audio().play_sfx(sounds::SFX_CONFIRM, sounds::SFX_CONFIRM_COUNT);
        host.switch_to(ENTRIES[_cursor].screen_type);
    }

    if (_cursor < _scroll_offset) {
        _scroll_offset = (_cursor / MAX_VISIBLE) * MAX_VISIBLE;
    } else if (_cursor >= _scroll_offset + MAX_VISIBLE) {
        _scroll_offset = (_cursor / MAX_VISIBLE) * MAX_VISIBLE;
    }
}

void MenuScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(MENU_BG);
    const auto cx = static_cast<int16_t>(display.width() / 2);

    for (auto& _star : _stars)
        display.draw_pixel(_star.x, _star.y, (_star.layer == 0) ? MENU_STAR_DIM : MENU_STAR_BRIGHT);

    TextRenderer::draw_text_centered(display, {cx, 6}, TITLE, MENU_TITLE, 1, BASIC_FONT_5X7);

    for (int16_t ux = 15; ux <= 65; ++ux) {
        if (static_cast<uint8_t>((ux + _frame) % 8) < 4) display.draw_pixel(ux, 14, MENU_SELECTED_BORDER);
    }

    const size_t end = _scroll_offset + MAX_VISIBLE;
    const size_t render_end = (end > ENTRY_COUNT) ? ENTRY_COUNT : end;

    for (size_t i = _scroll_offset; i < render_end; ++i) {
        const auto by = static_cast<int16_t>(BOX_START_Y + ((i - _scroll_offset) * (BOX_H + BOX_GAP)));
        const Rect box = {BOX_X, by, BOX_W, BOX_H};
        bool selected = (i == _cursor);
        const auto& entry = ENTRIES[i];
        auto preview = render_menu_preview_for(entry.screen_type);

        Color border_color =
            selected ? (((_frame / 12) % 2 == 0) ? MENU_SELECTED_BORDER : MENU_SELECTED_GLOW) : MENU_BOX_BORDER;
        Color text_color = selected ? border_color : MENU_BOX_TEXT;

        display.draw_rect(box, border_color);
        if (selected) {
            display.draw_pixel(box.x, box.y, MENU_SELECTED_GLOW);
            display.draw_pixel(static_cast<int16_t>(box.x + box.width - 1), box.y, MENU_SELECTED_GLOW);
            display.draw_pixel(box.x, static_cast<int16_t>(box.y + box.height - 1), MENU_SELECTED_GLOW);
            display.draw_pixel(static_cast<int16_t>(box.x + box.width - 1),
                               static_cast<int16_t>(box.y + box.height - 1), MENU_SELECTED_GLOW);
        }

        // 文字标签：左侧，垂直居中（相对于 box）
        const auto text_y = static_cast<int16_t>(box.y + (box.height - COMPACT_FONT_3X5.glyph_size.height) / 2);
        TextRenderer::draw_text(display, {static_cast<int16_t>(box.x + 3), text_y}, entry.name, text_color, 1,
                                COMPACT_FONT_3X5);

        // 预览图标：右侧，box 内偏移
        if (preview) {
            const auto prev_w = static_cast<int16_t>(box.height); // 正方形预览区
            const Rect prev_box = {
                static_cast<int16_t>(box.x + box.width - prev_w - 2),
                box.y,
                prev_w,
                box.height,
            };
            preview(display, prev_box, _frame);
        }
    }

    bool up_visible = (_scroll_offset > 0);
    bool down_visible = (render_end < ENTRY_COUNT);
    if (up_visible)
        draw_scroll_arrow(display, static_cast<int16_t>(cx - 1), 12, true,
                          ((_frame / 20) % 2 == 0) ? MENU_SCROLL_ARROW : MENU_BG);
    if (down_visible) {
        draw_scroll_arrow(display, static_cast<int16_t>(cx - 1),
                          static_cast<int16_t>(BOX_START_Y + (MAX_VISIBLE * (BOX_H + BOX_GAP)) - BOX_GAP), false,
                          ((_frame / 20) % 2 == 0) ? MENU_SCROLL_ARROW : MENU_BG);
    }

    TextRenderer::draw_text_centered(display, {cx, 72}, HINT, MENU_HINT, 1, COMPACT_FONT_3X5);
}

} // namespace handheld
