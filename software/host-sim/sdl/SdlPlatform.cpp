#include "sdl/SdlPlatform.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_surface.h"
#include <SDL3/SDL.h>

#include <stdexcept>
#include <string>

namespace handheld {

namespace {

std::runtime_error _sdl_error(const char* message) {
	return std::runtime_error(std::string(message) + ": " + SDL_GetError());
}

ButtonBits _button_from_scancode(SDL_Scancode scancode, bool& recognized) {
	recognized = true;

	switch (scancode) {
	case SDL_SCANCODE_W:
	case SDL_SCANCODE_UP:
		return ButtonBits::UP;
	case SDL_SCANCODE_S:
	case SDL_SCANCODE_DOWN:
		return ButtonBits::DOWN;
	case SDL_SCANCODE_A:
	case SDL_SCANCODE_LEFT:
		return ButtonBits::LEFT;
	case SDL_SCANCODE_D:
	case SDL_SCANCODE_RIGHT:
		return ButtonBits::RIGHT;
	case SDL_SCANCODE_J:
		return ButtonBits::A;
	case SDL_SCANCODE_K:
		return ButtonBits::B;
	case SDL_SCANCODE_RETURN:
		return ButtonBits::START;
	case SDL_SCANCODE_LSHIFT:
	case SDL_SCANCODE_RSHIFT:
		return ButtonBits::SELECT;
	default:
		recognized = false;
		return ButtonBits::NONE;
	}
}

} // namespace

SdlPlatform::SdlPlatform(Size display_size, int window_scale, uint32_t frame_time_ms)
	: _display(display_size),
	  _frame_time_ms(frame_time_ms) {
	// The simulator currently stubs audio, so it should not depend on a host audio device.
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		throw _sdl_error("SDL_Init failed");
	}

	_window = SDL_CreateWindow(
		"handheld-console host sim",
		display_size.width * window_scale,
		display_size.height * window_scale,
		0
	);
	if (_window == nullptr) {
		SDL_Quit();
		throw _sdl_error("SDL_CreateWindow failed");
	}

	_renderer = SDL_CreateRenderer(_window, nullptr);
	if (_renderer == nullptr) {
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw _sdl_error("SDL_CreateRenderer failed");
	}

	_texture = SDL_CreateTexture(
		_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		display_size.width,
		display_size.height
	);
	if (_texture == nullptr) {
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw _sdl_error("SDL_CreateTexture failed");
	}

	if (!SDL_SetTextureScaleMode(_texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART)) {
		SDL_DestroyTexture(_texture);
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw _sdl_error("SDL_SetTextureScaleMode failed");
	}

	if (!SDL_SetRenderLogicalPresentation(
			_renderer,
			display_size.width,
			display_size.height,
			SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
		)) {
		SDL_DestroyTexture(_texture);
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw _sdl_error("SDL_SetRenderLogicalPresentation failed");
	}

	_display.bind(*_renderer, *_texture);
	_last_frame_tick = _time.ticks_ms();
}

SdlPlatform::~SdlPlatform() {
	if (_texture != nullptr) {
		SDL_DestroyTexture(_texture);
	}
	if (_renderer != nullptr) {
		SDL_DestroyRenderer(_renderer);
	}
	if (_window != nullptr) {
		SDL_DestroyWindow(_window);
	}
	SDL_Quit();
}

bool SdlPlatform::is_running() const {
	return _running;
}

void SdlPlatform::process_events() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		_handle_event(event);
	}
}

void SdlPlatform::delay_to_next_frame() {
	const uint32_t now = _time.ticks_ms();
	const uint32_t elapsed = now - _last_frame_tick;
	if (elapsed < _frame_time_ms) {
		_time.delay_ms(_frame_time_ms - elapsed);
	}
	_last_frame_tick = _time.ticks_ms();
}

IDisplay& SdlPlatform::display() {
	return _display;
}

IInput& SdlPlatform::input() {
	return _input;
}

IAudio& SdlPlatform::audio() {
	return _audio;
}

IPower& SdlPlatform::power() {
	return _power;
}

ITime& SdlPlatform::time() {
	return _time;
}

IStorage& SdlPlatform::storage() {
	return _storage;
}

void SdlPlatform::_handle_event(const SDL_Event& event) {
	switch (event.type) {
	case SDL_EVENT_QUIT:
		_running = false;
		return;
	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP: {
		if (event.key.repeat) {
			return;
		}

		if (event.key.scancode == SDL_SCANCODE_ESCAPE && event.type == SDL_EVENT_KEY_DOWN) {
			_running = false;
			return;
		}

		bool recognized = false;
		const ButtonBits button = _button_from_scancode(event.key.scancode, recognized);
		if (recognized) {
			_input.set_button(button, event.type == SDL_EVENT_KEY_DOWN);
		}
		return;
	}
	default:
		return;
	}
}

} // namespace handheld
