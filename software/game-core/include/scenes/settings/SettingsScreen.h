#ifndef SCENES_SETTINGS_SETTINGS_SCREEN_H
#define SCENES_SETTINGS_SETTINGS_SCREEN_H

#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 设置屏幕 — 调节 BGM / SFX 音量
class SettingsScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

    // 菜单中渲染设置选项的预览图标
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    uint8_t _selected_row = 0; // 0 = BGM, 1 = SFX
    uint32_t _frame = 0;
    uint8_t _hold_frames = 0; // 长按连发计数器
};

} // namespace handheld

#endif
