#ifndef PLATFORM_INTERFACES_IPLATFORM_H
#define PLATFORM_INTERFACES_IPLATFORM_H

#include "platform/interfaces/IAudio.h"
#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IStorage.h"
#include "platform/interfaces/ITime.h"

namespace handheld {

// 平台服务集合
class IPlatform {
public:
	IPlatform() = default;
	IPlatform(const IPlatform&) = delete;
	IPlatform(IPlatform&&) = delete;
	IPlatform& operator=(const IPlatform&) = delete;
	IPlatform& operator=(IPlatform&&) = delete;
	virtual ~IPlatform() = default;

	// 返回显示服务
	virtual IDisplay& display() = 0;

	// 返回输入服务
	virtual IInput& input() = 0;

	// 返回音频服务
	virtual IAudio& audio() = 0;

	// 返回电源服务
	virtual IPower& power() = 0;

	// 返回时间服务
	virtual ITime& time() = 0;

	// 返回存储服务
	virtual IStorage& storage() = 0;
};

} // namespace handheld

#endif
