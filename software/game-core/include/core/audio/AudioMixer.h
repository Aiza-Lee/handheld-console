#ifndef CORE_AUDIO_AUDIO_MIXER_H
#define CORE_AUDIO_AUDIO_MIXER_H

#include "platform/interfaces/IAudio.h"

#include <cstddef>
#include <cstdint>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace handheld {

// 4通道软件音频混音器，在 game-core 层实现多通道混合。
// 通道 0 专用于 BGM（支持循环），通道 1-3 用于一次性 SFX。
// 输出 44100Hz S16LE 单声道 PCM，通过 IAudio::write_samples() 输出。
class AudioMixer {
public:
	static constexpr int CHANNEL_COUNT = 4;
	static constexpr int SAMPLE_RATE = 44100;
	static constexpr int BGM_CHANNEL = 0;
	static constexpr int SFX_CHANNEL_START = 1;
	static constexpr size_t SAMPLES_PER_FRAME = SAMPLE_RATE * 65 / 1000; // ~2866, 覆盖 SDL 默认 67ms 帧率

	AudioMixer() = default;

	// 在通道 0 启动背景音乐（自动停止之前的 BGM）
	void set_bgm(const Tone* tones, size_t count) {
		Channel& ch = _channels[BGM_CHANNEL];
		ch.active = true;
		ch.tones = tones;
		ch.tone_count = count;
		ch.tone_index = 0;
		ch.samples_elapsed = 0;
		ch.phase = 0.0;
		ch.loop = true;
		ch.volume = 0.30f;
	}

	// 在第一个空闲 SFX 通道播放一次性音效（不中断 BGM）
	void play_sfx(const Tone* tones, size_t count) {
		for (int i = SFX_CHANNEL_START; i < CHANNEL_COUNT; ++i) {
			if (!_channels[i].active) {
				Channel& ch = _channels[i];
				ch.active = true;
				ch.tones = tones;
				ch.tone_count = count;
				ch.tone_index = 0;
				ch.samples_elapsed = 0;
				ch.phase = 0.0;
				ch.loop = false;
				ch.volume = 0.50f;
				return;
			}
		}
	}

	// 停止所有通道
	void stop_all() {
		for (auto& ch : _channels) {
			ch.active = false;
		}
	}

	// 仅停止 BGM 通道
	void stop_bgm() {
		_channels[BGM_CHANNEL].active = false;
	}

	// 是否有任何通道在播放
	[[nodiscard]] bool is_playing() const {
		for (const auto& ch : _channels) {
			if (ch.active) return true;
		}
		return false;
	}

	// 生成混合后的 PCM 采样，写入 buf（调用者确保 buf 至少有 sample_count 个元素）
	void fill_buffer(int16_t* buf, size_t sample_count) {
		for (size_t i = 0; i < sample_count; ++i) {
			int32_t sum = 0;
			for (int ci = 0; ci < CHANNEL_COUNT; ++ci) {
				Channel& ch = _channels[ci];
				if (ch.active) {
					sum += _sample(ch);
					_advance(ch);
				}
			}
			if (sum > 32767) sum = 32767;
			if (sum < -32768) sum = -32768;
			buf[i] = static_cast<int16_t>(sum);
		}
	}

private:
	struct Channel {
		bool active = false;
		const Tone* tones = nullptr;
		size_t tone_count = 0;
		size_t tone_index = 0;
		uint32_t samples_elapsed = 0;
		double phase = 0.0;
		bool loop = false;
		float volume = 1.0f;
	};

	Channel _channels[CHANNEL_COUNT];

	// 返回当前通道在当前位置的采样值（含音量）。使用连续相位避免音符切换时的爆音。
	[[nodiscard]] int32_t _sample(const Channel& ch) const {
		const Tone& tone = ch.tones[ch.tone_index];
		if (tone.frequencyHz == 0) return 0;
		return static_cast<int32_t>(std::sin(ch.phase) * 28000.0 * ch.volume);
	}

	// 推进通道一个采样位置，处理 Tone 切换和循环。相位连续累积，频率变化时不产生跳变。
	void _advance(Channel& ch) {
		const Tone& tone = ch.tones[ch.tone_index];
		uint32_t tone_samples = static_cast<uint32_t>(tone.durationMs) * SAMPLE_RATE / 1000;
		if (tone_samples == 0) tone_samples = 1;

		if (tone.frequencyHz > 0) {
			ch.phase += 2.0 * M_PI * static_cast<double>(tone.frequencyHz) / SAMPLE_RATE;
			if (ch.phase > 2.0 * M_PI) ch.phase -= 2.0 * M_PI;
		}

		++ch.samples_elapsed;
		if (ch.samples_elapsed >= tone_samples) {
			ch.samples_elapsed = 0;
			++ch.tone_index;
			if (ch.tone_index >= ch.tone_count) {
				if (ch.loop) {
					ch.tone_index = 0;
				} else {
					ch.active = false;
				}
			}
		}
	}
};

} // namespace handheld

#endif
