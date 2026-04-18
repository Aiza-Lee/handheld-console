#include "core/runtime/SceneRunner.h"
#include "core/runtime/SceneFactory.h"
#include "core/runtime/SceneType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <memory>

namespace {

int menu_enter_count = 0;
int menu_update_count = 0;
int playground_enter_count = 0;
int playground_update_count = 0;

class MenuTrackingScene final : public handheld::Scene {
public:
	void enter(handheld::IPlatform&, handheld::ISceneHost&) override {
		++menu_enter_count;
	}

	void update(handheld::IPlatform& platform, handheld::ISceneHost& host) override {
		++menu_update_count;
		assert(platform.input().is_down(handheld::ButtonBits::A));
		// Request switch to Playground on first update
		if (menu_update_count == 1) {
			host.switch_to(handheld::SceneType::PLAYGROUND);
		}
	}

	void render(handheld::IPlatform& platform, handheld::ISceneHost&) override {
		platform.display().clear(handheld::Color::BLACK);
	}
};

class PlaygroundTrackingScene final : public handheld::Scene {
public:
	void enter(handheld::IPlatform&, handheld::ISceneHost&) override {
		++playground_enter_count;
	}

	void update(handheld::IPlatform& platform, handheld::ISceneHost&) override {
		++playground_update_count;
		assert(platform.input().is_down(handheld::ButtonBits::A));
	}

	void render(handheld::IPlatform& platform, handheld::ISceneHost&) override {
		platform.display().clear(handheld::Color::BLACK);
	}
};

// Test factory
class TestSceneFactory : public handheld::ISceneFactory {
public:
	std::unique_ptr<handheld::Scene> create(handheld::SceneType type) override {
		if (type == handheld::SceneType::MENU) {
			return std::make_unique<MenuTrackingScene>();
		} else {
			return std::make_unique<PlaygroundTrackingScene>();
		}
	}
};

} // namespace

int main() {
	handheld::FakePlatform platform;
	TestSceneFactory factory;
	handheld::SceneRunner runner(platform, factory, handheld::SceneType::MENU);

	platform.fake_input().set_button(handheld::ButtonBits::A, true);

	// First tick: Menu enters, updates (requests switch), render
	runner.tick();
	assert(menu_enter_count == 1);
	assert(menu_update_count == 1);
	assert(playground_enter_count == 0);  // Not entered yet, just created
	assert(playground_update_count == 0);

	// Second tick: Menu exits, Playground enters, updates, renders
	runner.tick();
	assert(menu_enter_count == 1);      // Still 1
	assert(menu_update_count == 1);     // Still 1
	assert(playground_enter_count == 1); // Now entered
	assert(playground_update_count == 1); // Now updated

	assert(platform.fake_display().last_clear_color() == handheld::Color::BLACK);

	return 0;
}
