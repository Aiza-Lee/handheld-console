#ifndef CORE_RUNTIME_SCREEN_FACTORY_H
#define CORE_RUNTIME_SCREEN_FACTORY_H

#include "core/runtime/ScreenType.h"
#include <memory>

namespace handheld {

class GameScreen;

// 屏幕工厂接口
class IScreenFactory {
public:
    IScreenFactory() = default;
    IScreenFactory(const IScreenFactory&) = delete;
    IScreenFactory(IScreenFactory&&) = delete;
    IScreenFactory& operator=(const IScreenFactory&) = delete;
    IScreenFactory& operator=(IScreenFactory&&) = delete;
    virtual ~IScreenFactory() = default;

    // 根据屏幕类型创建屏幕实例
    virtual std::unique_ptr<GameScreen> create(ScreenType type) = 0;
};

// 默认屏幕工厂实现
class DefaultScreenFactory : public IScreenFactory {
public:
    DefaultScreenFactory() = default;
    ~DefaultScreenFactory() override = default;

    std::unique_ptr<GameScreen> create(ScreenType type) override;
};

} // namespace handheld

#endif // CORE_RUNTIME_SCREEN_FACTORY_H
