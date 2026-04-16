#ifndef PLATFORM_INTERFACES_IPOWER_H
#define PLATFORM_INTERFACES_IPOWER_H

#include <stdint.h>

namespace handheld {

enum class PowerSource : uint8_t {
	UNKNOWN = 0,
	BATTERY,
	USB,
	EXTERNAL,
};

struct PowerStatus {
	uint16_t batteryMilliVolts = 0;
	uint8_t batteryPercent = 0;
	bool charging = false;
	PowerSource source = PowerSource::UNKNOWN;
};

// 电源状态接口
class IPower {
public:
	virtual ~IPower() = default;

	// 返回完整电源状态
	virtual PowerStatus read_status() const = 0;

	// 返回是否支持挂起
	virtual bool can_suspend() const = 0;

	// 请求进入挂起状态
	virtual void suspend() = 0;

	uint16_t read_battery_millivolts() const {
		return read_status().batteryMilliVolts;
	}

	uint8_t read_battery_percent() const {
		return read_status().batteryPercent;
	}

	bool is_charging() const {
		return read_status().charging;
	}
};

} // namespace handheld

#endif
