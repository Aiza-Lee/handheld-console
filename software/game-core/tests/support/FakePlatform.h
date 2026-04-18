#ifndef GAME_CORE_TESTS_SUPPORT_FAKE_PLATFORM_H
#define GAME_CORE_TESTS_SUPPORT_FAKE_PLATFORM_H

#include "core/common/ButtonBits.h"
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

#include <cstddef>
#include <cstdint>

namespace handheld {

class FakeDisplay final : public IDisplay {
public:
	explicit FakeDisplay(Size size = {160, 128}) : _size(size) {}

	[[nodiscard]] int16_t width() const override { return _size.width; }
	[[nodiscard]] int16_t height() const override { return _size.height; }

	void clear(Color color) override {
		_last_clear_color = color;
		++_clear_count;
	}

	void draw_pixel(int16_t /*x*/, int16_t /*y*/, Color /*color*/) override { ++_draw_pixel_count; }

	void present() override { ++_present_count; }

	[[nodiscard]] Color last_clear_color() const { return _last_clear_color; }
	[[nodiscard]] uint32_t clear_count() const { return _clear_count; }
	[[nodiscard]] uint32_t draw_pixel_count() const { return _draw_pixel_count; }
	[[nodiscard]] uint32_t present_count() const { return _present_count; }

private:
	Size _size;
	Color _last_clear_color = Color::BLACK;
	uint32_t _clear_count = 0;
	uint32_t _draw_pixel_count = 0;
	uint32_t _present_count = 0;
};

class FakeInput final : public IInput {
public:
	void poll() override {
		_previous = _current;
		_current = _pending;
		++_poll_count;
	}

	[[nodiscard]] ButtonState current_buttons() const override { return _current; }
	[[nodiscard]] ButtonState previous_buttons() const override { return _previous; }
	[[nodiscard]] uint32_t poll_count() const { return _poll_count; }

	void set_button(ButtonBits button, bool pressed) {
		_pending.set(button, pressed);
	}

private:
	ButtonState _current;
	ButtonState _previous;
	ButtonState _pending;
	uint32_t _poll_count = 0;
};

class FakeAudio final : public IAudio {
public:
	void play_tone(Tone tone) override {
		_last_tone = tone;
		_playing = true;
	}

	void play_sequence(const Tone* /*tones*/, size_t /*tone_count*/, bool loop) override {
		_looping = loop;
		_playing = true;
	}

	void set_master_volume(uint8_t volume) override { _volume = volume; }
	[[nodiscard]] uint8_t master_volume() const override { return _volume; }
	void set_muted(bool muted) override { _muted = muted; }
	[[nodiscard]] bool is_muted() const override { return _muted; }

	void stop() override {
		_playing = false;
		_looping = false;
	}

	[[nodiscard]] bool is_playing() const override { return _playing; }
	[[nodiscard]] Tone last_tone() const { return _last_tone; }
	[[nodiscard]] bool is_looping() const { return _looping; }

private:
	Tone _last_tone{};
	uint8_t _volume = 255;
	bool _muted = false;
	bool _playing = false;
	bool _looping = false;
};

class FakePower final : public IPower {
public:
	[[nodiscard]] PowerStatus read_status() const override { return _status; }
	[[nodiscard]] bool can_suspend() const override { return _allow_suspend; }
	void suspend() override {}

private:
	PowerStatus _status{};
	bool _allow_suspend = false;
};

class FakeTime final : public ITime {
public:
	[[nodiscard]] uint32_t ticks_ms() const override { return _ticks_ms; }
	void delay_ms(uint32_t duration_ms) override { _ticks_ms += duration_ms; }

private:
	uint32_t _ticks_ms = 0;
};

class FakeStorage final : public IStorage {
public:
	bool exists(const char* /*key*/) const override { return false; }
	bool load(const char* /*key*/, void* /*data*/, size_t /*size*/) const override { return false; }
	bool save(const char* /*key*/, const void* /*data*/, size_t /*size*/) override { return false; }
	bool erase(const char* /*key*/) override { return false; }
};

class FakePlatform final : public IPlatform {
public:
	explicit FakePlatform(Size display_size = {160, 128}) : _display(display_size) {}

	IDisplay& display() override { return _display; }
	IInput& input() override { return _input; }
	IAudio& audio() override { return _audio; }
	IPower& power() override { return _power; }
	ITime& time() override { return _time; }
	IStorage& storage() override { return _storage; }

	FakeDisplay& fake_display() { return _display; }
	FakeInput& fake_input() { return _input; }

private:
	FakeDisplay _display;
	FakeInput _input;
	FakeAudio _audio;
	FakePower _power;
	FakeTime _time;
	FakeStorage _storage;
};

} // namespace handheld

#endif
