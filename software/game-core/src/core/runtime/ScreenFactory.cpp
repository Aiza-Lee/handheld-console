#include "core/runtime/ScreenFactory.h"
#include "core/runtime/GameScreen.h"
#include "scenes/boot/BootScreen.h"
#include "scenes/menu/MenuScreen.h"
#include "scenes/playground/PlaygroundGuideScreen.h"
#include "scenes/playground/PlaygroundScreen.h"
#include "scenes/snake/SnakeScreen.h"
#include "scenes/pacman/PacmanScreen.h"
#include "scenes/breakout/BreakoutScreen.h"
#include "scenes/invaders/InvadersScreen.h"
#include "scenes/growball/GrowBallScreen.h"
#include "scenes/settings/SettingsScreen.h"
#include "scenes/tetris/TetrisScreen.h"
#include "scenes/game2048/Game2048Screen.h"
#include "scenes/pong/PongScreen.h"

namespace handheld {

std::unique_ptr<GameScreen> DefaultScreenFactory::create(ScreenType type) {
    switch (type) {
        case ScreenType::BOOT: return std::make_unique<BootScreen>();
        case ScreenType::MENU: return std::make_unique<MenuScreen>();
        case ScreenType::GUIDE: return std::make_unique<PlaygroundGuideScreen>();
        case ScreenType::SETTINGS: return std::make_unique<SettingsScreen>();
        case ScreenType::PLAYGROUND: return std::make_unique<PlaygroundScreen>();
        case ScreenType::SNAKE: return std::make_unique<SnakeScreen>();
        case ScreenType::PACMAN: return std::make_unique<PacmanScreen>();
        case ScreenType::BREAKOUT: return std::make_unique<BreakoutScreen>();
        case ScreenType::INVADERS: return std::make_unique<InvadersScreen>();
        case ScreenType::GROW_BALL: return std::make_unique<GrowBallScreen>();
        case ScreenType::TETRIS: return std::make_unique<TetrisScreen>();
        case ScreenType::GAME_2048: return std::make_unique<Game2048Screen>();
        case ScreenType::PONG: return std::make_unique<PongScreen>();
        default: return nullptr;
    }
}

} // namespace handheld
