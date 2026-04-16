#ifndef PLATFORM_INTERFACES_IAUDIO_H
#define PLATFORM_INTERFACES_IAUDIO_H

#include <stddef.h>
#include <stdint.h>

namespace handheld {

struct Tone {
	uint16_t frequencyHz = 0;
	uint16_t durationMs = 0;
	uint8_t volume = 255;
};

// 音频接口
class IAudio {
public:
	virtual ~IAudio() = default;

	// 播放单个音调
	virtual void play_tone(Tone tone) = 0;

	// 播放音调序列
	virtual void play_sequence(const Tone* tones, size_t tone_count, bool loop) = 0;

	// 设置主音量
	virtual void set_master_volume(uint8_t volume) = 0;

	// 返回主音量
	virtual uint8_t master_volume() const = 0;

	// 设置静音状态
	virtual void set_muted(bool muted) = 0;

	// 返回静音状态
	virtual bool is_muted() const = 0;

	// 停止当前播放
	virtual void stop() = 0;

	// 返回播放状态
	virtual bool is_playing() const = 0;

	void play_tone(uint16_t frequency_hz, uint16_t duration_ms, uint8_t volume = 255) {
		play_tone(Tone{frequency_hz, duration_ms, volume});
	}
};

} // namespace handheld

#endif
