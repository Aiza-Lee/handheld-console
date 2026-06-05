#ifndef SCENES_PONG_PONG_SCREEN_H
#define SCENES_PONG_PONG_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"

namespace handheld::pong::cfg {

constexpr bool ENABLE_BGM = true;

} // namespace handheld::pong::cfg

namespace handheld {

class PongScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);
};

} // namespace handheld

#endif
