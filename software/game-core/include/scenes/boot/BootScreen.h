#ifndef SCENES_BOOT_BOOT_SCREEN_H
#define SCENES_BOOT_BOOT_SCREEN_H

#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 启动画面 — 显示 "JLL Gamepad" 标题，按任意键进入菜单
class BootScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	struct Star {
		int16_t x, y;
		uint8_t speed;
		uint8_t layer;
	};
	static constexpr int16_t STAR_COUNT = 16;
	Star _stars[STAR_COUNT];
	uint32_t _frame;
	bool _stars_ready;
	uint32_t _rng_state;

	void init_stars();
	void update_stars();
	uint32_t next_rng();
};

} // namespace handheld

#endif
