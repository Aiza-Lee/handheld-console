#include "app/App.h"
#include "core/graphics/Color.h"
#include "games/menu/MenuGame.h"
#include "platform/null/NullPlatform.h"

#include <iostream>

int main() {
	handheld::NullPlatform platform;
	handheld::MenuGame menu;
	handheld::App app(platform, menu);

	app.tick();

	const bool frame_ok =
		platform.null_display().clear_count() == 1 &&
		platform.null_display().present_count() == 1 &&
		platform.null_display().last_clear_color() == handheld::Color::BLACK;

	if (!frame_ok) {
		std::cerr << "host-sim smoke failed" << '\n';
		return 1;
	}

	std::cout << "host-sim smoke passed" << '\n';
	return 0;
}
