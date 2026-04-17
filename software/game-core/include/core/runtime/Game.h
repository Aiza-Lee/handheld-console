#ifndef CORE_RUNTIME_GAME_H
#define CORE_RUNTIME_GAME_H

#include "platform/interfaces/IPlatform.h"

namespace handheld {

class IGameHost;

// 场景基类
class Game {
public:
	Game() = default;
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;
	virtual ~Game() = default;

	// 进入场景时调用
	virtual void enter(IPlatform& platform, IGameHost& host) {}

	// 离开场景时调用
	virtual void exit(IPlatform& platform, IGameHost& host) {}

	// 更新场景状态
	virtual void update(IPlatform& platform, IGameHost& host) = 0;

	// 渲染当前帧
	virtual void render(IPlatform& platform, IGameHost& host) = 0;
};

} // namespace handheld

#endif
