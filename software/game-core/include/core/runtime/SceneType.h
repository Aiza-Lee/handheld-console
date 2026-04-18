#ifndef CORE_RUNTIME_SCENE_TYPE_H
#define CORE_RUNTIME_SCENE_TYPE_H

#include <cstdint>
namespace handheld {

// 场景类型枚举，用于工厂创建
enum class SceneType : uint8_t {
	MENU,
	PLAYGROUND,
};

}  // namespace handheld

#endif  // CORE_RUNTIME_SCENE_TYPE_H
