#ifndef CORE_RUNTIME_ISCENE_HOST_H
#define CORE_RUNTIME_ISCENE_HOST_H

#include "core/runtime/SceneType.h"

namespace handheld {

class Scene;

// Scene 调度接口
class ISceneHost {
public:
	ISceneHost() = default;
	ISceneHost(const ISceneHost&) = delete;
	ISceneHost(ISceneHost&&) = delete;
	ISceneHost& operator=(const ISceneHost&) = delete;
	ISceneHost& operator=(ISceneHost&&) = delete;
	virtual ~ISceneHost() = default;

	// 请求在当前 tick 结束后切换到另一个场景
	virtual void switch_to(SceneType type) = 0;

	// 当前活动场景
	virtual Scene& current_scene() = 0;
};

} // namespace handheld

#endif
