#ifndef SCENES_GROWBALL_GROW_BALL_WORLD_H
#define SCENES_GROWBALL_GROW_BALL_WORLD_H

#include "scenes/growball/GrowBallConfig.h"
#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {

class GrowBallWorld {
public:
	struct Ball {
		float x, y, vx, vy, radius;
		Color color;
	};
	struct Food {
		float x, y, radius;
		Color color;
	};
	struct AiBall {
		float x, y, vx, vy, radius;
		Color color;
		float wander_angle;
		uint32_t last_change_ms;
	};

	void init(uint32_t now_ms);

	Ball&       player()       { return _player; }
	const Ball& player() const { return _player; }
	Food*       foods()        { return _foods; }
	const Food* foods() const  { return _foods; }
	AiBall*     ai()           { return _ai; }
	const AiBall* ai() const   { return _ai; }

	// 执行一帧模拟。victory/game_over 为输出标志。
	void simulate(float input_vx, float input_vy, uint32_t now_ms, bool& victory, bool& game_over);

	static float speed_limit(float radius);
	static void clamp_to_world(Ball& ball);

private:
	void spawn_food(int index);
	void spawn_ai(int index, uint32_t now_ms);
	void update_ai(uint32_t now_ms);
	void resolve_collisions(uint32_t now_ms, bool& victory, bool& game_over);

	Ball _player;
	Food _foods[growball::cfg::FOOD_COUNT];
	AiBall _ai[growball::cfg::AI_COUNT];
};

}  // namespace handheld

#endif  // SCENES_GROWBALL_GROW_BALL_WORLD_H
