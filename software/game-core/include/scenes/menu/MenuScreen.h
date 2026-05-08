#ifndef SCREENS_MENU_MENU_SCREEN_H
#define SCREENS_MENU_MENU_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"

namespace handheld {

// 菜单屏幕
class MenuScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;

private:
	int16_t _scan_width = 0;

	static constexpr Point UP_KEY_POS = {40, 40};
	static constexpr Point DOWN_KEY_POS = {40, 88};
	static constexpr Point LEFT_KEY_POS = {20, 64};
	static constexpr Point RIGHT_KEY_POS = {60, 64};
	static constexpr Point START_KEY_POS = {100, 64};
	static constexpr Point SELECT_KEY_POS = {140, 64};
};

}  // namespace handheld

#endif // SCREENS_MENU_MENU_SCREEN_H
