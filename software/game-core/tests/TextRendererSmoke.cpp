#include "core/graphics/TextRenderer.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
	handheld::FakePlatform platform;

	const handheld::Size title_size = handheld::TextRenderer::measure_text("MENU", 2);
	assert(title_size.width == 46);
	assert(title_size.height == 14);

	const handheld::Size multiline_size = handheld::TextRenderer::measure_text("UP\nDOWN");
	assert(multiline_size.width == 23);
	assert(multiline_size.height == 15);

	const handheld::Size compact_size = handheld::TextRenderer::measure_text(
		"MENU",
		1,
		handheld::COMPACT_FONT_3X5);
	assert(compact_size.width == 15);
	assert(compact_size.height == 5);

	handheld::TextRenderer::draw_text(
		platform.display(),
		{10, 10},
		"Ab!",
		handheld::Color::WHITE);
	handheld::TextRenderer::draw_text(
		platform.display(),
		{10, 20},
		"HUD",
		handheld::Color::WHITE,
		1,
		handheld::COMPACT_FONT_3X5);
	assert(platform.fake_display().draw_pixel_count() > 0);
	assert(platform.fake_display().present_count() == 0);

	return 0;
}
