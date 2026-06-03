#ifndef SCENES_PLAYGROUND_PLAYGROUND_GUIDE_SCREEN_H
#define SCENES_PLAYGROUND_PLAYGROUND_GUIDE_SCREEN_H

#include "core/runtime/GameScreen.h"

namespace handheld {

// Playground 操作指南浮层 — push 到 Playground 上方，显示按键说明和退出选项
class PlaygroundGuideScreen : public GameScreen {
	static constexpr bool ENABLE_BGM = false;
public:
	void enter(IPlatform& platform, IScreenHost& host) override;
	void update(IPlatform& platform, IScreenHost& host) override;
	void render(IPlatform& platform, IScreenHost& host) override;
};

} // namespace handheld

#endif
