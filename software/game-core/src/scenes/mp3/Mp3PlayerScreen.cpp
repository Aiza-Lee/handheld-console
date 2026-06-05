#include "scenes/mp3/Mp3PlayerScreen.h"

#include "scenes/mp3/Mp3Tracks.h"

#include "core/audio/Sounds.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

#include <cstdio>

namespace handheld {

using namespace mp3::cfg;

void Mp3PlayerScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG_COLOR);
    _track_idx = 0;
    _playing = false;
    _track_frame = 0;
    play_current_track(host);
}

void Mp3PlayerScreen::play_current_track(IScreenHost& host) {
    if (mp3::TRACK_COUNT == 0) return;
    const auto& tr = mp3::TRACKS[_track_idx];
    host.audio().set_bgm(tr.tones, tr.count);
    _playing = true;
    _track_frame = 0;  // 切歌时进度从 0 开始
}

void Mp3PlayerScreen::pause_track(IScreenHost& host) {
    host.audio().pause_bgm();
    _playing = false;
}

void Mp3PlayerScreen::resume_track(IScreenHost& host) {
    host.audio().resume_bgm();
    _playing = true;
}

void Mp3PlayerScreen::update(IPlatform& platform, IScreenHost& host) {
    // 只在播放时推进进度帧（暂停时 _track_frame 冻结）
    if (_playing) ++_track_frame;
    const auto& input = platform.input();

    // B：返回 menu
    if (input.was_pressed(ButtonBits::B)) {
        host.audio().stop_bgm();
        host.switch_to(ScreenType::MENU);
        return;
    }

    // LEFT / RIGHT：上 / 下一首
    if (input.was_pressed(ButtonBits::LEFT)) {
        _track_idx = static_cast<uint8_t>((_track_idx + mp3::TRACK_COUNT - 1) % mp3::TRACK_COUNT);
        play_current_track(host);
        return;
    }
    if (input.was_pressed(ButtonBits::RIGHT)) {
        _track_idx = static_cast<uint8_t>((_track_idx + 1) % mp3::TRACK_COUNT);
        play_current_track(host);
        return;
    }

    // A：播放 / 暂停
    if (input.was_pressed(ButtonBits::A)) {
        if (_playing) pause_track(host);
        else resume_track(host);
        return;
    }

    // START：暂停
    if (input.was_pressed(ButtonBits::START)) {
        if (_playing) pause_track(host);
        return;
    }
}

