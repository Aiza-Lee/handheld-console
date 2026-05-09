#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>

int main() {
	// 测试 1: Breakout 启动和基本渲染
	{
		handheld::DefaultScreenFactory factory;
		handheld::FakePlatform platform({80, 80});
		handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::BREAKOUT);

		runner.tick();
		assert(platform.fake_display().present_count() == 1);

		for (int i = 0; i < 20; ++i) {
			runner.tick();
		}
		assert(platform.fake_display().present_count() == 21);
	}

	// 测试 2: 发射球并运行（模拟按 A + 拍子移动）
	{
		handheld::DefaultScreenFactory factory;
		handheld::FakePlatform platform({80, 80});
		handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::BREAKOUT);

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
		handheld::DefaultScreenFactory factory;
		handheld::FakePlatform platform({80, 80});
		handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::BREAKOUT);

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

	return 0;
}
