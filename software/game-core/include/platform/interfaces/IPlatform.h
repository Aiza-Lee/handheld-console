#ifndef PLATFORM_INTERFACES_IPLATFORM_H
#define PLATFORM_INTERFACES_IPLATFORM_H

#include <cstddef>
#include <cstdint>

#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IAssetProvider.h"
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

	// 输出 PCM 音频采样（S16LE, 44100Hz, mono）
	virtual void write_audio_samples(const int16_t* data, size_t count) = 0;

	// 直接设置蜂鸣器频率（Hz）和音量百分比（0-100），用于 PWM 蜂鸣器。
	// 默认实现为空 — SDL/Fake 平台使用 PCM 路径，不需要此方法。
	virtual void set_buzzer_frequency(uint16_t freq_hz, uint8_t volume_pct) {}

	// 返回电源服务
	virtual IPower& power() = 0;

	// 返回时间服务
	virtual ITime& time() = 0;

	// 返回内置资产服务
	virtual IAssetProvider& assets() = 0;
};

} // namespace handheld

#endif
