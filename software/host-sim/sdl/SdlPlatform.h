#ifndef SOFTWARE_HOST_SIM_SDL_PLATFORM_H
#define SOFTWARE_HOST_SIM_SDL_PLATFORM_H

#include "core/common/ButtonState.h"
#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "platform/interfaces/IAudio.h"
#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IStorage.h"
#include "platform/interfaces/ITime.h"

#include <cstdint>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;
union SDL_Event;

namespace handheld {

class SdlPlatform final : public IPlatform {
public:
	explicit SdlPlatform(Size display_size = {240, 240}, int window_scale = 4, uint32_t frame_time_ms = 16);
	~SdlPlatform() override;

	SdlPlatform(const SdlPlatform&) = delete;
	SdlPlatform(SdlPlatform&&) = delete;
	SdlPlatform& operator=(const SdlPlatform&) = delete;
	SdlPlatform& operator=(SdlPlatform&&) = delete;

	[[nodiscard]] bool is_running() const;
	void process_events();
	void delay_to_next_frame();

	IDisplay& display() override;
	IInput& input() override;
	IAudio& audio() override;
	IPower& power() override;
	ITime& time() override;
	IStorage& storage() override;

private:
	class Display final : public IDisplay {
	public:
		explicit Display(Size size);

		[[nodiscard]] int16_t width() const override;
		[[nodiscard]] int16_t height() const override;
		void clear(Color color) override;
		void draw_pixel(int16_t x, int16_t y, Color color) override;
		void present() override;

		void bind(SDL_Renderer& renderer, SDL_Texture& texture);

	private:
		[[nodiscard]] uint32_t _to_argb8888(Color color) const;

		Size _size;
		std::vector<uint32_t> _framebuffer;
		SDL_Renderer* _renderer = nullptr;
		SDL_Texture* _texture = nullptr;
	};

	class Input final : public IInput {
	public:
		void poll() override;
		[[nodiscard]] ButtonState current_buttons() const override;
		[[nodiscard]] ButtonState previous_buttons() const override;

		void set_button(ButtonBits button, bool pressed);

	private:
		ButtonState _current;
		ButtonState _previous;
		ButtonState _pending;
	};

	class Audio final : public IAudio {
	public:
		void play_tone(Tone tone) override;
		void play_sequence(const Tone* tones, size_t tone_count, bool loop) override;
		void set_master_volume(uint8_t volume) override;
		[[nodiscard]] uint8_t master_volume() const override;
		void set_muted(bool muted) override;
		[[nodiscard]] bool is_muted() const override;
		void stop() override;
		[[nodiscard]] bool is_playing() const override;

	private:
		uint8_t _volume = 255;
		bool _muted = false;
		bool _playing = false;
	};

	class Power final : public IPower {
	public:
		[[nodiscard]] PowerStatus read_status() const override;
		[[nodiscard]] bool can_suspend() const override;
		void suspend() override;
	};

	class Time final : public ITime {
	public:
		Time();

		[[nodiscard]] uint32_t ticks_ms() const override;
		void delay_ms(uint32_t duration_ms) override;

	private:
		uint64_t _start_ticks = 0;
	};

	class Storage final : public IStorage {
	public:
		bool exists(const char* key) const override;
		bool load(const char* key, void* data, size_t size) const override;
		bool save(const char* key, const void* data, size_t size) override;
		bool erase(const char* key) override;
	};

	void _handle_event(const SDL_Event& event);

	Display _display;
	Input _input;
	Audio _audio;
	Power _power;
	Time _time;
	Storage _storage;
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	SDL_Texture* _texture = nullptr;
	uint32_t _frame_time_ms = 16;
	uint32_t _last_frame_tick = 0;
	bool _running = true;
};

} // namespace handheld

#endif
