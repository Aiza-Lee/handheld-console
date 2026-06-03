#ifndef CORE_ASSETS_BUILTIN_ASSET_PROVIDER_H
#define CORE_ASSETS_BUILTIN_ASSET_PROVIDER_H

#include "platform/interfaces/IAssetProvider.h"
#include <cstddef>
#include <cstdint>

namespace handheld {

struct AssetEntry;

// 编译期内置资产提供者 — 从 const 数组（编译进 .rodata）查找资产
class BuiltinAssetProvider final : public IAssetProvider {
public:
    BuiltinAssetProvider() : _entries(nullptr), _count(0) {}
    BuiltinAssetProvider(const AssetEntry* entries, size_t count);

    void reset(const AssetEntry* entries, size_t count);

    bool get(uint16_t asset_id, const void*& out_data, uint32_t& out_size) const override;
    [[nodiscard]] bool exists(uint16_t asset_id) const override;

private:
    const AssetEntry* _entries;
    size_t _count;
};

} // namespace handheld

#endif // CORE_ASSETS_BUILTIN_ASSET_PROVIDER_H
