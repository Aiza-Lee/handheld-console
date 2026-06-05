#include "scenes/pong/PongScreen.h"

namespace handheld {

void PongScreen::enter(IPlatform&, IScreenHost&) {}
void PongScreen::update(IPlatform&, IScreenHost&) {}
void PongScreen::render(IPlatform&, IScreenHost&) {}
void PongScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    (void)display;
    (void)box;
    (void)frame;
}

} // namespace handheld
