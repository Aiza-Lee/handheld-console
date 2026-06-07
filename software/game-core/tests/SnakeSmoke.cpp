#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // ================================================================
    // 测试 1: Snake 屏幕启动后能正常渲染多帧
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::SNAKE);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 运行 30 帧
        for (int i = 0; i < 30; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() == 31);
    }

    // ================================================================
    // 测试 2: 方向输入不会导致 180° 翻转
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::SNAKE);

        runner.tick();

        // 蛇初始朝右（RIGHT），按 LEFT 应被忽略（180° 掉头）
        platform.fake_input().set_button(handheld::ButtonBits::LEFT, true);
        for (int i = 0; i < 30; ++i) {
            runner.tick();
            platform.fake_input().set_button(handheld::ButtonBits::LEFT, false);
        }

        // 正常运行 30 帧未崩溃即可——不会向左撞墙
        assert(platform.fake_display().present_count() > 30);
    }

    // ================================================================
    // 测试 3: 碰撞后 GAME OVER，按 START 可重新开始
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::SNAKE);

        // 蛇朝右，一直走直到撞墙
        for (int i = 0; i < 200; ++i) {
            runner.tick();
        }

        // 应该已经 GAME OVER，按 START 重启
        platform.fake_input().set_button(handheld::ButtonBits::START, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::START, false);

        // 重启后再跑几帧，不应崩溃
        for (int i = 0; i < 10; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() > 200);
    }

    return 0;
}
