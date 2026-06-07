#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/common/ButtonBits.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

namespace {

// FakeTime 的 delay_ms 把时间向前推进；用 platform.time() 直接构造一个替代。
// 但 ITime 接口只暴露 const ticks_ms()。FakeTime 默认 ticks=0, 不变。
// 解决方案：每 tick 后调一次 delay_ms(100)，让 5 次 B 都在 1500ms 窗口内。

} // namespace

int main() {
    // ================================================================
    // 测试 1: 5 次 B 在 1.5s 窗口内连按 → 触发 dev 屏幕
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BOOT);

        // boot 启动后音频 BGM 不会立即产生 samples（play_sfx 是单次）。
        // 模拟 5 次 B 快速按：每 100ms 一次
        for (int i = 0; i < 5; ++i) {
            platform.fake_input().set_button(handheld::ButtonBits::B, true);
            runner.tick();
            platform.time().delay_ms(100);
            platform.fake_input().set_button(handheld::ButtonBits::B, false);
            runner.tick();
            platform.time().delay_ms(100);
        }
        // 5 次 B 完成后再跑 ~50 帧
        for (int i = 0; i < 50; ++i) runner.tick();

        // 触发后切到 DEVELOPER 屏（音频停止），但渲染继续
        assert(platform.fake_display().present_count() > 50);
    }

    // ================================================================
    // 测试 2: B 按太慢（间隔 > 1.5s）不会触发 dev，留在 BOOT
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::BOOT);

        // 按 5 次但每次间隔 2s（> 1.5s 窗口），应保持 BOOT
        for (int i = 0; i < 5; ++i) {
            platform.fake_input().set_button(handheld::ButtonBits::B, true);
            runner.tick();
            platform.time().delay_ms(2000);
            platform.fake_input().set_button(handheld::ButtonBits::B, false);
            runner.tick();
            platform.time().delay_ms(2000);
        }
        // BOOT 屏持续渲染；单按 B 在窗口外不会触发计数
        assert(platform.fake_display().present_count() > 5);

        // 此时按 A 应该正常进 MENU
        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);
        for (int i = 0; i < 5; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 10);
    }

    return 0;
}
