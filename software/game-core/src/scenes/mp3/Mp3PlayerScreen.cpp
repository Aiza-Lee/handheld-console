#include "scenes/mp3/Mp3PlayerScreen.h"

#include "scenes/mp3/Mp3Tracks.h"

#include "core/audio/Sounds.h"
#include "core/common/Algorithm.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

using namespace mp3::cfg;

namespace {

// 把毫秒格式化成 "M:SS\0"（最长 6 字节：4 字符 + \0 + 1 安全位）
void format_mmss(uint16_t ms, char* out) {
    uint16_t const total_s = static_cast<uint16_t>(ms / 1000U);
    uint16_t const m = static_cast<uint16_t>(total_s / 60U);
    uint16_t const s = static_cast<uint16_t>(total_s % 60U);
    out[0] = static_cast<char>('0' + (m > 9 ? 9 : m)); // 单数字位上限 9 分钟
    out[1] = ':';
    out[2] = static_cast<char>('0' + (s / 10U));
    out[3] = static_cast<char>('0' + (s % 10U));
    out[4] = '\0';
}

// 计算 track 的总时长（ms）。切歌时调用一次并缓存。
uint16_t compute_track_duration_ms(const Tone* tones, size_t count) {
    uint32_t total = 0;
    for (size_t i = 0; i < count; ++i) total += tones[i].durationMs;
    return static_cast<uint16_t>(total > 0xFFFFU ? 0xFFFFU : total);
}

} // namespace

void Mp3PlayerScreen::enter(IPlatform& platform, IScreenHost& host) {
    platform.display().clear(BG_COLOR);
    _track_idx = 0;
    _playing = false;
    _track_frame = 0;
    // 音量与全局 AudioEngine 共享（Settings 屏的修改在此自然生效，无需本地字段）
    play_current_track(host);
}

