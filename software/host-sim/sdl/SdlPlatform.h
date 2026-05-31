#ifndef SOFTWARE_HOST_SIM_SDL_PLATFORM_H
#define SOFTWARE_HOST_SIM_SDL_PLATFORM_H

#include "core/common/ButtonState.h"
#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/assets/BuiltinAssetProvider.h"
#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IStorage.h"
#include "platform/interfaces/ITime.h"

#include <cstdint>
#include <vector>

struct SDL_AudioStream;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;
union SDL_Event;

namespace handheld {

class SdlPlatform final : public IPlatform {
public:
	class Display final : public IDisplay {
	public:
		explicit Display(Size size);

		[[nodiscard]] int16_t width() const override;
		[[nodiscard]] int16_t height() const override;
		void clear(Color color) override;
		void draw_pixel(int16_t x, int16_t y, Color color) override;
			void fill_rect(const Rect& rect, Color color) override;
		void present() override;

		void bind(SDL_Renderer& renderer, SDL_Texture& texture);

	private:
		[[nodiscard]] uint32_t _to_argb8888(Color color) const;

		Size _size;
		std::vector<uint32_t> _framebuffer;
		SDL_Renderer* _renderer = nullptr;
		SDL_Texture* _texture = nullptr;
	};

	explicit SdlPlatform(Size display_size = {80, 80}, int window_scale = 4);
	~SdlPlatform() override;

	SdlPlatform(const SdlPlatform&) = delete;
	SdlPlatform(SdlPlatform&&) = delete;
	SdlPlatform& operator=(const SdlPlatform&) = delete;
	SdlPlatform& operator=(SdlPlatform&&) = delete;

	[[nodiscard]] bool is_running() const;
	void process_events();
	void delay_to_next_frame(uint32_t frame_time_ms);

	// 设置内置资产表（通常在 main() 中调用）
	void init_assets(const AssetEntry* entries, size_t count);

	IDisplay& display() override;
	IInput& input() override;
	void write_audio_samples(const int16_t* data, size_t count) override;
	IPower& power() override;
	ITime& time() override;
	IStorage& storage() override;
	IAssetProvider& assets() override;

private:
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
	SDL_AudioStream* _audio_stream = nullptr;
	bool _audio_muted = false;
	Power _power;
	Time _time;
	Storage _storage;
	BuiltinAssetProvider _assets;
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	SDL_Texture* _texture = nullptr;
	uint32_t _last_frame_tick = 0;
	bool _running = true;
};

} // namespace handheld

#endif
