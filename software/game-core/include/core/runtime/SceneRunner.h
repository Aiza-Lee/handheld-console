#ifndef CORE_RUNTIME_SCENE_RUNNER_H
#define CORE_RUNTIME_SCENE_RUNNER_H

#include "core/runtime/ISceneHost.h"
#include "core/runtime/Scene.h"
#include "core/runtime/SceneType.h"
#include <memory>

namespace handheld {

class ISceneFactory;

// 固定执行场景生命周期、输入采样和单帧提交流程。
class SceneRunner : public ISceneHost {
public:
	SceneRunner(IPlatform& platform, ISceneFactory& factory, SceneType initial_scene);
	SceneRunner(const SceneRunner&) = delete;
	SceneRunner(SceneRunner&&) = delete;
	SceneRunner& operator=(const SceneRunner&) = delete;
	SceneRunner& operator=(SceneRunner&&) = delete;
	~SceneRunner() = default;

	void tick();
	void run_forever();
	void switch_to(SceneType type) override;
	Scene& current_scene() override;

private:
	void _apply_pending_switch();

	IPlatform& _platform;
	ISceneFactory& _factory;
	std::unique_ptr<Scene> _current_scene;
	SceneType _pending_type;
	bool _has_pending_switch = false;
	bool _entered = false;
};

} // namespace handheld

#endif

