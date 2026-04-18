#include "scenes/menu/MenuScene.h"
#include "core/graphics/Color.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/ISceneHost.h"
#include "core/runtime/SceneType.h"
#include "scenes/Playground/PlaygroundScene.h"
#include <memory>

namespace handheld {

namespace {

Point _center_text(Point center, const char* text, int16_t scale = 1) {
	const Size size = TextRenderer::measure_text(text, scale);
	return {
		static_cast<int16_t>(center.x - (size.width / 2)),
		static_cast<int16_t>(center.y - (size.height / 2)),
	};
}

} // namespace

void MenuScene::update(IPlatform& platform, ISceneHost& host) {
	if (_scan_width < platform.display().width()) {
		++_scan_width;
	}
	if (platform.input().was_pressed(ButtonBits::START)) {
		host.switch_to(SceneType::PLAYGROUND);
	}
}

void MenuScene::render(IPlatform& platform, ISceneHost& /*host*/) {
	IDisplay& display = platform.display();
	display.clear(Color::BLACK);

	const char* title = "HANDHELD";
	const Size title_size = TextRenderer::measure_text(title, 2);
	const Point title_origin = {
		static_cast<int16_t>((display.width() - title_size.width) / 2),
		12,
	};
	TextRenderer::draw_text(display, title_origin, title, Color::WHITE, 2);

	const auto line_y = static_cast<int16_t>(title_origin.y + title_size.height + 8);
	display.draw_h_line(0, line_y, _scan_width, Color::GRAY);

	TextRenderer::draw_text(display, _center_text(UP_KEY_POS, "UP"), "UP", Color::WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, _center_text(DOWN_KEY_POS, "DOWN"), "DOWN", Color::WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, _center_text(LEFT_KEY_POS, "LEFT"), "LEFT", Color::WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, _center_text(RIGHT_KEY_POS, "RIGHT"), "RIGHT", Color::WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, _center_text(START_KEY_POS, "START"), "START", Color::GREEN, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, _center_text(SELECT_KEY_POS, "SELECT"), "SELECT", Color::CYAN, 1, COMPACT_FONT_3X5);

	TextRenderer::draw_text(
		display,
		_center_text({static_cast<int16_t>(display.width() / 2), 108}, "PRESS START"),
		"PRESS START",
		Color::YELLOW,
		1,
		COMPACT_FONT_3X5);

	for (int16_t i = 0; i < _scan_width; ++i) {
		display.draw_pixel(i, static_cast<int16_t>(display.height() / 2), Color::WHITE);
	}
}

} // namespace handheld
