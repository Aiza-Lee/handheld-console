#include "core/assets/BuiltinAssetProvider.h"
#include "core/assets/AssetEntry.h"

namespace handheld {

BuiltinAssetProvider::BuiltinAssetProvider(const AssetEntry* entries, size_t count) :
    _entries(entries), _count(count) {}

void BuiltinAssetProvider::reset(const AssetEntry* entries, size_t count) {
    _entries = entries;
    _count = count;
}

bool BuiltinAssetProvider::get(uint16_t asset_id, const void*& out_data, uint32_t& out_size) const {
    for (size_t i = 0; i < _count; ++i) {
        if (_entries[i].id == asset_id) {
            out_data = _entries[i].data;
            out_size = _entries[i].size;
            return true;
        }
    }
    out_data = nullptr;
    out_size = 0;
    return false;
}

bool BuiltinAssetProvider::exists(uint16_t asset_id) const {
    for (size_t i = 0; i < _count; ++i) {
        if (_entries[i].id == asset_id) return true;
    }
    return false;
}

} // namespace handheld
