#include "games/menu/MenuGame.h"
#include "core/graphics/Color.h"

namespace handheld {

void MenuGame::update(IPlatform&) {}

void MenuGame::render(IPlatform& platform) {
	platform.display().clear(Color::BLACK);
	platform.display().present();
}

} // namespace handheld
