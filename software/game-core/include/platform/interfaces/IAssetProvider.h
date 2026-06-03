#ifndef PLATFORM_INTERFACES_IASSET_PROVIDER_H
#define PLATFORM_INTERFACES_IASSET_PROVIDER_H

#include <cstdint>

namespace handheld {

struct AssetEntry;

// 内置资产提供接口 — 读取编译进固件的静态数据（精灵、关卡、音效等）
class IAssetProvider {
public:
    IAssetProvider() = default;
    IAssetProvider(const IAssetProvider&) = delete;
    IAssetProvider(IAssetProvider&&) = delete;
    IAssetProvider& operator=(const IAssetProvider&) = delete;
    IAssetProvider& operator=(IAssetProvider&&) = delete;
    virtual ~IAssetProvider() = default;

    // 获取资产数据指针和大小。返回 false 表示未找到。
    virtual bool get(uint16_t asset_id, const void*& out_data, uint32_t& out_size) const = 0;

    // 判断指定 ID 的资产是否存在
    [[nodiscard]] virtual bool exists(uint16_t asset_id) const = 0;
};

} // namespace handheld

#endif // PLATFORM_INTERFACES_IASSET_PROVIDER_H
