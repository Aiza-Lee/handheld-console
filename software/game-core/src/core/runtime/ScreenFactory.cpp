#include "core/runtime/ScreenFactory.h"
#include "core/runtime/GameScreen.h"
#include "scenes/menu/MenuScreen.h"
#include "scenes/playground/PlaygroundScreen.h"
#include <stdexcept>

namespace handheld {

std::unique_ptr<GameScreen> DefaultScreenFactory::create(ScreenType type) {
	switch (type) {
		case ScreenType::MENU:
			return std::make_unique<MenuScreen>();
		case ScreenType::PLAYGROUND:
			return std::make_unique<PlaygroundScreen>();
		default:
			throw std::runtime_error("Unknown screen type");
	}
}

}  // namespace handheld
