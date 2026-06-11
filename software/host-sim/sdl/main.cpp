#include "sdl/SdlPlatform.h"
#include "core/random/Random.h"
#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"

#include <exception>
#include <iostream>

int main() {
    try {
        handheld::SdlPlatform platform;
        handheld::random::seed(platform.time().ticks_ms());
        constexpr uint32_t FRAME_TIME_MS = 50;
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BOOT, FRAME_TIME_MS);

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
