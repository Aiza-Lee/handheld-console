#include "sdl/SdlPlatform.h"

namespace handheld {

void SdlPlatform::Input::poll() {
	_previous = _current;
	_current = _pending;
}

ButtonState SdlPlatform::Input::current_buttons() const {
	return _current;
}

ButtonState SdlPlatform::Input::previous_buttons() const {
	return _previous;
}

void SdlPlatform::Input::set_button(ButtonBits button, bool pressed) {
	_pending.set(button, pressed);
}

} // namespace handheld