void Mp3PlayerScreen::render(IPlatform& platform, IScreenHost& /*host*/) {
    IDisplay& display = platform.display();
    display.clear(BG_COLOR);

    // 1. 机身外壳
    display.fill_rect({BODY_X, BODY_Y, BODY_W, BODY_H}, BODY_COLOR);

    // 2. 屏幕凹陷区（外框 + 内填）
    display.draw_rect({SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H}, BTN_BORDER);
    display.fill_rect({static_cast<int16_t>(SCREEN_X + 1), static_cast<int16_t>(SCREEN_Y + 1),
                       static_cast<int16_t>(SCREEN_W - 2), static_cast<int16_t>(SCREEN_H - 2)},
                      SCREEN_BG);

    // 3. 屏幕内文字
    if (mp3::TRACK_COUNT > 0) {
        char header[16];
        snprintf(header, sizeof(header), "TRACK %u/%u",
                 static_cast<unsigned>(_track_idx + 1),
                 static_cast<unsigned>(mp3::TRACK_COUNT));
        TextRenderer::draw_text(display, {static_cast<int16_t>(SCREEN_X + 3),
                                          static_cast<int16_t>(SCREEN_Y + 2)},
                                header, SCREEN_FG, 1, BASIC_FONT_5X7);
        TextRenderer::draw_text(display, {static_cast<int16_t>(SCREEN_X + 3),
                                          static_cast<int16_t>(SCREEN_Y + 12)},
                                mp3::TRACKS[_track_idx].name, SCREEN_FG, 1, COMPACT_FONT_3X5);
    }

    // 4. 进度条
    display.draw_rect({static_cast<int16_t>(PROGRESS_X - 1), static_cast<int16_t>(PROGRESS_Y - 1),
                       static_cast<int16_t>(PROGRESS_W + 2), static_cast<int16_t>(PROGRESS_H + 2)},
                      PROGRESS_FRAME);
    display.fill_rect({PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H}, PROGRESS_BG);
    if (mp3::TRACK_COUNT > 0) {
        const auto& tr = mp3::TRACKS[_track_idx];
        uint32_t total_ms = 0;
        for (size_t i = 0; i < tr.count; ++i) total_ms += tr.tones[i].durationMs;
        const uint32_t current_ms = (_track_frame * 50) % (total_ms > 0 ? total_ms : 1);
        const int16_t fill_w = (total_ms > 0)
            ? static_cast<int16_t>((static_cast<uint32_t>(PROGRESS_W) * current_ms) / total_ms)
            : 0;
        if (fill_w > 0) {
            display.fill_rect({PROGRESS_X, PROGRESS_Y, fill_w, PROGRESS_H}, PROGRESS_FG);
        }
    }

    // 5. 屏幕内状态行
    const char* status = _playing ? "PLAYING" : "PAUSED";
    TextRenderer::draw_text(display, {static_cast<int16_t>(SCREEN_X + 3),
                                      static_cast<int16_t>(SCREEN_Y + 36)},
                            status, STATUS_COLOR, 1, COMPACT_FONT_3X5);

    // 6. 按钮行：4 个方框 + 直接画出来的图标（不用 TextRenderer）
    enum class BtnKind { PREV, PLAY_PAUSE, NEXT, MODE };
    struct Btn { int16_t x; BtnKind kind; };
    const Btn btns[] = {
        {BTN_X_PREV, BtnKind::PREV},
        {BTN_X_PLAY, BtnKind::PLAY_PAUSE},
        {BTN_X_NEXT, BtnKind::NEXT},
        {BTN_X_MODE, BtnKind::MODE},
    };
    for (const auto& b : btns) {
        const bool is_play = (b.kind == BtnKind::PLAY_PAUSE);
        // 播放/暂停按钮在 playing 时高亮（图标用暗色对比）
        if (is_play && _playing) {
            display.fill_rect({b.x, BTN_Y, BTN_W, BTN_H}, BTN_ACTIVE);
        }
        display.draw_rect({b.x, BTN_Y, BTN_W, BTN_H}, BTN_BORDER);
        const int16_t cx = static_cast<int16_t>(b.x + BTN_W / 2);
        const int16_t cy = static_cast<int16_t>(BTN_Y + BTN_H / 2);
        const Color icon_color = (is_play && _playing) ? SCREEN_BG : BTN_LABEL;

        switch (b.kind) {
            case BtnKind::PREV:
                // 左侧 2x8 竖条 + 左三角
                display.fill_rect({static_cast<int16_t>(cx - 4),
                                   static_cast<int16_t>(cy - 4), 2, 8}, icon_color);
                display.draw_line(static_cast<int16_t>(cx + 3),
                                  static_cast<int16_t>(cy - 4),
                                  static_cast<int16_t>(cx - 2), cy, icon_color);
                display.draw_line(static_cast<int16_t>(cx - 2), cy,
                                  static_cast<int16_t>(cx + 3),
                                  static_cast<int16_t>(cy + 4), icon_color);
                display.draw_line(static_cast<int16_t>(cx + 3),
                                  static_cast<int16_t>(cy + 4),
                                  static_cast<int16_t>(cx + 3),
                                  static_cast<int16_t>(cy - 4), icon_color);
                break;
            case BtnKind::PLAY_PAUSE:
                if (_playing) {
                    // 暂停图标：两根 2x8 竖条
                    display.fill_rect({static_cast<int16_t>(cx - 3),
                                       static_cast<int16_t>(cy - 4), 2, 8}, icon_color);
                    display.fill_rect({static_cast<int16_t>(cx + 1),
                                       static_cast<int16_t>(cy - 4), 2, 8}, icon_color);
                } else {
                    // 播放图标：右三角
                    display.draw_line(static_cast<int16_t>(cx - 3),
                                      static_cast<int16_t>(cy - 4),
                                      static_cast<int16_t>(cx + 3), cy, icon_color);
                    display.draw_line(static_cast<int16_t>(cx + 3), cy,
                                      static_cast<int16_t>(cx - 3),
                                      static_cast<int16_t>(cy + 4), icon_color);
                    display.draw_line(static_cast<int16_t>(cx - 3),
                                      static_cast<int16_t>(cy + 4),
                                      static_cast<int16_t>(cx - 3),
                                      static_cast<int16_t>(cy - 4), icon_color);
                }
                break;
            case BtnKind::NEXT:
                // 右三角 + 右侧 2x8 竖条
                display.draw_line(static_cast<int16_t>(cx - 3),
                                  static_cast<int16_t>(cy - 4),
                                  static_cast<int16_t>(cx + 2), cy, icon_color);
                display.draw_line(static_cast<int16_t>(cx + 2), cy,
                                  static_cast<int16_t>(cx - 3),
                                  static_cast<int16_t>(cy + 4), icon_color);
                display.draw_line(static_cast<int16_t>(cx - 3),
                                  static_cast<int16_t>(cy + 4),
                                  static_cast<int16_t>(cx - 3),
                                  static_cast<int16_t>(cy - 4), icon_color);
                display.fill_rect({static_cast<int16_t>(cx + 2),
                                   static_cast<int16_t>(cy - 4), 2, 8}, icon_color);
                break;
            case BtnKind::MODE:
                // 三条横线（菜单图标）
                display.fill_rect({static_cast<int16_t>(cx - 4),
                                   static_cast<int16_t>(cy - 3), 8, 1}, icon_color);
                display.fill_rect({static_cast<int16_t>(cx - 4), cy, 8, 1}, icon_color);
                display.fill_rect({static_cast<int16_t>(cx - 4),
                                   static_cast<int16_t>(cy + 3), 8, 1}, icon_color);
                break;
        }
    }

    // 7. 底部操作提示（缩短到 17 字符，居中后 68px 宽，刚好放下）
    TextRenderer::draw_text_centered(display, {40, HINT_Y}, "L/R A:PLAY B:EXIT",
                                     HINT_COLOR, 1, COMPACT_FONT_3X5);
}

