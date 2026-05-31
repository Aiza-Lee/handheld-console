#ifndef CORE_AUDIO_AUDIO_ENGINE_H
#define CORE_AUDIO_AUDIO_ENGINE_H

#include "core/audio/Sounds.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace handheld {

// 4通道软件音频引擎，在 game-core 层实现多通道混合。
// 通道 0 专用于 BGM（循环），通道 1-3 用于一次性 SFX。
// 输出 44100Hz S16LE 单声道 PCM。
class AudioEngine {
public:
    static constexpr int CHANNEL_COUNT = 4;
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BGM_CHANNEL = 0;
    static constexpr int SFX_CHANNEL_START = 1;

    AudioEngine() = default;

    void set_bgm(const Tone* tones, size_t count) {
        if (tones == nullptr || count == 0) {
            _channels[BGM_CHANNEL].active = false;
            return;
        }
        Channel& ch = _channels[BGM_CHANNEL];
        ch.active = true;
        ch.tones = tones;
        ch.tone_count = count;
        ch.tone_index = 0;
        ch.samples_elapsed = 0;
        ch.phase = 0.0;
        ch.loop = true;
        ch.volume = 0.30F;
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
                ch.phase = 0.0;
                ch.loop = false;
                ch.volume = 0.50F;
                return;
            }
        }
    }

    void stop_all() {
        for (auto& ch : _channels) {
            ch.active = false;
        }
    }

    void stop_bgm() {
        _channels[BGM_CHANNEL].active = false;
    }

    [[nodiscard]] bool is_playing() const {
        for (const auto& ch : _channels) {
            if (ch.active) return true;
        }
        return false;
    }

    void fill_buffer(int16_t* buf, size_t sample_count) {
        for (size_t i = 0; i < sample_count; ++i) {
            int32_t sum = 0;
            for (auto & ch : _channels) {
                if (ch.active) {
                    sum += _sample(ch);
                    _advance(ch);
                }
            }
            sum = std::min(sum, 32767);
            sum = std::max(sum, -32768);
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
        float volume = 1.0F;
    };

    Channel _channels[CHANNEL_COUNT];

    [[nodiscard]] int32_t _sample(const Channel& ch) const {
        const Tone& tone = ch.tones[ch.tone_index];
        if (tone.frequencyHz == 0) return 0;
        return static_cast<int32_t>(std::sin(ch.phase) * 28000.0 * ch.volume);
    }

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
