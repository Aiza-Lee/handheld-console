#include "core/runtime/SceneFactory.h"
#include "core/runtime/Scene.h"
#include "scenes/menu/MenuScene.h"
#include "scenes/Playground/PlaygroundScene.h"
#include <stdexcept>

namespace handheld {

std::unique_ptr<Scene> DefaultSceneFactory::create(SceneType type) {
	switch (type) {
		case SceneType::MENU:
			return std::make_unique<MenuScene>();
		case SceneType::PLAYGROUND:
			return std::make_unique<PlaygroundScene>();
		default:
			throw std::runtime_error("Unknown scene type");
	}
}

}  // namespace handheld
