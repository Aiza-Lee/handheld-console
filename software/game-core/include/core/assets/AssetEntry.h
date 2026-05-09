#ifndef CORE_ASSETS_ASSET_ENTRY_H
#define CORE_ASSETS_ASSET_ENTRY_H

#include <cstdint>

namespace handheld {

// 资产条目 — 描述一个编译期内置资产
struct AssetEntry {
	uint16_t id;			// 资产标识符
	const void* data;		// 指向编译期数据的指针
	uint32_t size;			// 数据大小（字节）
};

}  // namespace handheld

#endif  // CORE_ASSETS_ASSET_ENTRY_H
