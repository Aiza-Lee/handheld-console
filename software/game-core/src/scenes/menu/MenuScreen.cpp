#include "scenes/menu/MenuScreen.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"

namespace handheld {

namespace {
} // namespace

void MenuScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	platform.display().clear(Color::BLACK);
}

void MenuScreen::update(IPlatform& platform, IScreenHost& host) {
	const auto& input = platform.input();

	if (input.was_pressed(ButtonBits::UP)) {
		_cursor = (_cursor == 0) ? _entry_count - 1 : _cursor - 1;
	}
	if (input.was_pressed(ButtonBits::DOWN)) {
		_cursor = (_cursor + 1 >= _entry_count) ? 0 : _cursor + 1;
	}
	if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
		host.switch_to(_entries[_cursor].screen_type);
	}
}

void MenuScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(Color::BLACK);

	const auto cx = static_cast<int16_t>(display.width() / 2);

	const char* title = "GAMES";
	TextRenderer::draw_text_centered(display, {cx, 8}, title, Color::WHITE, 1, BASIC_FONT_5X7);

	for (size_t i = 0; i < _entry_count; ++i) {
		const auto by = static_cast<int16_t>(BOX_START_Y + (i * (BOX_H + BOX_GAP)));
		const Rect box = {BOX_X, by, BOX_W, BOX_H};
		const bool selected = (i == _cursor);

		const Color border_color = selected ? Color::YELLOW : Color::GRAY;
		const Color text_color = selected ? Color::YELLOW : Color::GRAY;

		display.draw_rect(box, border_color);

		TextRenderer::draw_text_centered(
			display, 
			{static_cast<int16_t>(box.x + (box.width / 2)), static_cast<int16_t>(box.y + (box.height / 2))}, 
			_entries[i].name, text_color, 1, COMPACT_FONT_3X5
		);
	}

	const char* hint = "A/START: select";
	TextRenderer::draw_text_centered(display, {cx, 72}, hint, Color::GRAY, 1, COMPACT_FONT_3X5);


}

} // namespace handheld
