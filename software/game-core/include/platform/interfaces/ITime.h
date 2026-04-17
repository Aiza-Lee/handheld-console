#ifndef PLATFORM_INTERFACES_ITIME_H
#define PLATFORM_INTERFACES_ITIME_H

#include <cstdint>

namespace handheld {

// 时间接口
class ITime {
public:
	ITime() = default;
	ITime(const ITime&) = delete;
	ITime(ITime&&) = delete;
	ITime& operator=(const ITime&) = delete;
	ITime& operator=(ITime&&) = delete;
	virtual ~ITime() = default;

	// 返回累计毫秒数
	[[nodiscard]] virtual uint32_t ticks_ms() const = 0;

	// 阻塞指定毫秒数
	virtual void delay_ms(uint32_t duration_ms) = 0;
};

} // namespace handheld

#endif
