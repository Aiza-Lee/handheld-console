#ifndef PLATFORM_NULL_NULL_PLATFORM_H
#define PLATFORM_NULL_NULL_PLATFORM_H

#include "core/common/Button.h"
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

// 平台适配完成前的空实现，供游戏逻辑开发和接线使用。
class NullDisplay final : public IDisplay {
public:
	explicit NullDisplay(Size size = {160, 128}) : size_(size) {}

	[[nodiscard]] int16_t width() const override { return size_.width; }
	[[nodiscard]] int16_t height() const override { return size_.height; }
	void clear(Color color) override {
		last_clear_color_ = color;
		++clear_count_;
	}
	void draw_pixel(int16_t /*x*/, int16_t /*y*/, Color /*color*/) override {}
	void present() override { ++present_count_; }

	void set_size(Size size) { size_ = size; }
	[[nodiscard]] Color last_clear_color() const { return last_clear_color_; }
	[[nodiscard]] uint32_t clear_count() const { return clear_count_; }
	[[nodiscard]] uint32_t present_count() const { return present_count_; }
	void reset_counters() {
		clear_count_ = 0;
		present_count_ = 0;
	}

private:
	Size size_;
	Color last_clear_color_ = Color::BLACK;
	uint32_t clear_count_ = 0;
	uint32_t present_count_ = 0;
};

class NullInput final : public IInput {
public:
	void poll() override {
		previous_ = current_;
		current_ = next_;
		++poll_count_;
	}

	[[nodiscard]] ButtonState current_buttons() const override { return current_; }
	[[nodiscard]] ButtonState previous_buttons() const override { return previous_; }
	[[nodiscard]] uint32_t poll_count() const { return poll_count_; }
	void reset_poll_count() { poll_count_ = 0; }

	void set_next_buttons(ButtonState buttons) { next_ = buttons; }

	void set_button(Button button, bool pressed) {
		next_.set(button, pressed);
	}

private:
	ButtonState current_;
	ButtonState previous_;
	ButtonState next_;
	uint32_t poll_count_ = 0;
};

class NullAudio final : public IAudio {
public:
	void play_tone(Tone tone) override {
		last_tone_ = tone;
		playing_ = true;
	}

	void play_sequence(const Tone* /*tones*/, size_t /*tone_count*/, bool loop) override {
		looping_ = loop;
		playing_ = true;
	}

	void set_master_volume(uint8_t volume) override { volume_ = volume; }
	[[nodiscard]] uint8_t master_volume() const override { return volume_; }
	void set_muted(bool muted) override { muted_ = muted; }
	[[nodiscard]] bool is_muted() const override { return muted_; }

	void stop() override {
		playing_ = false;
		looping_ = false;
	}

	[[nodiscard]] bool is_playing() const override { return playing_; }
	[[nodiscard]] Tone last_tone() const { return last_tone_; }
	[[nodiscard]] bool is_looping() const { return looping_; }

private:
	Tone last_tone_{};
	uint8_t volume_ = 255;
	bool muted_ = false;
	bool playing_ = false;
	bool looping_ = false;
};

class NullPower final : public IPower {
public:
	[[nodiscard]] PowerStatus read_status() const override { return status_; }
	[[nodiscard]] bool can_suspend() const override { return allow_suspend_; }
	void suspend() override {}

	void set_status(PowerStatus status) { status_ = status; }
	void set_can_suspend(bool allow_suspend) { allow_suspend_ = allow_suspend; }

private:
	PowerStatus status_{};
	bool allow_suspend_ = false;
};

class NullTime final : public ITime {
public:
	[[nodiscard]] uint32_t ticks_ms() const override { return ticks_ms_; }
	void delay_ms(uint32_t duration_ms) override { ticks_ms_ += duration_ms; }

	void set_ticks_ms(uint32_t ticks_ms) { ticks_ms_ = ticks_ms; }
	void advance_ms(uint32_t duration_ms) { ticks_ms_ += duration_ms; }

private:
	uint32_t ticks_ms_ = 0;
};

class NullStorage final : public IStorage {
public:
	bool exists(const char* /*key*/) const override { return false; }
	bool load(const char* /*key*/, void* /*data*/, size_t /*size*/) const override { return false; }
	bool save(const char* /*key*/, const void* /*data*/, size_t /*size*/) override { return false; }
	bool erase(const char* /*key*/) override { return false; }
};

class NullPlatform final : public IPlatform {
public:
	explicit NullPlatform(Size display_size = {160, 128}) : display_(display_size) {}

	IDisplay& display() override { return display_; }
	IInput& input() override { return input_; }
	IAudio& audio() override { return audio_; }
	IPower& power() override { return power_; }
	ITime& time() override { return time_; }
	IStorage& storage() override { return storage_; }

	NullDisplay& null_display() { return display_; }
	NullInput& null_input() { return input_; }
	NullAudio& null_audio() { return audio_; }
	NullPower& null_power() { return power_; }
	NullTime& null_time() { return time_; }
	NullStorage& null_storage() { return storage_; }

private:
	NullDisplay display_;
	NullInput input_;
	NullAudio audio_;
	NullPower power_;
	NullTime time_;
	NullStorage storage_;
};

} // namespace handheld

#endif
