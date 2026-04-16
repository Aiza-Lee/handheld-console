#ifndef CORE_RUNTIME_GAME_H
#define CORE_RUNTIME_GAME_H

#include "platform/interfaces/IPlatform.h"

namespace handheld {

// 场景基类
class Game {
public:
	virtual ~Game() = default;

	// 进入场景时调用
	virtual void enter(IPlatform& platform) {}

	// 更新场景状态
	virtual void update(IPlatform& platform) = 0;

	// 渲染当前帧
	virtual void render(IPlatform& platform) = 0;
};

} // namespace handheld

#endif
