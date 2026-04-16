#include "app/App.h"

#include "games/menu/MenuGame.h"
#include "platform/interfaces/IAudio.h"
#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IStorage.h"
#include "platform/interfaces/ITime.h"
#include "ti_msp_dl_config.h"

namespace handheld {

namespace {

// 平台适配完成前的临时空实现
class NullDisplay final : public IDisplay {
public:
	int16_t width() const override { return 0; }
	int16_t height() const override { return 0; }
	void clear(Color) override {}
	void draw_pixel(int16_t, int16_t, Color) override {}
	void present() override {}
};

class NullInput final : public IInput {
public:
	void poll() override {
		previous_ = current_;
		current_ = {};
	}

	ButtonState current_buttons() const override { return current_; }
	ButtonState previous_buttons() const override { return previous_; }

private:
	ButtonState current_;
	ButtonState previous_;
};

class NullAudio final : public IAudio {
public:
	void play_tone(Tone) override {}
	void play_sequence(const Tone*, size_t, bool) override {}
	void set_master_volume(uint8_t volume) override { volume_ = volume; }
	uint8_t master_volume() const override { return volume_; }
	void set_muted(bool muted) override { muted_ = muted; }
	bool is_muted() const override { return muted_; }
	void stop() override {}
	bool is_playing() const override { return false; }

private:
	uint8_t volume_ = 255;
	bool muted_ = false;
};

class NullPower final : public IPower {
public:
	PowerStatus read_status() const override { return {}; }
	bool can_suspend() const override { return false; }
	void suspend() override {}
};

class NullTime final : public ITime {
public:
	uint32_t ticks_ms() const override { return 0; }
	void delay_ms(uint32_t) override {}
};

class NullStorage final : public IStorage {
public:
	bool exists(const char*) const override { return false; }
	bool load(const char*, void*, size_t) const override { return false; }
	bool save(const char*, const void*, size_t) override { return false; }
	bool erase(const char*) override { return false; }
};

class BootstrapPlatform final : public IPlatform {
public:
	IDisplay& display() override { return display_; }
	IInput& input() override { return input_; }
	IAudio& audio() override { return audio_; }
	IPower& power() override { return power_; }
	ITime& time() override { return time_; }
	IStorage& storage() override { return storage_; }

private:
	NullDisplay display_;
	NullInput input_;
	NullAudio audio_;
	NullPower power_;
	NullTime time_;
	NullStorage storage_;
};

} // namespace

void App::run() {
	SYSCFG_DL_init();

	BootstrapPlatform platform;
	MenuGame menu;
	menu.enter(platform);
	menu.update(platform);
	menu.render(platform);
}

} // namespace handheld
