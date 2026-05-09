#include "scenes/boot/BootScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

namespace {
} // namespace

void BootScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	platform.display().clear(Color::BLACK);
}

void BootScreen::update(IPlatform& platform, IScreenHost& host) {
	const auto& input = platform.input();
	for (auto btn : {ButtonBits::UP, ButtonBits::DOWN, ButtonBits::LEFT, ButtonBits::RIGHT,
	                 ButtonBits::A, ButtonBits::B, ButtonBits::START, ButtonBits::SELECT}) {
		if (input.was_pressed(btn)) {
			host.switch_to(ScreenType::MENU);
			return;
		}
	}	
}

void BootScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(Color::BLACK);

	const auto cx = static_cast<int16_t>(display.width() / 2);

	const char* title = "JLL Gamepad";
	TextRenderer::draw_text_centered(display, {cx, 28}, title, Color::WHITE, 1, BASIC_FONT_5X7);
	
	const char* hint = "Press any key";
	TextRenderer::draw_text_centered(display, {cx, 50}, hint, Color::GRAY, 1, COMPACT_FONT_3X5);

}

} // namespace handheld
