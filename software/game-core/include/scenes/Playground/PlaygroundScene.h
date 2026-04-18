#ifndef SCENES_PLAYGROUND_PLAYGROUND_SCENE_H
#define SCENES_PLAYGROUND_PLAYGROUND_SCENE_H

#include "core/runtime/Scene.h"
#include <array>

namespace handheld {

// 开发者功能测试场景
class PlaygroundScene : public Scene {
public:
	void enter(IPlatform& platform, ISceneHost& host) override;
	void update(IPlatform& platform, ISceneHost& host) override;
	void render(IPlatform& platform, ISceneHost& host) override;

private:
	struct Star {
		int16_t x = 0;
		int16_t y = 0;
		uint8_t speed = 1;
		uint8_t layer = 0;
	};

	static constexpr uint8_t MODE_COUNT = 5;
	static constexpr size_t STAR_COUNT = 28;

	std::array<Star, STAR_COUNT> _stars{};
	bool _stars_ready = false;

	int16_t _x = 0;
	int16_t _y = 0;
	bool _auto_move = true;
	bool _clear_each_frame = true;
	uint8_t _mode = 0;
	uint32_t _frame = 0;
	uint8_t _accent_phase = 0;

	void init_stars(IDisplay& display);
	void update_starfield(IDisplay& display);
	void render_background(IDisplay& display) const;
	void render_mode_grid_tunnel(IDisplay& display) const;
	void render_mode_orbit_lab(IDisplay& display) const;
	void render_mode_starflow(IDisplay& display) const;
	void render_mode_signal_scope(IDisplay& display) const;
	void render_mode_aurora_bloom(IDisplay& display) const;
	Rect stage_rect(const IDisplay& display) const;
	Point stage_center(const IDisplay& display) const;
	void render_stage_shell(IDisplay& display) const;
	void render_hud(IDisplay& display) const;
	void draw_crosshair(IDisplay& display, Point center, int16_t radius, Color color) const;

	static uint32_t hash_u32(uint32_t x) {
		x ^= x >> 16;
		x *= 0x7feb352dU;
		x ^= x >> 15;
		x *= 0x846ca68bU;
		x ^= x >> 16;
		return x;
	}
};

} // namespace handheld

#endif // SCENES_PLAYGROUND_PLAYGROUND_SCENE_H