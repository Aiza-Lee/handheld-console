#include "sdl/SdlPlatform.h"

#include "SDL3/SDL_storage.h"

#include <cstdio>

namespace handheld {

namespace {

// 把 uint16_t key 映射到 SDL_Storage 内的相对路径。
// 单一文件 = 单一槽位；命名稳定，便于外部脚本备份/调试。
void _path_for(uint16_t key, char* out, size_t out_size) {
    std::snprintf(out, out_size, "k_%04x.bin", key);
}

} // namespace

IStorage::Status SdlPlatform::Storage::read(uint16_t key, void* buf, uint32_t size) const {
    if (!_sdl || !SDL_StorageReady(_sdl)) return Status::IO_ERROR;

    char path[16];
    _path_for(key, path, sizeof(path));

    Uint64 actual = 0;
    if (!SDL_GetStorageFileSize(_sdl, path, &actual)) return Status::NOT_FOUND;
    if (actual != size) return Status::INVALID_SIZE;

    if (!SDL_ReadStorageFile(_sdl, path, buf, static_cast<Uint64>(size))) return Status::IO_ERROR;
    return Status::OK;
}

IStorage::Status SdlPlatform::Storage::write(uint16_t key, const void* buf, uint32_t size) {
    if (!_sdl || !SDL_StorageReady(_sdl)) return Status::IO_ERROR;

    char path[16];
    _path_for(key, path, sizeof(path));

    if (!SDL_WriteStorageFile(_sdl, path, buf, static_cast<Uint64>(size))) return Status::IO_ERROR;
    return Status::OK;
}

IStorage::Status SdlPlatform::Storage::commit() {
    // SDL_Storage 写完即落盘（文件系统直通），no-op flush。
    return Status::OK;
}

IStorage::Status SdlPlatform::Storage::erase(uint16_t key) {
    if (!_sdl || !SDL_StorageReady(_sdl)) return Status::IO_ERROR;

    char path[16];
    _path_for(key, path, sizeof(path));

    if (!SDL_RemoveStoragePath(_sdl, path)) return Status::IO_ERROR;
    return Status::OK;
}

bool SdlPlatform::Storage::exists(uint16_t key) const {
    if (!_sdl || !SDL_StorageReady(_sdl)) return false;

    char path[16];
    _path_for(key, path, sizeof(path));

    Uint64 actual = 0;
    return SDL_GetStorageFileSize(_sdl, path, &actual) && actual > 0;
}

uint32_t SdlPlatform::Storage::size_of(uint16_t key) const {
    if (!_sdl || !SDL_StorageReady(_sdl)) return 0;

    char path[16];
    _path_for(key, path, sizeof(path));

    Uint64 actual = 0;
    if (!SDL_GetStorageFileSize(_sdl, path, &actual)) return 0;
    return static_cast<uint32_t>(actual);
}

} // namespace handheld
