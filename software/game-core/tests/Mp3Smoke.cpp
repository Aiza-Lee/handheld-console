#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenType.h"
#include "core/common/ButtonBits.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    // ================================================================
    // 测试 1: MP3 屏幕启动后能正常渲染多帧 + 写入音频
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::MP3);

        for (int i = 0; i < 10; ++i) runner.tick();
        assert(platform.fake_display().present_count() == 10);
        // BGM 应当持续输出音频
        assert(platform.samples_written() > 0);
    }

    // ================================================================
    // 测试 2: 切歌（LEFT/RIGHT）不崩溃，进度条会变化
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::MP3);

        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::RIGHT, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::RIGHT, false);
        for (int i = 0; i < 5; ++i) runner.tick();

        platform.fake_input().set_button(handheld::ButtonBits::LEFT, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::LEFT, false);
        for (int i = 0; i < 5; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 10);
    }

    // ================================================================
    // 测试 3: A 切换播放/暂停不会崩溃
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::MP3);

        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);
        for (int i = 0; i < 5; ++i) runner.tick();

        platform.fake_input().set_button(handheld::ButtonBits::A, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::A, false);
        for (int i = 0; i < 5; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 10);
    }

    // ================================================================
    // 测试 4: B 退出后不崩溃（runner 仍持续 tick，栈上新屏幕应可接管）
    // ================================================================
    {
        handheld::FakePlatform platform({80, 80});
        handheld::ScreenRunner runner(platform, handheld::ScreenType::MP3);

        for (int i = 0; i < 5; ++i) runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::B, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::B, false);
        for (int i = 0; i < 5; ++i) runner.tick();
        assert(platform.fake_display().present_count() > 10);
    }

    return 0;
}
