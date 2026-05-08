#include "sdl/SdlPlatform.h"
#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"

#include <exception>
#include <iostream>

int main() {
	try {
		handheld::SdlPlatform platform;
		handheld::DefaultScreenFactory factory;
		handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::MENU);

		while (platform.is_running()) {
			platform.process_events();
			if (!platform.is_running()) {
				break;
			}
			runner.tick();
			platform.delay_to_next_frame();
		}

		return 0;
	} catch (const std::exception& error) {
		std::cerr << "host-sim-sdl failed: " << error.what() << '\n';
		return 1;
	}
}
