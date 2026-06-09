#include "core/runtime/ScreenRunner.h"

#include "core/runtime/GameScreen.h"
#include "scenes/boot/BootScreen.h"
#include "scenes/menu/MenuScreen.h"
#include "scenes/snake/SnakeScreen.h"
#include "scenes/pacman/PacmanScreen.h"
#include "scenes/breakout/BreakoutScreen.h"
#include "scenes/invaders/InvadersScreen.h"
#include "scenes/growball/GrowBallScreen.h"
#include "scenes/settings/SettingsScreen.h"
#include "scenes/tetris/TetrisScreen.h"
#include "scenes/game2048/Game2048Screen.h"
#include "scenes/pong/PongScreen.h"
#include "scenes/developer/DeveloperScreen.h"
// Temporarily disabled to reduce firmware size:
// #include "scenes/playground/PlaygroundGuideScreen.h"
// #include "scenes/playground/PlaygroundScreen.h"
#include "scenes/mp3/Mp3PlayerScreen.h"

namespace handheld {

namespace {

ScreenRunner::MakeScreenFn g_make_screen_override = nullptr;

std::unique_ptr<GameScreen> make_screen(ScreenType type) {
    switch (type) {
        case ScreenType::BOOT: return std::make_unique<BootScreen>();
        case ScreenType::MENU: return std::make_unique<MenuScreen>();
        case ScreenType::SETTINGS: return std::make_unique<SettingsScreen>();
        case ScreenType::SNAKE: return std::make_unique<SnakeScreen>();
        case ScreenType::PACMAN: return std::make_unique<PacmanScreen>();
        case ScreenType::BREAKOUT: return std::make_unique<BreakoutScreen>();
        case ScreenType::INVADERS: return std::make_unique<InvadersScreen>();
        case ScreenType::GROW_BALL: return std::make_unique<GrowBallScreen>();
        case ScreenType::TETRIS: return std::make_unique<TetrisScreen>();
        case ScreenType::GAME_2048: return std::make_unique<Game2048Screen>();
        case ScreenType::PONG: return std::make_unique<PongScreen>();
        case ScreenType::DEVELOPER: return std::make_unique<DeveloperScreen>();
        case ScreenType::MP3: return std::make_unique<Mp3PlayerScreen>();
        // Disabled: GUIDE, PLAYGROUND (kept in enum for binary compat)
        default: return nullptr;
    }
}

} // namespace

void ScreenRunner::set_make_screen_override(MakeScreenFn fn) { g_make_screen_override = fn; }

ScreenRunner::ScreenRunner(IPlatform& platform, ScreenType initial_screen, uint32_t frame_time_ms) :
    _platform(platform), _pending_type(initial_screen), _pending_op(PendingOp::SWITCH),
    _frame_time_ms(frame_time_ms) {}

// ---- IScreenHost 实现 ----

void ScreenRunner::switch_to(ScreenType type) {
    _pending_type = type;
    _pending_op = PendingOp::SWITCH;
}

void ScreenRunner::push_screen(ScreenType type) {
    _pending_type = type;
    _pending_op = PendingOp::PUSH;
}

void ScreenRunner::pop_screen() { _pending_op = PendingOp::POP; }

// ---- 内部调度 ----

void ScreenRunner::_apply_pending() {
    switch (_pending_op) {
        case PendingOp::SWITCH: {
            // 退出并销毁栈中所有屏幕
            _audio_engine.stop_all();
            if (_stack_size > 0 && _entered) {
                _stack[_stack_size - 1]->exit(_platform, *this);
            }
            for (uint8_t i = 0; i < _stack_size; ++i) {
                _stack[i].reset();
            }
            _stack_size = 0;

            auto screen = g_make_screen_override
                              ? g_make_screen_override(_pending_type)
                              : make_screen(_pending_type);
            if (!screen) return;
            _stack[0] = std::move(screen);
            _stack_size = 1;
            _pending_op = PendingOp::NONE;
            _entered = false;
            break;
        }
        case PendingOp::PUSH: {
            // 挂起当前栈顶，压入新屏幕
            if (_stack_size >= K_MAX_SCREENS) {
                _pending_op = PendingOp::NONE;
                break;
            }
            if (_stack_size > 0 && _entered) {
                _stack[_stack_size - 1]->suspend(_platform, *this);
            }
            auto screen = g_make_screen_override
                              ? g_make_screen_override(_pending_type)
                              : make_screen(_pending_type);
            if (!screen) {
                _pending_op = PendingOp::NONE;
                break;
            }
            _stack[_stack_size] = std::move(screen);
            ++_stack_size;
            _pending_op = PendingOp::NONE;
            _entered = false;
            break;
        }
        case PendingOp::POP: {
            // 弹出栈顶，恢复下层屏幕
            if (_stack_size <= 1) {
                _pending_op = PendingOp::NONE;
                break;
            }
            _audio_engine.stop_all();
            if (_entered) {
                _stack[_stack_size - 1]->exit(_platform, *this);
            }
            _stack[_stack_size - 1].reset();
            --_stack_size;
            _pending_op = PendingOp::NONE;
            // 恢复刚回到栈顶的屏幕
            _stack[_stack_size - 1]->resume(_platform, *this);
            // _entered 保持 true：resume 相当于重新 enter
            break;
        }
        case PendingOp::NONE: break;
    }
}

void ScreenRunner::tick() {
    _apply_pending();

    if (_stack_size == 0) return;

    GameScreen& top = *_stack[_stack_size - 1];
    if (!_entered) {
        top.enter(_platform, *this);
        _entered = true;
    }

    _platform.input().poll();
    top.update(_platform, *this);
    top.render(_platform, *this);

    // 驱动音频引擎
    if (_audio_engine.is_playing()) {
        // 直接频率通道：PWM 蜂鸣器平台通过此路径设置频率，避免 PCM 检测开销
        _platform.set_buzzer_frequency(_audio_engine.active_frequency(), _audio_engine.active_volume_pct());

        // PCM 通道：为 SDL/DAC 平台生成方波 PCM 采样
        const size_t count = (AudioEngine::SAMPLE_RATE * _frame_time_ms) / 1000;
        _audio_engine.fill_buffer(_audio_buf.data(), count);
        _platform.write_audio_samples(_audio_buf.data(), count);
    }

    _platform.display().present();

    _apply_pending();
}

void ScreenRunner::run_forever() {
    _last_frame_tick = _platform.time().ticks_ms();
    for (;;) {
        uint32_t now = _platform.time().ticks_ms();
        uint32_t elapsed = now - _last_frame_tick;
        if (elapsed >= _frame_time_ms) {
            _last_frame_tick += _frame_time_ms;
            tick();
        }
    }
}

} // namespace handheld
