#include "scenes/playground/PlaygroundGuideScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

void PlaygroundGuideScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	platform.display().clear(Color::BLACK);
}

void PlaygroundGuideScreen::update(IPlatform& platform, IScreenHost& host) {
	const auto& input = platform.input();

	if (input.was_pressed(ButtonBits::SELECT) || input.was_pressed(ButtonBits::B)) {
		host.pop_screen();
		return;
	}

	if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
		host.switch_to(ScreenType::MENU);
	}
}

void PlaygroundGuideScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(Color::BLACK);

	const auto cx = static_cast<int16_t>(display.width() / 2);

	// 标题
	TextRenderer::draw_text_centered(display, {cx, 8}, "GUIDE", Color::WHITE, 1, BASIC_FONT_5X7);

	// 按键说明
	constexpr int16_t COL1 = 6;
	constexpr int16_t COL2 = 38;
	constexpr int16_t LINE_H = 7;
	constexpr int16_t START_Y = 20;

	struct Row { const char* key; const char* action; };
	constexpr Row ROWS[] = {
		{"D-PAD",  "Move"},
		{"A",      "Auto Move"},
		{"B",      "Trace"},
		{"START",  "Mode"},
		{"SELECT", "Guide"},
	};

	for (size_t i = 0; i < sizeof(ROWS) / sizeof(ROWS[0]); ++i) {
		const auto y = static_cast<int16_t>(START_Y + (i * LINE_H));
		TextRenderer::draw_text(display, {COL1, y}, ROWS[i].key, Color::GRAY, 1, COMPACT_FONT_3X5);
		TextRenderer::draw_text(display, {COL2, y}, ROWS[i].action, Color::WHITE, 1, COMPACT_FONT_3X5);
	}

	// 退出选项框
	constexpr int16_t BOX_X = 12;
	constexpr int16_t BOX_W = 56;
	constexpr int16_t BOX_H = 14;
	constexpr int16_t BOX_Y = 56;

	display.draw_rect(Rect{BOX_X, BOX_Y, BOX_W, BOX_H}, Color::YELLOW);
	TextRenderer::draw_text_centered(
		display,
		{static_cast<int16_t>(BOX_X + (BOX_W / 2)), static_cast<int16_t>(BOX_Y + (BOX_H / 2))},
		"EXIT MENU", Color::YELLOW, 1, COMPACT_FONT_3X5);

	// 底部提示
	TextRenderer::draw_text_centered(display, {cx, 73}, "SEL/B:Close", Color::GRAY, 1, COMPACT_FONT_3X5);
}

} // namespace handheld
