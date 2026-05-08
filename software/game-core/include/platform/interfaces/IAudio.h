#ifndef PLATFORM_INTERFACES_IAUDIO_H
#define PLATFORM_INTERFACES_IAUDIO_H

#include <cstddef>
#include <cstdint>

namespace handheld {

struct Tone {
	uint16_t frequencyHz = 0;
	uint16_t durationMs = 0;
};

// 音频接口
class IAudio {
public:
	IAudio() = default;
	IAudio(const IAudio&) = delete;
	IAudio(IAudio&&) = delete;
	IAudio& operator=(const IAudio&) = delete;
	IAudio& operator=(IAudio&&) = delete;
	virtual ~IAudio() = default;

	// 播放单个音调
	virtual void play_tone(Tone tone) = 0;

	// 播放音调序列
	virtual void play_sequence(const Tone* tones, size_t tone_count, bool loop) = 0;

	// 设置静音状态
	virtual void set_muted(bool muted) = 0;

	// 返回静音状态
	[[nodiscard]] virtual bool is_muted() const = 0;

	// 停止当前播放
	virtual void stop() = 0;

	// 返回播放状态
	[[nodiscard]] virtual bool is_playing() const = 0;

	void play_tone(uint16_t frequency_hz, uint16_t duration_ms) {
		play_tone(Tone{frequency_hz, duration_ms});
	}
};

} // namespace handheld

#endif
