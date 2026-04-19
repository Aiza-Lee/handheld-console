#include "core/runtime/SceneRunner.h"
#include "core/runtime/SceneFactory.h"

namespace handheld {

SceneRunner::SceneRunner(IPlatform& platform, ISceneFactory& factory, SceneType initial_scene)
	: _platform(platform),
	  _factory(factory),
	  _pending_type(initial_scene),
	  _has_pending_switch(true) {}

void SceneRunner::switch_to(SceneType type) {
	_pending_type = type;
	_has_pending_switch = true;
}

Scene& SceneRunner::current_scene() {
	return *_current_scene;
}

void SceneRunner::_apply_pending_switch() {
	if (!_has_pending_switch) {
		return;
	}

	if (_entered && _current_scene) {
		_current_scene->exit(_platform, *this);
	}

	_current_scene = _factory.create(_pending_type);
	_has_pending_switch = false;
	_entered = false;
}

void SceneRunner::tick() {
	_apply_pending_switch();

	if (!_entered) {
		_current_scene->enter(_platform, *this);
		_entered = true;
	}

	_platform.input().poll();
	_current_scene->update(_platform, *this);
	_current_scene->render(_platform, *this);

	_platform.display().present();

	_apply_pending_switch();
}

void SceneRunner::run_forever() {
	for (;;) {
		tick();
	}
}

}  // namespace handheld
