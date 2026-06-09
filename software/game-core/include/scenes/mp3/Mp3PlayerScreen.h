#ifndef SCENES_MP3_MP3_PLAYER_SCREEN_H
#define SCENES_MP3_MP3_PLAYER_SCREEN_H

#include "core/graphics/Color.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld::mp3::cfg {

constexpr bool ENABLE_BGM = false; // 已有自己的 mp3 tracks，不走全局 BGM

// 设备外形
constexpr int16_t BODY_X = 2, BODY_Y = 2, BODY_W = 76, BODY_H = 76;
// 屏幕凹陷区
constexpr int16_t SCREEN_X = 6, SCREEN_Y = 8, SCREEN_W = 68, SCREEN_H = 42;
// 屏幕内进度条
constexpr int16_t PROGRESS_X = 9, PROGRESS_Y = 26, PROGRESS_W = 62, PROGRESS_H = 4;
// 4 个按钮
constexpr int16_t BTN_Y = 56, BTN_H = 14, BTN_W = 14;
constexpr int16_t BTN_X_PREV = 8, BTN_X_PLAY = 26, BTN_X_NEXT = 44, BTN_X_MODE = 62;
// 底部提示文字
constexpr int16_t HINT_Y = 74;

// 颜色
constexpr Color BG_COLOR = rgb565(8, 12, 20);
constexpr Color BODY_COLOR = rgb565(28, 32, 40);
constexpr Color SCREEN_BG = rgb565(12, 18, 30);
constexpr Color SCREEN_FG = rgb565(180, 220, 240);
constexpr Color PROGRESS_BG = rgb565(40, 50, 60);
constexpr Color PROGRESS_FG = rgb565(120, 200, 240);
constexpr Color PROGRESS_FRAME = rgb565(80, 100, 120);
constexpr Color BTN_BORDER = rgb565(80, 90, 110);
constexpr Color BTN_ACTIVE = rgb565(120, 200, 240);
constexpr Color BTN_LABEL = rgb565(200, 210, 220);
constexpr Color STATUS_COLOR = rgb565(180, 220, 180);
constexpr Color HINT_COLOR = rgb565(120, 140, 160);

} // namespace handheld::mp3::cfg

namespace handheld {

class Mp3PlayerScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    void play_current_track(IScreenHost& host);
    void pause_track(IScreenHost& host);
    void resume_track(IScreenHost& host);

    uint8_t _track_idx = 0;        // 当前曲目索引
    bool _playing = false;          // 是否正在播放
    uint32_t _track_frame = 0;      // 自当前 track 开始以来的帧计数（用于进度）
    uint16_t _track_duration_ms = 0; // 当前曲目总时长（ms），play_current_track 时缓存
};

} // namespace handheld

#endif
