#include "scenes/boot/BootScreen.h"
#include "scenes/boot/BootConfig.h"

#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "core/audio/Sounds.h"
#include "core/random/Random.h"

namespace handheld {

using namespace boot::cfg;

void BootScreen::init_stars() {
    for (auto& s : _stars) {
        s.x = static_cast<int16_t>(random::next() % 80);
        s.y = static_cast<int16_t>(random::next() % 80);
        s.speed = static_cast<uint8_t>(1 + (random::next() % 3));
        s.layer = static_cast<uint8_t>(random::next() % 3);
    }
}

void BootScreen::update_stars() {
    for (auto& s : _stars) {
        s.y += s.speed;
        if (s.y >= 80) {
            s.y = 0;
            s.x = static_cast<int16_t>(random::next() % 80);
        }
    }
}

void BootScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(Color::BLACK);
    init_stars();
    _stars_ready = true;
    _frame = 0;
    _b_press_count = 0;
    _last_b_ms = 0;
    host.audio().play_sfx(sounds::BOOT, sounds::BOOT_COUNT);
}

void BootScreen::update(IPlatform& platform, IScreenHost& host) {
    ++_frame;
    update_stars();
    const auto& input = platform.input();
    const uint32_t now = platform.time().ticks_ms();

    if (input.was_pressed(ButtonBits::SELECT)) {
        if (now - _last_b_ms > DEV_TRIGGER_WINDOW_MS) _b_press_count = 0;
        _last_b_ms = now;
        ++_b_press_count;
        if (_b_press_count >= DEV_TRIGGER_COUNT) {
            _b_press_count = 0;
            host.switch_to(ScreenType::DEVELOPER);
        }
        return;
    }
    for (auto btn : {ButtonBits::A, ButtonBits::START}) {
        if (input.was_pressed(btn)) {
            _b_press_count = 0;
            host.switch_to(ScreenType::MENU);
            return;
        }
    }
}

void BootScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BOOT_BG);
    const auto cx = static_cast<int16_t>(display.width() / 2);

    for (auto& s : _stars) {
        Color sc = (s.layer == 0) ? BOOT_STAR_DIM : (s.layer == 1) ? BOOT_STAR_MID : BOOT_STAR_BRIGHT;
        display.draw_pixel(s.x, s.y, sc);
        if (s.layer == 2 && s.y > 0) display.draw_pixel(s.x, static_cast<int16_t>(s.y - 1), BOOT_STAR_DIM);
    }

    display.draw_rect({4, 4, 72, 72}, BOOT_BORDER);
    if ((_frame / 16) % 2 == 0) {
        display.draw_pixel(4, 4, BOOT_STAR_BRIGHT);
        display.draw_pixel(75, 4, BOOT_STAR_BRIGHT);
        display.draw_pixel(4, 75, BOOT_STAR_BRIGHT);
        display.draw_pixel(75, 75, BOOT_STAR_BRIGHT);
    }

    Color title_color = ((_frame / 20) % 2 == 0) ? BOOT_TITLE_GLOW : BOOT_TITLE;
    TextRenderer::draw_text_centered(display, {cx, TITLE_Y}, TITLE, title_color, 1, BASIC_FONT_5X7);

    for (int16_t gx = 15; gx <= 65; ++gx) {
        auto phase = static_cast<uint8_t>((gx + _frame) % 10);
        if (phase < 4) display.draw_pixel(gx, GLOW_Y, BOOT_TITLE_GLOW);
        else if (phase < 6) display.draw_pixel(gx, GLOW_Y, BOOT_TITLE);
    }

    for (int16_t rx = 20; rx <= 34; ++rx) display.draw_pixel(rx, RULE_Y, BOOT_BORDER);
    for (int16_t rx = 46; rx <= 60; ++rx) display.draw_pixel(rx, RULE_Y, BOOT_BORDER);

    if ((_frame / 30) % 2 == 0)
        TextRenderer::draw_text_centered(display, {cx, HINT_Y}, HINT, BOOT_HINT, 1, COMPACT_FONT_3X5);
}

} // namespace handheld
