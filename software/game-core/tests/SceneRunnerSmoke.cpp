#include "core/runtime/SceneRunner.h"
#include "core/runtime/SceneFactory.h"
#include "core/runtime/SceneType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <memory>
#include <vector>

namespace {

int menu_enter_count = 0;
int menu_update_count = 0;
int playground_enter_count = 0;
int playground_update_count = 0;
std::vector<int> render_log;

enum RenderEvent {
	MENU_RENDER = 1,
	PLAYGROUND_RENDER = 2,
};

class MenuTrackingScene final : public handheld::Scene {
public:
	void enter(handheld::IPlatform& /*platform*/, handheld::ISceneHost& /*host*/) override {
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

	void render(handheld::IPlatform& platform, handheld::ISceneHost& /*host*/) override {
		render_log.push_back(MENU_RENDER);
		auto& display = platform.display();
		display.clear(handheld::Color::BLACK);
		display.draw_pixel(0, 0, handheld::Color::WHITE);
	}
};

class PlaygroundTrackingScene final : public handheld::Scene {
public:
	void enter(handheld::IPlatform& /*platform*/, handheld::ISceneHost& /*host*/) override {
		++playground_enter_count;
	}

	void update(handheld::IPlatform& platform, handheld::ISceneHost& /*host*/) override {
		++playground_update_count;
		assert(platform.input().is_down(handheld::ButtonBits::A));
	}

	void render(handheld::IPlatform& platform, handheld::ISceneHost& /*host*/) override {
		render_log.push_back(PLAYGROUND_RENDER);
		auto& display = platform.display();
		display.clear(handheld::Color::BLACK);
		display.draw_pixel(1, 0, handheld::Color::WHITE);
	}
};

// Test factory
class TestSceneFactory : public handheld::ISceneFactory {
public:
	std::unique_ptr<handheld::Scene> create(handheld::SceneType type) override {
		if (type == handheld::SceneType::MENU) {
			return std::make_unique<MenuTrackingScene>();
		}  			return std::make_unique<PlaygroundTrackingScene>();
	
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
	assert(render_log.size() == 1);
	assert(render_log[0] == MENU_RENDER);

	// Second tick: Menu exits, Playground enters, updates, renders
	runner.tick();
	assert(menu_enter_count == 1);      // Still 1
	assert(menu_update_count == 1);     // Still 1
	assert(playground_enter_count == 1); // Now entered
	assert(playground_update_count == 1); // Now updated
	assert(render_log.size() == 2);
	assert(render_log[1] == PLAYGROUND_RENDER);

	assert(platform.fake_display().present_count() == 2);

	return 0;
}
