#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "scenes/breakout/BreakoutScreen.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // 测试 1: Breakout 启动和基本渲染
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BREAKOUT);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        for (int i = 0; i < 20; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() == 21);
    }

    // 测试 2: 发射球并运行（模拟按 A + 拍子移动）
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BREAKOUT);

        runner.tick();

        // 按 A 发射球
        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);

        // 移动拍子 + 运行多帧
        platform.fake_input().set_button(handheld::ButtonBits::LEFT, true);
        for (int i = 0; i < 60; ++i) {
            runner.tick();
        }
        platform.fake_input().set_button(handheld::ButtonBits::LEFT, false);

        assert(platform.fake_display().present_count() > 60);
    }

    // 测试 3: 等待 GAME OVER 后重启
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BREAKOUT);

        runner.tick();

        // 发射球后不操作，等待球掉落
        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);

        for (int i = 0; i < 500; ++i) {
            runner.tick();
        }

        // 应该已经 GAME OVER，按 START 重启
        platform.fake_input().set_button(handheld::ButtonBits::START, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::START, false);

        for (int i = 0; i < 10; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() > 500);
    }

    // 测试 4: split_balls() 道具应"精准 double"，封顶 MAX_BALLS，不再自激
    //   旧实现有索引爆炸 bug：1 球→16 球（一次 split 把 MAX_BALLS 吃满）。
    //   新实现：连续 split 应该是 1→2→4→8→16→16。
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BREAKOUT);

        runner.tick();

        // 发射一个球（launch_ball 置 _ball_count=1）
        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);

        auto* breakout = static_cast<handheld::BreakoutScreen*>(runner.top_screen());
        assert(breakout != nullptr);
        assert(breakout->ball_count() == 1);

        // 第 1 次 split：1 → 2
        breakout->split_balls();
        assert(breakout->ball_count() == 2);

        // 第 2 次：2 → 4
        breakout->split_balls();
        assert(breakout->ball_count() == 4);

        // 第 3 次：4 → 8
        breakout->split_balls();
        assert(breakout->ball_count() == 8);

        // 第 4 次：8 → 16（封顶，不再翻倍）
        breakout->split_balls();
        assert(breakout->ball_count() == 16);

        // 第 5 次：16 → 16（满状态调用必须幂等，不能越界）
        breakout->split_balls();
        assert(breakout->ball_count() == 16);
    }

    return 0;
}
