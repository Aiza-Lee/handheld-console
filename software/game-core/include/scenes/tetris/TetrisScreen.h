#ifndef SCENES_TETRIS_TETRIS_SCREEN_H
#define SCENES_TETRIS_TETRIS_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"

namespace handheld::tetris::cfg {

constexpr bool ENABLE_BGM = true;

} // namespace handheld::tetris::cfg

namespace handheld {

class TetrisScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);
};

} // namespace handheld

#endif
