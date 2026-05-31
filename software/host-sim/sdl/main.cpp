#include "sdl/SdlPlatform.h"
#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/assets/AssetEntry.h"

#include <exception>
#include <iostream>

// 由 add_assets() CMake 函数生成
extern "C" const handheld::AssetEntry builtin_assets[];
extern "C" const uint32_t builtin_assets_count;

int main() {
	try {
		handheld::SdlPlatform platform;
		platform.init_assets(builtin_assets, builtin_assets_count);
		handheld::DefaultScreenFactory factory;
		constexpr uint32_t FRAME_TIME_MS = 67;
		handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::BOOT, FRAME_TIME_MS);

		while (platform.is_running()) {
			platform.process_events();
			if (!platform.is_running()) {
				break;
			}
			runner.tick();
			platform.delay_to_next_frame(runner.frame_time_ms());
		}

		return 0;
	} catch (const std::exception& error) {
		std::cerr << "host-sim-sdl failed: " << error.what() << '\n';
		return 1;
	}
}
