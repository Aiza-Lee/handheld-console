#ifndef CORE_RUNTIME_SCREEN_TYPE_H
#define CORE_RUNTIME_SCREEN_TYPE_H

#include <cstdint>
namespace handheld {

// 屏幕类型枚举，用于工厂创建
// 每个值对应一个具体的 GameScreen 子类
enum class ScreenType : uint8_t {
    BOOT,
    MENU,
    GUIDE,
    SETTINGS,
    PLAYGROUND,
    SNAKE,
    PACMAN,
    BREAKOUT,
    INVADERS,
    GROW_BALL,
    TETRIS,
    GAME_2048,
    PONG,
    MP3,
    DEVELOPER,
};

} // namespace handheld

#endif // CORE_RUNTIME_SCREEN_TYPE_H
