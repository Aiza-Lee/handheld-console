#ifndef SCENES_GROWBALL_GROW_BALL_SCREEN_H
#define SCENES_GROWBALL_GROW_BALL_SCREEN_H

#include "core/runtime/GameScreen.h"
#include "scenes/growball/GrowBallWorld.h"
#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {

class GrowBallScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    static void draw_filled_circle(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color);
    static void draw_circle_outline(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color);
    void world_to_screen(float wx, float wy, int16_t& sx, int16_t& sy) const;

    GrowBallWorld _world;
    float _zoom = 1.0F;
    float _camera_x = 0;
    float _camera_y = 0;
    bool _paused = false;
    bool _victory = false;
    bool _game_over = false;
    uint32_t _thrust_timer = 0;
};

} // namespace handheld

#endif // SCENES_GROWBALL_GROW_BALL_SCREEN_H
