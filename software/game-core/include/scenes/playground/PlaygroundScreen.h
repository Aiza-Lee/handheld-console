#ifndef SCREENS_PLAYGROUND_PLAYGROUND_SCREEN_H
#define SCREENS_PLAYGROUND_PLAYGROUND_SCREEN_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <array>
#include <cstdint>

namespace handheld {

struct IDisplay;

// Playground 屏幕 — 按键测试和视觉演示
class PlaygroundScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	static constexpr uint8_t MODE_COUNT = 5;

	struct Star {
		int16_t x = 0;
		int16_t y = 0;
		uint8_t speed = 0;
		uint8_t layer = 0;
	};

	[[nodiscard]] Rect stage_rect(const IDisplay& display) const;
	[[nodiscard]] Point stage_center(const IDisplay& display) const;

	void render_stage_shell(IDisplay& display) const;
	void init_stars(IDisplay& display);
	void update_starfield(IDisplay& display);
	void render_background(IDisplay& display) const;
	void render_mode_grid_tunnel(IDisplay& display) const;
	void render_mode_orbit_lab(IDisplay& display) const;
	void render_mode_starflow(IDisplay& display) const;
	void render_mode_signal_scope(IDisplay& display) const;
	void render_mode_aurora_bloom(IDisplay& display) const;
	void render_hud(IDisplay& display) const;

	void draw_crosshair(IDisplay& display, Point center, int16_t radius, Color color) const;
	static uint32_t hash_u32(uint32_t seed);

	int16_t _x = 0;
	int16_t _y = 0;
	uint8_t _mode = 0;
	uint8_t _accent_phase = 0;
	uint32_t _frame = 0;
	bool _auto_move = true;
	bool _clear_each_frame = false;
	bool _stars_ready = false;

	std::array<Star, 32> _stars{};
};

inline uint32_t PlaygroundScreen::hash_u32(uint32_t seed) {
	uint32_t h = seed;
	h ^= h >> 17U;
	h *= 0xed5ad4bbU;
	h ^= h >> 11U;
	h *= 0xac4c1b51U;
	h ^= h >> 15U;
	return h;
}

}  // namespace handheld

#endif // SCREENS_PLAYGROUND_PLAYGROUND_SCREEN_H
