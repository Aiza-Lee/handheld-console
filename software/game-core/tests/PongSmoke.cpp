#include "core/common/ButtonBits.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <cstdint>

namespace {

// 简单的 80x80 平台烟测：运行帧数 + 验证 present() 增长
void run_frames(handheld::ScreenRunner& runner, int n) {
    for (int i = 0; i < n; ++i) runner.tick();
}

} // namespace

int main() {
    using namespace handheld;

    // ================================================================
    // 测试 1：进入 PONG 屏幕并运行 50 帧，应平稳渲染
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::PONG);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 50 帧覆盖：进入 SERVE 阶段 + 自动发球（30 帧） + 球在 PLAY 阶段运动
        run_frames(runner, 50);
        assert(platform.fake_display().present_count() == 51);
    }

    // ================================================================
    // 测试 2：P1 持续按 UP/DOWN，无崩溃
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::PONG);

        runner.tick();

        platform.fake_input().set_button(ButtonBits::UP, true);
        for (int i = 0; i < 20; ++i) {
            runner.tick();
        }
        platform.fake_input().set_button(ButtonBits::UP, false);

        platform.fake_input().set_button(ButtonBits::DOWN, true);
        for (int i = 0; i < 20; ++i) {
            runner.tick();
        }
        platform.fake_input().set_button(ButtonBits::DOWN, false);

        for (int i = 0; i < 10; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 50);
    }

    // ================================================================
    // 测试 3：按 SELECT 切换 1P ↔ 2P 模式，再多跑一些帧不崩溃
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::PONG);

        runner.tick();

        // 切到 2P
        platform.fake_input().set_button(ButtonBits::SELECT, true);
        runner.tick();
        platform.fake_input().set_button(ButtonBits::SELECT, false);

        // 在 2P 模式下按 A/B 步进 P2 挡板
        for (int i = 0; i < 5; ++i) {
            platform.fake_input().set_button(ButtonBits::A, true);
            runner.tick();
            platform.fake_input().set_button(ButtonBits::A, false);
            runner.tick();
        }
        for (int i = 0; i < 5; ++i) {
            platform.fake_input().set_button(ButtonBits::B, true);
            runner.tick();
            platform.fake_input().set_button(ButtonBits::B, false);
            runner.tick();
        }

        // 再切回 1P
        platform.fake_input().set_button(ButtonBits::SELECT, true);
        runner.tick();
        platform.fake_input().set_button(ButtonBits::SELECT, false);

        // 跑几帧
        run_frames(runner, 10);
        assert(platform.fake_display().present_count() > 25);
    }

    // ================================================================
    // 测试 4：球会反弹（左挡板撞击）
    //   - 用 D-pad UP 让挡板保持在球 y 范围，让球发球后多次撞左挡板
    //   - 验证 200 帧内不崩溃
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::PONG);

        runner.tick();

        // 长按 UP，让 P1 挡板到顶部（_left_paddle_y = PADDLE_Y_MIN = 10）
        platform.fake_input().set_button(ButtonBits::UP, true);
        for (int i = 0; i < 30; ++i) runner.tick();
        platform.fake_input().set_button(ButtonBits::UP, false);

        // 200 帧覆盖：SERVE（30） + PLAY（170）
        run_frames(runner, 200);
        assert(platform.fake_display().present_count() > 200);
    }

    return 0;
}
