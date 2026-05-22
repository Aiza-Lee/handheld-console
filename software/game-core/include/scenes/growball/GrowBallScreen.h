#ifndef SCENES_GROWBALL_GROW_BALL_SCREEN_H
#define SCENES_GROWBALL_GROW_BALL_SCREEN_H

#include "core/runtime/GameScreen.h"
#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {

class GrowBallScreen : public GameScreen {
public:
	struct Ball {
		float x, y;
		float vx, vy;
		float radius;
		Color color;
	};

	struct Food {
		float x, y;
		float radius;
		Color color;
	};

	struct AiBall {
		float x, y;
		float vx, vy;
		float radius;
		Color color;
		float wander_angle;
		uint32_t last_change_ms;
	};

	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	void spawn_food(int index);
	void spawn_ai(int index, uint32_t now_ms);
	static void draw_filled_circle(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color) ;
	static void draw_circle_outline(IDisplay& display, int16_t cx, int16_t cy, int16_t r, Color color) ;
	void world_to_screen(float wx, float wy, int16_t& sx, int16_t& sy) const;
	[[nodiscard]] float screen_to_world_size(float screen_size) const;
	static void clamp_to_world(Ball& ball) ;
	static float ball_speed_limit(float radius) ;

	Ball _player;
	Food _foods[20];
	AiBall _ai[4];
	float _zoom = 1.0F;
	float _camera_x = 0;
	float _camera_y = 0;
	bool _game_over = false;
	uint32_t _start_ms = 0;
};

}  // namespace handheld

#endif  // SCENES_GROWBALL_GROW_BALL_SCREEN_H
