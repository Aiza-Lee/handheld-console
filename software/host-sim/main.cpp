#include "core/runtime/SceneRunner.h"
#include "core/runtime/SceneFactory.h"
#include "core/runtime/SceneType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <iostream>

int main() {
	handheld::FakePlatform platform;
	handheld::DefaultSceneFactory factory;
	handheld::SceneRunner runner(platform, factory, handheld::SceneType::MENU);

	runner.tick();

	const bool frame_ok =
		platform.fake_display().clear_count() == 1 &&
		platform.fake_display().present_count() == 1 &&
		platform.fake_display().last_clear_color() == handheld::Color::BLACK;

	if (!frame_ok) {
		std::cerr
			<< "host-sim smoke failed: clear_count=" << platform.fake_display().clear_count()
			<< " present_count=" << platform.fake_display().present_count()
			<< " last_clear_color=" << static_cast<int>(platform.fake_display().last_clear_color())
			<< '\n';
		return 1;
	}

	std::cout << "host-sim smoke passed" << '\n';
	return 0;
}
