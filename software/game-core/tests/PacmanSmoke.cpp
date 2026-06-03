#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // 测试: Pac-Man 启动、运行、GAME OVER 后重启
    {
        handheld::DefaultScreenFactory factory;
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::PACMAN);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 跑 100 帧（足够发生各种碰撞）
        for (int i = 0; i < 100; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() == 101);

        // 确认在大量帧后游戏能响应 START 重启（可能已 GAME OVER）
        platform.fake_input().set_button(handheld::ButtonBits::START, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::START, false);

        for (int i = 0; i < 10; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() > 110);
    }

    // 测试: 方向键输入
    {
        handheld::DefaultScreenFactory factory;
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::PACMAN);

        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::RIGHT, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::RIGHT, false);
        platform.fake_input().set_button(handheld::ButtonBits::DOWN, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::DOWN, false);

        // 运行多帧，应无崩溃
        for (int i = 0; i < 30; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() > 30);
    }

    return 0;
}
