#ifndef SCENES_BOOT_BOOT_SCREEN_H
#define SCENES_BOOT_BOOT_SCREEN_H

#include "core/runtime/GameScreen.h"

namespace handheld {

// 启动画面 — 显示 "JLL Gamepad" 标题，按任意键进入菜单
class BootScreen : public GameScreen {
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;
};

} // namespace handheld

#endif
