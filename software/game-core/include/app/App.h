#ifndef APP_APP_H
#define APP_APP_H

#include "core/runtime/IGameHost.h"
#include "core/runtime/Game.h"

namespace handheld {

class App : public IGameHost {
public:
	App(IPlatform& platform, Game& game);
	App(const App&) = delete;
	App(App&&) = delete;
	App& operator=(const App&) = delete;
	App& operator=(App&&) = delete;
	~App() = default;

	void tick();
	void run_forever();
	void switch_to(Game& game) override;
	Game& current_game() override;

private:
	void apply_pending_switch();

	IPlatform& _platform;
	Game* _current_game;
	Game* _pending_game = nullptr;
	bool _entered = false;
};

} // namespace handheld

#endif
