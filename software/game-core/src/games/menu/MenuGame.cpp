#include "games/menu/MenuGame.h"
#include "core/graphics/Color.h"

namespace handheld {

void MenuGame::update(IPlatform& /*platform*/, IGameHost& /*host*/) {}

void MenuGame::render(IPlatform& platform, IGameHost& /*host*/) {
	platform.display().clear(Color::BLACK);
	platform.display().present();
}

} // namespace handheld
