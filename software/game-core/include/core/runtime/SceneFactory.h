#ifndef CORE_RUNTIME_SCENE_FACTORY_H
#define CORE_RUNTIME_SCENE_FACTORY_H

#include "core/runtime/SceneType.h"
#include <memory>

namespace handheld {

class Scene;

// 场景工厂接口
class ISceneFactory {
public:
	ISceneFactory() = default;
	ISceneFactory(const ISceneFactory&) = delete;
	ISceneFactory(ISceneFactory&&) = delete;
	ISceneFactory& operator=(const ISceneFactory&) = delete;
	ISceneFactory& operator=(ISceneFactory&&) = delete;
	virtual ~ISceneFactory() = default;

	// 根据场景类型创建场景实例
	virtual std::unique_ptr<Scene> create(SceneType type) = 0;
};

// 默认场景工厂实现
class DefaultSceneFactory : public ISceneFactory {
public:
	DefaultSceneFactory() = default;
	~DefaultSceneFactory() override = default;

	std::unique_ptr<Scene> create(SceneType type) override;
};

}  // namespace handheld

#endif  // CORE_RUNTIME_SCENE_FACTORY_H
