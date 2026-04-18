#include "sdl/SdlPlatform.h"

#include <SDL3/SDL.h>

namespace handheld {

SdlPlatform::Time::Time() : _start_ticks(SDL_GetTicks()) {}

uint32_t SdlPlatform::Time::ticks_ms() const {
	return static_cast<uint32_t>(SDL_GetTicks() - _start_ticks);
}

void SdlPlatform::Time::delay_ms(uint32_t duration_ms) {
	SDL_Delay(duration_ms);
}

} // namespace handheld
