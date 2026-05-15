#include "sdl/SdlPlatform.h"

#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace handheld {

namespace {

constexpr int SAMPLE_RATE = 44100;
constexpr int MAX_VOLUME = 28000;
constexpr SDL_AudioFormat AUDIO_FMT = SDL_AUDIO_S16LE;

void _generate_sine(Tone tone, std::vector<int16_t>& out) {
	int n = static_cast<int>(tone.durationMs) * SAMPLE_RATE / 1000;
	int start = static_cast<int>(out.size());
	out.resize(static_cast<size_t>(start + n));
	for (int i = 0; i < n; ++i) {
		double t = static_cast<double>(start + i) / SAMPLE_RATE;
		double val = (tone.frequencyHz > 0)
			? std::sin(2.0 * M_PI * tone.frequencyHz * t) * MAX_VOLUME
			: 0.0;
		out[start + i] = static_cast<int16_t>(val);
	}
}

} // namespace

SdlPlatform::Audio::Audio() {
	_ensure_stream();
}

SdlPlatform::Audio::~Audio() {
	close();
}

void SdlPlatform::Audio::close() {
	if (_stream) {
		SDL_ClearAudioStream(_stream);
		SDL_DestroyAudioStream(_stream);
		_stream = nullptr;
	}
}

SDL_AudioSpec SdlPlatform::Audio::_spec() {
	SDL_AudioSpec s{};
	s.format = AUDIO_FMT;
	s.channels = 1;
	s.freq = SAMPLE_RATE;
	return s;
}

void SdlPlatform::Audio::_ensure_stream() {
	if (_stream) return;
	SDL_AudioSpec s = _spec();
	_stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &s, nullptr, nullptr);
}

void SdlPlatform::Audio::_queue_tone(Tone tone) {
	if (!_stream || _muted) return;
	std::vector<int16_t> buf;
	_generate_sine(tone, buf);
	if (!buf.empty()) {
		SDL_PutAudioStreamData(_stream, buf.data(),
			static_cast<int>(buf.size() * sizeof(int16_t)));
	}
}

void SdlPlatform::Audio::play_tone(Tone tone) {
	_ensure_stream();
	SDL_ClearAudioStream(_stream);
	_queue_tone(tone);
	SDL_ResumeAudioStreamDevice(_stream);
}

void SdlPlatform::Audio::play_sequence(const Tone* tones, size_t tone_count, bool /*loop*/) {
	_ensure_stream();
	SDL_ClearAudioStream(_stream);
	for (size_t i = 0; i < tone_count; ++i) {
		_queue_tone(tones[i]);
	}
	SDL_ResumeAudioStreamDevice(_stream);
}

void SdlPlatform::Audio::set_muted(bool muted) {
	_muted = muted;
}

bool SdlPlatform::Audio::is_muted() const {
	return _muted;
}

void SdlPlatform::Audio::stop() {
	if (_stream) SDL_ClearAudioStream(_stream);
}

bool SdlPlatform::Audio::is_playing() const {
	return _stream && SDL_GetAudioStreamQueued(_stream) > 0;
}

void SdlPlatform::Audio::write_samples(const int16_t* data, size_t count) {
	_ensure_stream();
	if (_muted || !_stream || count == 0) return;
	SDL_PutAudioStreamData(_stream, data, static_cast<int>(count * sizeof(int16_t)));
	SDL_ResumeAudioStreamDevice(_stream);
}

} // namespace handheld
