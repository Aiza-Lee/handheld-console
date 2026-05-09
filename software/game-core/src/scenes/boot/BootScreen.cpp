#include "scenes/boot/BootScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"
#include "platform/interfaces/IAudio.h"

// 开机音乐 — 由 add_sounds() 在平台 CMakeLists 中生成。
// 使用 weak 链接：如果平台未注册该符号则静默跳过。
extern "C" [[gnu::weak]] const handheld::Tone _sound_BOOT[];
extern "C" [[gnu::weak]] const uint32_t _sound_BOOT_count;

namespace handheld {

namespace {
} // namespace

void BootScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	platform.display().clear(Color::BLACK);
	if (_sound_BOOT) {
		platform.audio().play_sequence(_sound_BOOT, _sound_BOOT_count, false);
	}
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
