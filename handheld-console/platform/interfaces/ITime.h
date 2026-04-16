#ifndef PLATFORM_INTERFACES_ITIME_H
#define PLATFORM_INTERFACES_ITIME_H

#include <stdint.h>

namespace handheld {

// 时间接口
class ITime {
public:
	virtual ~ITime() = default;

	// 返回累计毫秒数
	virtual uint32_t ticks_ms() const = 0;

	// 阻塞指定毫秒数
	virtual void delay_ms(uint32_t duration_ms) = 0;
};

} // namespace handheld

#endif
