#ifndef CORE_AUDIO_AUDIO_ENGINE_H
#define CORE_AUDIO_AUDIO_ENGINE_H

#include "core/audio/Sounds.h"
#include "core/common/Algorithm.h"

#include <cstddef>
#include <cstdint>

namespace handheld {

// 4通道软件音频引擎，在 game-core 层实现多通道混合。
// 通道 0 专用于 BGM（循环），通道 1-3 用于一次性 SFX。
// 输出 44100Hz S16LE 单声道方波 PCM（适配 PWM 蜂鸣器硬件）。
// 方波合成使用纯整数 DDS（Direct Digital Synthesis），无需浮点运算。
class AudioEngine {
public:
    static constexpr int CHANNEL_COUNT = 4;
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BGM_CHANNEL = 0;
    static constexpr int SFX_CHANNEL_START = 1;
    static constexpr int32_t kMaxAmp = 28000; // 100% 音量对应的方波峰值幅度

    AudioEngine() = default;

    void set_bgm(const Tone* tones, size_t count) {
        if (tones == nullptr || count == 0) {
            _channels[BGM_CHANNEL].active = false;
            return;
        }
		_bgm_paused = false;
        Channel& ch = _channels[BGM_CHANNEL];
        ch.active = true;
        ch.tones = tones;
        ch.tone_count = count;
        ch.tone_index = 0;
        ch.samples_elapsed = 0;
        ch.phase_accum = 0;
        ch.loop = true;
        ch.amp = static_cast<int32_t>(_bgm_volume_pct) * 280;
    }

    void play_sfx(const Tone* tones, size_t count) {
        if (tones == nullptr || count == 0) {
            return;
        }
        for (int i = SFX_CHANNEL_START; i < CHANNEL_COUNT; ++i) {
            if (!_channels[i].active) {
                Channel& ch = _channels[i];
                ch.active = true;
                ch.tones = tones;
                ch.tone_count = count;
                ch.tone_index = 0;
                ch.samples_elapsed = 0;
                ch.phase_accum = 0;
                ch.loop = false;
                ch.amp = static_cast<int32_t>(_sfx_volume_pct) * 280;
                return;
            }
        }
    }

    void stop_all() {
        for (auto& ch : _channels) {
            ch.active = false;
        }
    }

    void stop_bgm() { _channels[BGM_CHANNEL].active = false; }

    // 暂停 BGM：fill_buffer 跳过 BGM channel；channel 状态（位置/相位）保留，
    // 后续 resume_bgm() 从暂停点继续。
    void pause_bgm() { _bgm_paused = true; }

    // 恢复 BGM：从 pause_bgm() 的暂停点继续。
    void resume_bgm() { _bgm_paused = false; }

    [[nodiscard]] bool is_playing() const {
        for (const auto& ch : _channels) {
            if (ch.active) return true;
        }
        return false;
    }

    // ── 音量控制 ───────────────────────────────────

    [[nodiscard]] uint8_t bgm_volume() const { return _bgm_volume_pct; }
    [[nodiscard]] uint8_t sfx_volume() const { return _sfx_volume_pct; }

    void set_bgm_volume(uint8_t pct) {
        _bgm_volume_pct = handheld::min(pct, static_cast<uint8_t>(100));
        if (_channels[BGM_CHANNEL].active) {
            _channels[BGM_CHANNEL].amp = static_cast<int32_t>(_bgm_volume_pct) * 280;
        }
    }

    void set_sfx_volume(uint8_t pct) {
        _sfx_volume_pct = handheld::min(pct, static_cast<uint8_t>(100));
        for (int i = SFX_CHANNEL_START; i < CHANNEL_COUNT; ++i) {
            if (_channels[i].active) {
                _channels[i].amp = static_cast<int32_t>(_sfx_volume_pct) * 280;
            }
        }
    }

    // ── 查询 ───────────────────────────────────────

    // 返回当前活跃的最高优先级通道的频率（Hz）。
    // 优先级：SFX 通道 (3 > 2 > 1) > BGM 通道 (0)。
    // 无活跃通道时返回 0。
    [[nodiscard]] uint16_t active_frequency() const {
        for (int i = CHANNEL_COUNT - 1; i >= SFX_CHANNEL_START; --i) {
            if (_channels[i].active) {
                return _channels[i]
					.tones[_channels[i].tone_index]
					.frequencyHz;
            }
        }
        if (_channels[BGM_CHANNEL].active) {
            return _channels[BGM_CHANNEL]
				.tones[_channels[BGM_CHANNEL].tone_index]
				.frequencyHz;
        }
        return 0;
    }

    // 返回当前活跃的最高优先级通道的音量百分比（0-100）。
    [[nodiscard]] uint8_t active_volume_pct() const {
        for (int i = CHANNEL_COUNT - 1; i >= SFX_CHANNEL_START; --i) {
            if (_channels[i].active) return _sfx_volume_pct;
        }
        if (_channels[BGM_CHANNEL].active) return _bgm_volume_pct;
        return 0;
    }

    // 填充方波 PCM 采样缓冲区（纯整数 DDS）。
    void fill_buffer(int16_t* buf, size_t sample_count) {
        for (size_t i = 0; i < sample_count; ++i) {
            int32_t sum = 0;
            for (int ch_idx = 0; ch_idx < CHANNEL_COUNT; ++ch_idx) {
                if (ch_idx == BGM_CHANNEL && _bgm_paused) continue;
                Channel& ch = _channels[ch_idx];
                if (ch.active) {
                    sum += _sample(ch);
                    _advance(ch);
                }
            }
            sum = handheld::min(sum, 32767);
            sum = handheld::max(sum, -32768);
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
        uint32_t phase_accum = 0; // DDS 相位累加器 (0 ~ SAMPLE_RATE-1)
        bool loop = false;
        int32_t amp = 0; // 方波峰值幅度
    };

    Channel _channels[CHANNEL_COUNT];
    uint8_t _bgm_volume_pct = 5;
    uint8_t _sfx_volume_pct = 10;
    bool _bgm_paused = false; // BGM 暂停标志；true 时 fill_buffer 跳过 BGM channel

    // 纯整数方波：相位 < SAMPLE_RATE/2 输出 +amp，否则输出 -amp
    [[nodiscard]] int32_t _sample(const Channel& ch) const {
        const Tone& tone = ch.tones[ch.tone_index];
        if (tone.frequencyHz == 0) return 0;
        return (ch.phase_accum < (SAMPLE_RATE / 2U)) ? ch.amp : -ch.amp;
    }

    // 纯整数 DDS 相位推进 + 音符时长跟踪
    void _advance(Channel& ch) {
        const Tone& tone = ch.tones[ch.tone_index];
        ch.phase_accum += tone.frequencyHz;
        if (ch.phase_accum >= SAMPLE_RATE) {
            ch.phase_accum -= SAMPLE_RATE;
        }

        uint32_t tone_samples = static_cast<uint32_t>(tone.durationMs) * SAMPLE_RATE / 1000;
        if (tone_samples == 0) tone_samples = 1;

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
