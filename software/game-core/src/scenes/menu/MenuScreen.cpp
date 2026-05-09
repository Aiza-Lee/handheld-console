#include "scenes/menu/MenuScreen.h"

#include <algorithm>
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"

namespace handheld {

namespace {

void draw_scroll_arrow(IDisplay& display, int16_t x, int16_t y, bool up) {
	if (up) {
		display.draw_pixel(x, static_cast<int16_t>(y + 1), Color::GRAY);
		display.draw_pixel(static_cast<int16_t>(x + 1), y, Color::GRAY);
		display.draw_pixel(static_cast<int16_t>(x + 2), static_cast<int16_t>(y + 1), Color::GRAY);
	} else {
		display.draw_pixel(x, y, Color::GRAY);
		display.draw_pixel(static_cast<int16_t>(x + 1), static_cast<int16_t>(y + 1), Color::GRAY);
		display.draw_pixel(static_cast<int16_t>(x + 2), y, Color::GRAY);
	}
}

} // namespace

void MenuScreen::enter(IPlatform& platform, IScreenHost& /*host*/) {
	platform.display().clear(Color::BLACK);
}

void MenuScreen::update(IPlatform& platform, IScreenHost& host) {
	const auto& input = platform.input();

	if (input.was_pressed(ButtonBits::UP)) {
		if (_cursor == 0) {
			_cursor = _entry_count - 1;
		} else {
			--_cursor;
		}
	}
	if (input.was_pressed(ButtonBits::DOWN)) {
		_cursor = (_cursor + 1 >= _entry_count) ? 0 : _cursor + 1;
	}
	if (input.was_pressed(ButtonBits::A) || input.was_pressed(ButtonBits::START)) {
		host.switch_to(_entries[_cursor].screen_type);
	}

	// 滚动跟随光标
	_scroll_offset = std::min(_cursor, _scroll_offset);
	if (_cursor >= _scroll_offset + MAX_VISIBLE) {
		_scroll_offset = _cursor - MAX_VISIBLE + 1;
	}
}

void MenuScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(Color::BLACK);

	const auto cx = static_cast<int16_t>(display.width() / 2);

	const char* title = "GAMES";
	TextRenderer::draw_text_centered(display, {cx, 6}, title, Color::WHITE, 1, BASIC_FONT_5X7);

	const size_t end = _scroll_offset + MAX_VISIBLE;
	const size_t render_end = (end > _entry_count) ? _entry_count : end;

	for (size_t i = _scroll_offset; i < render_end; ++i) {
		const auto by = static_cast<int16_t>(BOX_START_Y + ((i - _scroll_offset) * (BOX_H + BOX_GAP)));
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

	// 滚动指示器
	if (_scroll_offset > 0) {
		draw_scroll_arrow(display, static_cast<int16_t>(cx - 1), 12, true);
	}
	if (render_end < _entry_count) {
		draw_scroll_arrow(display, static_cast<int16_t>(cx - 1), static_cast<int16_t>(BOX_START_Y + (MAX_VISIBLE * (BOX_H + BOX_GAP)) - BOX_GAP), false);
	}

	const char* hint = "A/START: select";
	TextRenderer::draw_text_centered(display, {cx, 72}, hint, Color::GRAY, 1, COMPACT_FONT_3X5);
}

} // namespace handheld
