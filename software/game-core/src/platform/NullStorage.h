#ifndef PLATFORM_NULL_STORAGE_H
#define PLATFORM_NULL_STORAGE_H

#include "platform/interfaces/IStorage.h"

namespace handheld {

// 桩实现：所有写操作返 IOError，所有读操作返 NotFound。
// 用于：(a) 固件初期未绑 FLASHCTL 时的占位 backend；
//       (b) 不需要持久化的 host 模式（如 headless smoke 的非持久场景）。
class NullStorage final : public IStorage {
public:
    Status read(uint16_t /*key*/, void* /*buf*/, uint32_t /*size*/) const override { return Status::NOT_FOUND; }
    Status write(uint16_t /*key*/, const void* /*buf*/, uint32_t /*size*/) override { return Status::IO_ERROR; }
    Status commit() override { return Status::IO_ERROR; }
    Status erase(uint16_t /*key*/) override { return Status::IO_ERROR; }
    [[nodiscard]] bool exists(uint16_t /*key*/) const override { return false; }
    [[nodiscard]] uint32_t size_of(uint16_t /*key*/) const override { return 0; }
};

} // namespace handheld

#endif
