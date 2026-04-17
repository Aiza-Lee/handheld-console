#include "app/App.h"
#include "core/graphics/Color.h"
#include "platform/null/NullPlatform.h"

#include <cassert>

namespace {

class TrackingGame final : public handheld::Game {
public:
	explicit TrackingGame(handheld::Game* next_game = nullptr) : _next_game(next_game) {}

	void enter(handheld::IPlatform&, handheld::IGameHost&) override {
		++enter_count;
	}

	void exit(handheld::IPlatform&, handheld::IGameHost&) override {
		++exit_count;
	}

	void update(handheld::IPlatform& platform, handheld::IGameHost& host) override {
		++update_count;
		assert(platform.input().is_down(handheld::Button::A));

		if (_next_game != nullptr && !_did_request_switch) {
			host.switch_to(*_next_game);
			_did_request_switch = true;
		}
	}

	void render(handheld::IPlatform& platform, handheld::IGameHost&) override {
		++render_count;
		platform.display().clear(handheld::Color::BLACK);
		platform.display().present();
	}

	int enter_count = 0;
	int exit_count = 0;
	int update_count = 0;
	int render_count = 0;

private:
	handheld::Game* _next_game = nullptr;
	bool _did_request_switch = false;
};

} // namespace

int main() {
	handheld::NullPlatform platform;
	TrackingGame gameplay;
	TrackingGame menu(&gameplay);
	handheld::App app(platform, menu);

	platform.null_input().set_button(handheld::Button::A, true);
	app.tick();

	assert(&app.current_game() == &gameplay);
	assert(menu.enter_count == 1);
	assert(menu.exit_count == 1);
	assert(menu.update_count == 1);
	assert(menu.render_count == 1);
	assert(gameplay.enter_count == 0);
	assert(gameplay.exit_count == 0);
	assert(gameplay.update_count == 0);
	assert(gameplay.render_count == 0);
	assert(platform.null_input().poll_count() == 1);
	assert(platform.null_display().clear_count() == 1);
	assert(platform.null_display().present_count() == 1);
	assert(platform.null_display().last_clear_color() == handheld::Color::BLACK);

	app.tick();

	assert(&app.current_game() == &gameplay);
	assert(menu.enter_count == 1);
	assert(menu.exit_count == 1);
	assert(menu.update_count == 1);
	assert(menu.render_count == 1);
	assert(gameplay.enter_count == 1);
	assert(gameplay.exit_count == 0);
	assert(gameplay.update_count == 1);
	assert(gameplay.render_count == 1);
	assert(platform.null_input().poll_count() == 2);
	assert(platform.null_display().clear_count() == 2);
	assert(platform.null_display().present_count() == 2);
	assert(platform.null_display().last_clear_color() == handheld::Color::BLACK);

	return 0;
}
