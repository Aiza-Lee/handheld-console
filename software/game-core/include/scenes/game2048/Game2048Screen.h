#ifndef SCENES_GAME2048_GAME_2048_SCREEN_H
#define SCENES_GAME2048_GAME_2048_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"

namespace handheld::game2048::cfg {

constexpr bool ENABLE_BGM = true;

} // namespace handheld::game2048::cfg

namespace handheld {

class Game2048Screen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);
};

} // namespace handheld

#endif
