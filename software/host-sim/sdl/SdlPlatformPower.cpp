#include "sdl/SdlPlatform.h"

namespace handheld {

PowerStatus SdlPlatform::Power::read_status() const {
	return {};
}

bool SdlPlatform::Power::can_suspend() const {
	return false;
}

void SdlPlatform::Power::suspend() {}

} // namespace handheld
