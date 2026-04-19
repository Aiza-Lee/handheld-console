#include "sdl/SdlPlatform.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <stdexcept>
#include <string>

namespace handheld {

namespace {

std::runtime_error sdl_error(const char* message) {
	return std::runtime_error(std::string(message) + ": " + SDL_GetError());
}

} // namespace

SdlPlatform::Display::Display(Size size)
	: _size(size),
	  _framebuffer(static_cast<size_t>(size.width) * static_cast<size_t>(size.height), _to_argb8888(Color::BLACK)) {}

int16_t SdlPlatform::Display::width() const {
	return _size.width;
}

int16_t SdlPlatform::Display::height() const {
	return _size.height;
}

void SdlPlatform::Display::clear(Color color) {
	std::fill(_framebuffer.begin(), _framebuffer.end(), _to_argb8888(color));
}

void SdlPlatform::Display::draw_pixel(int16_t x, int16_t y, Color color) {
	if (x < 0 || y < 0 || x >= _size.width || y >= _size.height) {
		return;
	}

	const auto index = (static_cast<size_t>(y) * static_cast<size_t>(_size.width)) + static_cast<size_t>(x);
	_framebuffer[index] = _to_argb8888(color);
}

void SdlPlatform::Display::present() {
	if (_renderer == nullptr || _texture == nullptr) {
		return;
	}

	const int pitch = static_cast<int>(static_cast<size_t>(_size.width) * sizeof(uint32_t));
	if (!SDL_UpdateTexture(_texture, nullptr, _framebuffer.data(), pitch)) {
		throw sdl_error("SDL_UpdateTexture failed");
	}

	if (!SDL_RenderClear(_renderer)) {
		throw sdl_error("SDL_RenderClear failed");
	}

	if (!SDL_RenderTexture(_renderer, _texture, nullptr, nullptr)) {
		throw sdl_error("SDL_RenderTexture failed");
	}

	if (!SDL_RenderPresent(_renderer)) {
		throw sdl_error("SDL_RenderPresent failed");
	}
}
void SdlPlatform::Display::bind(SDL_Renderer& renderer, SDL_Texture& texture) {
	_renderer = &renderer;
	_texture = &texture;
}

uint32_t SdlPlatform::Display::_to_argb8888(Color color) const {
	const auto value = static_cast<uint16_t>(color);
	const auto red = static_cast<uint8_t>(((value >> 11) & 0x1FU) * 255U / 31U);
	const auto green = static_cast<uint8_t>(((value >> 5) & 0x3FU) * 255U / 63U);
	const auto blue = static_cast<uint8_t>((value & 0x1FU) * 255U / 31U);

	return 0xFF000000U |
		(static_cast<uint32_t>(red) << 16) |
		(static_cast<uint32_t>(green) << 8) |
		static_cast<uint32_t>(blue);
}

} // namespace handheld
