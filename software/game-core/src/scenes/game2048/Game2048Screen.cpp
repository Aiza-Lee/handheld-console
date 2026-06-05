#include "scenes/game2048/Game2048Screen.h"

namespace handheld {

void Game2048Screen::enter(IPlatform&, IScreenHost&) {}
void Game2048Screen::update(IPlatform&, IScreenHost&) {}
void Game2048Screen::render(IPlatform&, IScreenHost&) {}
void Game2048Screen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    (void)display;
    (void)box;
    (void)frame;
}

} // namespace handheld
