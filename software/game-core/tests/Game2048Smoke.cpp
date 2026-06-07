#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "core/common/ButtonBits.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // ================================================================
    // 测试 1: 2048 屏幕启动后能正常渲染多帧
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::GAME_2048);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 运行 30 帧不崩溃
        for (int i = 0; i < 30; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() == 31);
    }

    // ================================================================
    // 测试 2: 方向输入不会导致崩溃
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::GAME_2048);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 模拟几轮方向键操作
        for (int i = 0; i < 50; ++i) {
            switch (i % 4) {
                case 0: platform.fake_input().set_button(handheld::ButtonBits::UP, true); break;
                case 1: platform.fake_input().set_button(handheld::ButtonBits::DOWN, true); break;
                case 2: platform.fake_input().set_button(handheld::ButtonBits::LEFT, true); break;
                case 3: platform.fake_input().set_button(handheld::ButtonBits::RIGHT, true); break;
                default: break;
            }
            runner.tick();
            // 释放按键，便于下次边缘检测
            platform.fake_input().set_button(handheld::ButtonBits::UP, false);
            platform.fake_input().set_button(handheld::ButtonBits::DOWN, false);
            platform.fake_input().set_button(handheld::ButtonBits::LEFT, false);
            platform.fake_input().set_button(handheld::ButtonBits::RIGHT, false);
        }

        // 持续渲染
        assert(platform.fake_display().present_count() > 30);
    }

    // ================================================================
    // 测试 3: 大量操作后屏幕仍能稳定运行
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::GAME_2048);

        runner.tick();

        // 多轮方向 + 暂停 / 恢复
        for (int i = 0; i < 100; ++i) {
            switch (i % 8) {
                case 0: platform.fake_input().set_button(handheld::ButtonBits::UP, true); break;
                case 1: platform.fake_input().set_button(handheld::ButtonBits::LEFT, true); break;
                case 2: platform.fake_input().set_button(handheld::ButtonBits::DOWN, true); break;
                case 3: platform.fake_input().set_button(handheld::ButtonBits::RIGHT, true); break;
                case 4:
                    platform.fake_input().set_button(handheld::ButtonBits::START, true);
                    runner.tick();
                    platform.fake_input().set_button(handheld::ButtonBits::START, false);
                    break;
                case 5:
                    platform.fake_input().set_button(handheld::ButtonBits::A, true);
                    runner.tick();
                    platform.fake_input().set_button(handheld::ButtonBits::A, false);
                    break;
                default: break;
            }
            runner.tick();
            platform.fake_input().set_button(handheld::ButtonBits::UP, false);
            platform.fake_input().set_button(handheld::ButtonBits::DOWN, false);
            platform.fake_input().set_button(handheld::ButtonBits::LEFT, false);
            platform.fake_input().set_button(handheld::ButtonBits::RIGHT, false);
        }

        assert(platform.fake_display().present_count() > 50);
    }

    return 0;
}
