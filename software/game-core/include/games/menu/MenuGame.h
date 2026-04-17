#ifndef GAMES_MENU_MENU_GAME_H
#define GAMES_MENU_MENU_GAME_H

#include "core/runtime/Game.h"

namespace handheld {

// 菜单场景
class MenuGame : public Game {
public:
	void update(IPlatform& platform, IGameHost& host) override;
	void render(IPlatform& platform, IGameHost& host) override;
};

}  // namespace handheld

#endif
