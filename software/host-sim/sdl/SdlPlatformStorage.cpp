#include "sdl/SdlPlatform.h"

namespace handheld {

bool SdlPlatform::Storage::exists(const char* /*key*/) const {
	return false;
}

bool SdlPlatform::Storage::load(const char* /*key*/, void* /*data*/, size_t /*size*/) const {
	return false;
}

bool SdlPlatform::Storage::save(const char* /*key*/, const void* /*data*/, size_t /*size*/) {
	return false;
}

bool SdlPlatform::Storage::erase(const char* /*key*/) {
	return false;
}

} // namespace handheld
