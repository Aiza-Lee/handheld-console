#include "sdl/SdlPlatform.h"

namespace handheld {

void SdlPlatform::Audio::play_tone(Tone /*tone*/) {
	_playing = true;
}

void SdlPlatform::Audio::play_sequence(const Tone* /*tones*/, size_t /*tone_count*/, bool /*loop*/) {
	_playing = true;
}

void SdlPlatform::Audio::set_master_volume(uint8_t volume) {
	_volume = volume;
}

uint8_t SdlPlatform::Audio::master_volume() const {
	return _volume;
}

void SdlPlatform::Audio::set_muted(bool muted) {
	_muted = muted;
}

bool SdlPlatform::Audio::is_muted() const {
	return _muted;
}

void SdlPlatform::Audio::stop() {
	_playing = false;
}

bool SdlPlatform::Audio::is_playing() const {
	return _playing;
}

} // namespace handheld
