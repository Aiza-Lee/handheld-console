#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <memory>

namespace {

using handheld::IPlatform;
using handheld::IScreenHost;
using handheld::ScreenType;

// ---- 测试屏幕 ----

class BootTestScreen final : public handheld::GameScreen {
public:
	void enter(IPlatform& /*p*/, IScreenHost& /*h*/) override {}
	void update(IPlatform& platform, IScreenHost& host) override {
		host.switch_to(ScreenType::MENU);
	}
	void render(IPlatform& platform, IScreenHost& /*h*/) override {
		platform.display().clear(handheld::Color::BLACK);
	}
};

class MenuTestScreen final : public handheld::GameScreen {
public:
	void enter(IPlatform& /*p*/, IScreenHost& /*h*/) override {}
	void update(IPlatform& platform, IScreenHost& host) override {
		host.switch_to(ScreenType::PLAYGROUND);
	}
	void render(IPlatform& platform, IScreenHost& /*h*/) override {
		platform.display().clear(handheld::Color::BLACK);
	}
};

class GameTestScreen final : public handheld::GameScreen {
public:
	void enter(IPlatform& /*p*/, IScreenHost& /*h*/) override {}
	void suspend(IPlatform& /*p*/, IScreenHost& /*h*/) override { suspended = true; }
	void resume(IPlatform& /*p*/, IScreenHost& /*h*/) override { resumed = true; }
	void update(IPlatform& platform, IScreenHost& host) override {
		updated = true;
		if (push_requested) {
			host.push_screen(ScreenType::MENU);
			push_requested = false;
		}
	}
	void render(IPlatform& platform, IScreenHost& /*h*/) override {
		rendered = true;
		platform.display().clear(handheld::Color::BLACK);
	}

	bool suspended = false;
	bool resumed = false;
	bool updated = false;
	bool rendered = false;
	bool push_requested = false;
};

class OverlayScreen final : public handheld::GameScreen {
public:
	explicit OverlayScreen(bool& exited_flag) : _exited_flag(&exited_flag) {}
	void enter(IPlatform& /*p*/, IScreenHost& /*h*/) override {}
	void exit(IPlatform& /*p*/, IScreenHost& /*h*/) override { *_exited_flag = true; }
	void update(IPlatform& platform, IScreenHost& host) override {
		host.pop_screen();
	}
	void render(IPlatform& platform, IScreenHost& /*h*/) override {
		platform.display().clear(handheld::Color::BLACK);
	}

private:
	bool* _exited_flag;
};

// ---- 工厂：序列调用决定了返回哪种屏幕 ----
class TestScreenFactory : public handheld::IScreenFactory {
public:
	enum Mode {
		SWITCH_TEST,       // BOOT → BootTestScreen → MENU → MenuTestScreen → PLAYGROUND → GameTestScreen
		PUSH_POP_TEST,     // PLAYGROUND → GameTestScreen → push → OverlayScreen → pop
	};

	explicit TestScreenFactory(Mode mode) : _mode(mode) {}

	std::unique_ptr<handheld::GameScreen> create(ScreenType type) override {
		_call_count++;
		switch (_mode) {
		case SWITCH_TEST:
			if (type == ScreenType::BOOT) return std::make_unique<BootTestScreen>();
			if (type == ScreenType::MENU) return std::make_unique<MenuTestScreen>();
			return std::make_unique<GameTestScreen>();
		case PUSH_POP_TEST:
			if (type == ScreenType::PLAYGROUND) {
				auto g = std::make_unique<GameTestScreen>();
				g->push_requested = true;
				_recent_game = g.get();
				return g;
			}
			return std::make_unique<OverlayScreen>(_overlay_exited);
		}
		return nullptr;
	}

	int _call_count = 0;
	GameTestScreen* _recent_game = nullptr;
	bool _overlay_exited = false;

private:
	Mode _mode;
};

} // namespace

int main() {
	// ================================================================
	// 测试 1: switch_to — BOOT → MENU → PLAYGROUND 完整链
	// ================================================================
	{
		handheld::FakePlatform platform({80, 80});
		TestScreenFactory factory(TestScreenFactory::SWITCH_TEST);
		handheld::ScreenRunner runner(platform, factory, ScreenType::BOOT);

		// tick 1: Boot enters, update → switch_to(MENU), render, present
		runner.tick();

		// tick 2: apply switch → Menu enters, update → switch_to(PLAYGROUND), render, present
		runner.tick();

		// tick 3: apply switch → Game enters, update, render, present
		runner.tick();

		assert(platform.fake_display().present_count() == 3);
	}

	// ================================================================
	// 测试 2: push_screen → pop_screen（栈行为）
	// ================================================================
	{
		handheld::FakePlatform platform({80, 80});
		TestScreenFactory factory(TestScreenFactory::PUSH_POP_TEST);
		handheld::ScreenRunner runner(platform, factory, ScreenType::PLAYGROUND);

		// tick 1: Game enters, update → push_screen
		runner.tick();
		assert(factory._recent_game->updated);
		assert(factory._recent_game->suspended);    // push 后 suspend
		assert(!factory._recent_game->resumed);

		// tick 2: Overlay enters, update → pop_screen
		runner.tick();
		assert(factory._overlay_exited);             // pop 后 overlay exit
		assert(factory._recent_game->resumed);       // pop 后 game resume

		// tick 3: Game 继续运行
		factory._recent_game->updated = false;
		runner.tick();
		assert(factory._recent_game->updated);

		assert(platform.fake_display().present_count() == 3);
	}

	// ================================================================
	// 测试 3: pop_screen 在栈深度为 1 时是 no-op
	// ================================================================
	{
		TestScreenFactory factory(TestScreenFactory::SWITCH_TEST);
		handheld::FakePlatform platform({80, 80});
		handheld::ScreenRunner runner(platform, factory, ScreenType::BOOT);

		// tick 1: Boot → switch_to(MENU)
		runner.tick();
		// tick 2: Menu → switch_to(PLAYGROUND)
		runner.tick();
		// tick 3: Game enters, no push/pop
		runner.tick();

		// 栈上只有一个屏幕，pop_screen 在 ScreenRunner::_apply_pending
		// 中检查到 stack.size() <= 1 时不做任何操作
		assert(platform.fake_display().present_count() == 3);
	}

	return 0;
}
