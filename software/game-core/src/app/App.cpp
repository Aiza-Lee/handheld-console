#include "app/App.h"

namespace handheld {

App::App(IPlatform& platform, Game& game) : _platform(platform), _current_game(&game) {}

void App::switch_to(Game& game) {
	_pending_game = &game;
}

Game& App::current_game() {
	return *_current_game;
}

void App::apply_pending_switch() {
	if (_pending_game == nullptr) {
		return;
	}

	if (_pending_game == _current_game) {
		_pending_game = nullptr;
		return;
	}

	if (_entered) {
		_current_game->exit(_platform, *this);
	}

	_current_game = _pending_game;
	_pending_game = nullptr;
	_entered = false;
}

void App::tick() {
	apply_pending_switch();

	if (!_entered) {
		_current_game->enter(_platform, *this);
		_entered = true;
	}

	_platform.input().poll();
	_current_game->update(_platform, *this);
	_current_game->render(_platform, *this);

	apply_pending_switch();
}

void App::run_forever() {
	for (;;) {
		tick();
	}
}

} // namespace handheld
