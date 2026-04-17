#ifndef PLATFORM_INTERFACES_ISTORAGE_H
#define PLATFORM_INTERFACES_ISTORAGE_H

#include <cstddef>

namespace handheld {

// 持久化存储接口
class IStorage {
public:
	IStorage() = default;
	IStorage(const IStorage&) = delete;
	IStorage(IStorage&&) = delete;
	IStorage& operator=(const IStorage&) = delete;
	IStorage& operator=(IStorage&&) = delete;
	virtual ~IStorage() = default;

	// 判断键是否存在
	virtual bool exists(const char* key) const = 0;

	// 读取键对应的数据块
	virtual bool load(const char* key, void* data, size_t size) const = 0;

	// 保存键对应的数据块
	virtual bool save(const char* key, const void* data, size_t size) = 0;

	// 删除键对应的数据块
	virtual bool erase(const char* key) = 0;
};

} // namespace handheld

#endif
