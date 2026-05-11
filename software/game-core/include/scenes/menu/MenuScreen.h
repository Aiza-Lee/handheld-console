#ifndef SCENES_MENU_MENU_SCREEN_H
#define SCENES_MENU_MENU_SCREEN_H

#include "core/runtime/GameScreen.h"
#include "scenes/menu/MenuConfig.h"
#include <cstddef>

namespace handheld {

// 菜单屏幕 — 展示已注册游戏列表，光标选择后进入
class MenuScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	size_t _cursor = 0;
	size_t _scroll_offset = 0;

	// 背景星星动画
	struct Star {
		int16_t x, y;
		uint8_t speed;
		uint8_t layer;
	};
	Star _stars[menu::cfg::STAR_COUNT];
	uint32_t _frame;
	bool _stars_ready;
	uint32_t _rng_state;

	void init_stars();
	void update_stars();
	uint32_t next_rng();
};

} // namespace handheld

#endif
