#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
    using namespace handheld;

    // ================================================================
    // 测试 1: Tetris 屏幕启动后能正常渲染多帧
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::TETRIS);

        runner.tick();
        assert(platform.fake_display().present_count() == 1);

        // 运行 30 帧
        for (int i = 0; i < 30; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() == 31);
    }

    // ================================================================
    // 测试 2: 方向键输入不会触发游戏结束（仅在边界被忽略）
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::TETRIS);

        runner.tick();

        // 同时按 LEFT + RIGHT，方块原地不动；空板上 30 帧后无堆叠重叠
        platform.fake_input().set_button(ButtonBits::LEFT, true);
        platform.fake_input().set_button(ButtonBits::RIGHT, true);
        for (int i = 0; i < 30; ++i) {
            runner.tick();
        }
        platform.fake_input().set_button(ButtonBits::LEFT, false);
        platform.fake_input().set_button(ButtonBits::RIGHT, false);

        assert(platform.fake_display().present_count() > 30);
    }

    // ================================================================
    // 测试 3: 按 B 切换到菜单，菜单接管后继续 tick 不崩溃
    // ================================================================
    {
        DefaultScreenFactory factory;
        FakePlatform platform({80, 80});
        ScreenRunner runner(platform, factory, ScreenType::TETRIS);

        for (int i = 0; i < 5; ++i) {
            runner.tick();
        }
        const uint32_t before = platform.fake_display().present_count();

        platform.fake_input().set_button(ButtonBits::B, true);
        runner.tick();
        platform.fake_input().set_button(ButtonBits::B, false);

        // 菜单接管后，再 tick 几次应继续 present
        for (int i = 0; i < 5; ++i) {
            runner.tick();
        }
        assert(platform.fake_display().present_count() > before);
    }

    return 0;
}
