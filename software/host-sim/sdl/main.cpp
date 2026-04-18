#include "sdl/SdlPlatform.h"
#include "core/runtime/SceneRunner.h"
#include "core/runtime/SceneFactory.h"
#include "core/runtime/SceneType.h"

#include <exception>
#include <iostream>

int main() {
	try {
		handheld::SdlPlatform platform;
		handheld::DefaultSceneFactory factory;
		handheld::SceneRunner runner(platform, factory, handheld::SceneType::MENU);

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