void Mp3PlayerScreen::render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame) {
    // 菜单预览：3×5 文字 + 横条模拟播放条。所有尺寸跟随 box 自适应：
    //   文字：box 宽 ≥ 12 用 "MP3"（12px 宽），否则用 "MP"（8px 宽）
    //   横条：宽 = box.width - 2（留 1px 边距），永远不出框
    const auto cx = static_cast<int16_t>(box.x + box.width / 2);
    const auto cy = static_cast<int16_t>(box.y + box.height / 2);
    // const char* text = (box.width >= 12) ? "MP3" : "MP";
    const char* text = "MP3";
    TextRenderer::draw_text_centered(display, {cx, static_cast<int16_t>(cy - 1)}, text,
                                     mp3::cfg::SCREEN_FG, 1, COMPACT_FONT_3X5);
    const int16_t bar_w = static_cast<int16_t>(box.width - 2);
    const int16_t bar_x = static_cast<int16_t>(cx - bar_w / 2);
    const int16_t bar_y = static_cast<int16_t>(cy + 3);
    display.draw_h_line(bar_x, bar_y, bar_w, mp3::cfg::PROGRESS_FRAME);
    const int phase = static_cast<int>((frame / 4) % bar_w);
    const int fill_w = phase + 1;
    display.draw_h_line(bar_x, bar_y, static_cast<int16_t>(fill_w), mp3::cfg::PROGRESS_FG);
}

} // namespace handheld
