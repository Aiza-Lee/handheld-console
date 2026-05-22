#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/common/ButtonBits.h"
#include "tests/support/FakePlatform.h"
#include <cassert>

int main() {
    handheld::FakePlatform platform({80, 80});
    handheld::DefaultScreenFactory factory;
    handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::GROW_BALL);

    runner.tick();
    assert(platform.fake_display().present_count() == 1);

    platform.fake_input().set_button(handheld::ButtonBits::RIGHT, true);
    for (int i = 0; i < 10; ++i) runner.tick();
    assert(platform.fake_display().present_count() == 11);

    platform.fake_input().set_button(handheld::ButtonBits::RIGHT, false);
    platform.fake_input().set_button(handheld::ButtonBits::UP, true);
    for (int i = 0; i < 5; ++i) runner.tick();
    assert(platform.fake_display().present_count() == 16);

    return 0;
}
