// 临时测试：验证 SDL3 音频能否正常播放
// 编译: cd software/host-sim && clang++ -std=c++17 \
//   -I../game-core/include -I../../third_party/SDL/include \
//   sdl/audio_test.cpp \
//   -Lbuild/clang-debug/third_party/sdl -lSDL3 \
//   -o build/clang-debug/audio_test

#include "platform/interfaces/IAudio.h"
#include <SDL3/SDL.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

constexpr int SAMPLE_RATE = 44100;
constexpr int MAX_VOLUME = 28000;

static void generate_tone(handheld::Tone t, std::vector<int16_t>& out) {
	int n = static_cast<int>(t.durationMs) * SAMPLE_RATE / 1000;
	int start = static_cast<int>(out.size());
	out.resize(static_cast<size_t>(start + n));
	for (int i = 0; i < n; ++i) {
		double time = static_cast<double>(start + i) / SAMPLE_RATE;
		double val = (t.frequencyHz > 0)
			? std::sin(2.0 * M_PI * t.frequencyHz * time) * MAX_VOLUME
			: 0.0;
		out[start + i] = static_cast<int16_t>(val);
	}
}

int main() {
	if (!SDL_Init(SDL_INIT_AUDIO)) {
		std::fprintf(stderr, "SDL_Init audio failed: %s\n", SDL_GetError());
		return 1;
	}

	// 开机音乐音序
	const handheld::Tone boot_music[] = {
		{261, 100}, {329, 100}, {391, 100}, {523, 150},
		{0,   100},
		{523,  80}, {391,  80}, {329,  80}, {261, 300},
	};

	// 生成完整 PCM
	std::vector<int16_t> pcm;
	for (auto t : boot_music) {
		generate_tone(t, pcm);
	}

	std::printf("Generated %zu samples (%.1f sec)\n",
		pcm.size(), static_cast<double>(pcm.size()) / SAMPLE_RATE);

	// 打开音频设备
	SDL_AudioSpec spec;
	SDL_zero(spec);
	spec.format = SDL_AUDIO_S16LE;
	spec.channels = 1;
	spec.freq = SAMPLE_RATE;

	SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
	if (!stream) {
		std::fprintf(stderr, "SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// 推数据并开始播放
	SDL_PutAudioStreamData(stream, pcm.data(),
		static_cast<int>(pcm.size() * sizeof(int16_t)));
	SDL_ResumeAudioStreamDevice(stream);

	std::printf("Playing... press Ctrl+C to stop\n");

	// 轮询等待播放完毕
	while (SDL_GetAudioStreamQueued(stream) > 0) {
		SDL_Delay(50);
	}

	std::printf("Done.\n");

	SDL_DestroyAudioStream(stream);
	SDL_Quit();
	return 0;
}