void Mp3PlayerScreen::play_current_track(IScreenHost& host) {
    if (mp3::TRACK_COUNT == 0) return;
    const auto& tr = mp3::TRACKS[_track_idx];
    host.audio().set_bgm(tr.tones, tr.count);
    _track_duration_ms = compute_track_duration_ms(tr.tones, tr.count);
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

    // UP / DOWN：调整全局 BGM 音量 ±5%（写入共享 AudioEngine，Settings 屏立即看到）
    if (input.was_pressed(ButtonBits::UP)) {
        host.audio().set_bgm_volume(handheld::clamp(
            static_cast<uint8_t>(host.audio().bgm_volume() + 5), uint8_t{0}, uint8_t{100}));
        return;
    }
    if (input.was_pressed(ButtonBits::DOWN)) {
        host.audio().set_bgm_volume(handheld::clamp(
            static_cast<uint8_t>(host.audio().bgm_volume() - 5), uint8_t{0}, uint8_t{100}));
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

void Mp3PlayerScreen::render(IPlatform& platform, IScreenHost& host) {
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
        // Row 0-7: track NAME in 5x7（center.y=13 → origin.y=10，完全在屏幕凹陷区内）
        TextRenderer::draw_text_centered(display,
            {static_cast<int16_t>(SCREEN_X + SCREEN_W / 2),
             static_cast<int16_t>(SCREEN_Y + 5)},
            mp3::TRACKS[_track_idx].name, SCREEN_FG, 1, BASIC_FONT_5X7);

        // Row 9-13: "1/16" 左 + "M:SS/M:SS" 右
        char idx_buf[8];
        idx_buf[0] = static_cast<char>('0' + ((_track_idx + 1) / 10));
        idx_buf[1] = static_cast<char>('0' + ((_track_idx + 1) % 10));
        idx_buf[2] = '/';
        idx_buf[3] = static_cast<char>('0' + (mp3::TRACK_COUNT / 10));
        idx_buf[4] = static_cast<char>('0' + (mp3::TRACK_COUNT % 10));
        idx_buf[5] = '\0';
        TextRenderer::draw_text(display,
            {static_cast<int16_t>(SCREEN_X + 3), static_cast<int16_t>(SCREEN_Y + 12)},
            idx_buf, HINT_COLOR, 1, COMPACT_FONT_3X5);

        const uint32_t current_ms = (_track_frame * 50U) % (_track_duration_ms > 0 ? _track_duration_ms : 1U);
        char cur[5], tot[5];
        format_mmss(static_cast<uint16_t>(current_ms), cur);
        format_mmss(_track_duration_ms, tot);
        // "M:SS/M:SS" = 8 chars + NUL = 9 bytes
        char time_buf[10];
        time_buf[0] = cur[0]; time_buf[1] = cur[1];
        time_buf[2] = cur[2]; time_buf[3] = cur[3];
        time_buf[4] = '/';
        time_buf[5] = tot[0]; time_buf[6] = tot[1];
        time_buf[7] = tot[2]; time_buf[8] = tot[3];
        time_buf[9] = '\0';
        // 8 chars × 4 px = 32 px；从右侧 SCREEN_X + SCREEN_W - 3 - 32 起画
        TextRenderer::draw_text(display,
            {static_cast<int16_t>(SCREEN_X + SCREEN_W - 3 - 32), static_cast<int16_t>(SCREEN_Y + 12)},
            time_buf, HINT_COLOR, 1, COMPACT_FONT_3X5);

        // Row 15-19: 进度条
        display.draw_rect({static_cast<int16_t>(PROGRESS_X - 1), static_cast<int16_t>(PROGRESS_Y - 1),
                           static_cast<int16_t>(PROGRESS_W + 2), static_cast<int16_t>(PROGRESS_H + 2)},
                          PROGRESS_FRAME);
        display.fill_rect({PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H}, PROGRESS_BG);
        if (_track_duration_ms > 0) {
            const int16_t fill_w = static_cast<int16_t>(
                (static_cast<uint32_t>(PROGRESS_W) * current_ms) / _track_duration_ms);
            if (fill_w > 0) {
                display.fill_rect({PROGRESS_X, PROGRESS_Y, fill_w, PROGRESS_H}, PROGRESS_FG);
            }
        }

        // Row 22-26: 状态 + 5 个 bar + "50%" 数字（全部从共享 AudioEngine 读取音量）
        const uint8_t volume_pct = host.audio().bgm_volume();
        const char* status = _playing ? "PLAYING" : "PAUSED";
        TextRenderer::draw_text(display,
            {static_cast<int16_t>(SCREEN_X + 3), static_cast<int16_t>(SCREEN_Y + 24)},
            status, STATUS_COLOR, 1, COMPACT_FONT_3X5);

        // 5 个 fill_rect bar：2px 宽 + 1px gap；5×2 + 4×1 = 14 px；x=37..50
        const uint8_t filled = static_cast<uint8_t>((volume_pct + 10) / 20); // 0-5 整数
        const int16_t bar_y = static_cast<int16_t>(SCREEN_Y + 25);
        constexpr int16_t bars_left = SCREEN_X + 31; // = 37
        for (uint8_t i = 0; i < 5; ++i) {
            const int16_t bx = static_cast<int16_t>(bars_left + (i * 3));
            display.fill_rect(Rect{bx, bar_y, 2, 4}, (i < filled) ? PROGRESS_FG : PROGRESS_BG);
        }
        // 音量数字（3 字符 "50%"，11 px 宽）在 x=52 起，正好在 bar 之后
        char vol_buf[4];
        vol_buf[0] = (volume_pct >= 100) ? '1' : static_cast<char>('0' + ((volume_pct / 10) % 10));
        vol_buf[1] = (volume_pct >= 100) ? '0' : static_cast<char>('0' + (volume_pct % 10));
        vol_buf[2] = '%';
        vol_buf[3] = '\0';
        TextRenderer::draw_text(display,
            {static_cast<int16_t>(SCREEN_X + 46), static_cast<int16_t>(SCREEN_Y + 24)},
            vol_buf, STATUS_COLOR, 1, COMPACT_FONT_3X5);
    }

    // 4. 按钮行：3 个方框（去掉 MODE，UP/DOWN 改成音量键）
    enum class BtnKind { PREV, PLAY_PAUSE, NEXT };
    struct Btn { int16_t x; BtnKind kind; };
    const Btn btns[] = {
        {BTN_X_PREV, BtnKind::PREV},
        {BTN_X_PLAY, BtnKind::PLAY_PAUSE},
        {BTN_X_NEXT, BtnKind::NEXT},
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
        }
    }

    // 5. 底部操作提示（19 字符 × 4 px = 76 px，居中后 x=2..78，80 px 显示内安全）
    TextRenderer::draw_text_centered(display, {40, HINT_Y}, "L/R:T A:PLY B:EXIT",
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
