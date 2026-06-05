#include "scenes/tetris/TetrisScreen.h"

namespace handheld {

void TetrisScreen::enter(IPlatform&, IScreenHost&) {}
void TetrisScreen::update(IPlatform&, IScreenHost&) {}
void TetrisScreen::render(IPlatform&, IScreenHost&) {}
void TetrisScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    (void)display;
    (void)box;
    (void)frame;
}

} // namespace handheld
