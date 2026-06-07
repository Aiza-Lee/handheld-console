#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/common/ButtonBits.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // ================================================================
    // 测试 1: Developer 屏幕启动后能正常渲染多帧，滚动到末尾不崩溃
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::DEVELOPER);

        // 跑满滚动周期（~80 帧滚动完）+ 余量
        for (int i = 0; i < 200; ++i) runner.tick();
        assert(platform.fake_display().present_count() == 200);
    }

    // ================================================================
    // 测试 2: 任意时候按 B/A/START 都不崩溃
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::DEVELOPER);

        for (int i = 0; i < 10; ++i) runner.tick();

        platform.fake_input().set_button(handheld::ButtonBits::B, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::B, false);
        for (int i = 0; i < 5; ++i) runner.tick();

        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);
        for (int i = 0; i < 5; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 20);
    }

    return 0;
}
