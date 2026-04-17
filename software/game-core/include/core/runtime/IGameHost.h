#ifndef CORE_RUNTIME_IGAME_HOST_H
#define CORE_RUNTIME_IGAME_HOST_H

namespace handheld {

class Game;

// Game 调度接口
class IGameHost {
public:
	IGameHost() = default;
	IGameHost(const IGameHost&) = delete;
	IGameHost(IGameHost&&) = delete;
	IGameHost& operator=(const IGameHost&) = delete;
	IGameHost& operator=(IGameHost&&) = delete;
	virtual ~IGameHost() = default;

	// 请求在当前帧结束后切换到另一个场景
	virtual void switch_to(Game& game) = 0;

	// 当前活动场景
	virtual Game& current_game() = 0;
};

} // namespace handheld

#endif
