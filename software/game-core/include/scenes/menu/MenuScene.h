#ifndef SCENES_MENU_MENU_SCENE_H
#define SCENES_MENU_MENU_SCENE_H

#include "core/graphics/Geometry.h"
#include "core/runtime/Scene.h"

namespace handheld {

// 菜单场景
class MenuScene : public Scene {
public:
	void enter(IPlatform& platform, ISceneHost& host) override;
	void update(IPlatform& platform, ISceneHost& host) override;
	void render(IPlatform& platform, ISceneHost& host) override;

private:
	int16_t _scan_width = 0;

	static constexpr Point UP_KEY_POS = {40, 40};
	static constexpr Point DOWN_KEY_POS = {40, 88};
	static constexpr Point LEFT_KEY_POS = {20, 64};
	static constexpr Point RIGHT_KEY_POS = {60, 64};
	static constexpr Point START_KEY_POS = {100, 64};
	static constexpr Point SELECT_KEY_POS = {140, 64};
};

}  // namespace handheld

#endif // SCENES_MENU_MENU_SCENE_H
