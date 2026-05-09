#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "tests/support/FakePlatform.h"

#include <iostream>

int main() {
	handheld::FakePlatform platform;
	handheld::DefaultScreenFactory factory;
	handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::BOOT);

	runner.tick();

	const bool frame_ok = platform.fake_display().present_count() == 1;

	if (!frame_ok) {
		std::cerr
			<< "host-sim smoke failed: present_count=" << platform.fake_display().present_count()
			<< '\n';
		return 1;
	}

	std::cout << "host-sim smoke passed" << '\n';
	return 0;
}
