#ifndef SCENES_MENU_MENU_SCREEN_H
#define SCENES_MENU_MENU_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include "core/runtime/ScreenType.h"
#include <cstddef>

namespace handheld {

// 菜单项 — 游戏名称与对应屏幕类型
struct MenuEntry {
	const char* name;
	ScreenType screen_type;
};

// 菜单屏幕 — 展示已注册游戏列表，光标选择后进入
class MenuScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	static constexpr MenuEntry _entries[] = {
		{"Playground", ScreenType::PLAYGROUND},
	};
	static constexpr size_t _entry_count = 1;
	static constexpr int16_t BOX_X = 5;
	static constexpr int16_t BOX_W = 70;
	static constexpr int16_t BOX_H = 16;
	static constexpr int16_t BOX_START_Y = 24;
	static constexpr int16_t BOX_GAP = 4;

	size_t _cursor = 0;
};

} // namespace handheld

#endif
