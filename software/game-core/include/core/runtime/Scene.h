#ifndef CORE_RUNTIME_SCENE_H
#define CORE_RUNTIME_SCENE_H

#include "platform/interfaces/IPlatform.h"

namespace handheld {

class ISceneHost;

// 场景基类
class Scene {
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) = delete;
	virtual ~Scene() = default;

	// 进入场景时调用
	virtual void enter(IPlatform& platform, ISceneHost& host) {}

	// 离开场景时调用
	virtual void exit(IPlatform& platform, ISceneHost& host) {}

	// 更新场景状态
	virtual void update(IPlatform& platform, ISceneHost& host) = 0;

	// 渲染当前帧
	virtual void render(IPlatform& platform, ISceneHost& host) = 0;
};

} // namespace handheld

#endif
