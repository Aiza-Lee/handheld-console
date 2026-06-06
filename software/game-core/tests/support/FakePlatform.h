#ifndef GAME_CORE_TESTS_SUPPORT_FAKE_PLATFORM_H
#define GAME_CORE_TESTS_SUPPORT_FAKE_PLATFORM_H

#include "core/common/ButtonBits.h"
#include "core/common/ButtonState.h"
#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "platform/interfaces/IDisplay.h"
#include "platform/interfaces/IInput.h"
#include "platform/interfaces/IPlatform.h"
#include "platform/interfaces/IPower.h"
#include "platform/interfaces/IStorage.h"
#include "platform/interfaces/ITime.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>

namespace handheld {

class FakeDisplay final : public IDisplay {
public:
    explicit FakeDisplay(Size size = {160, 128}) : _size(size) {}

    [[nodiscard]] int16_t width() const override { return _size.width; }
    [[nodiscard]] int16_t height() const override { return _size.height; }

    void clear(Color color) override {
        _last_clear_color = color;
        ++_clear_count;
    }

    void draw_pixel(int16_t /*x*/, int16_t /*y*/, Color /*color*/) override { ++_draw_pixel_count; }

    void present() override { ++_present_count; }

    [[nodiscard]] Color last_clear_color() const { return _last_clear_color; }
    [[nodiscard]] uint32_t clear_count() const { return _clear_count; }
    [[nodiscard]] uint32_t draw_pixel_count() const { return _draw_pixel_count; }
    [[nodiscard]] uint32_t present_count() const { return _present_count; }

private:
    Size _size;
    Color _last_clear_color = Color::BLACK;
    uint32_t _clear_count = 0;
    uint32_t _draw_pixel_count = 0;
    uint32_t _present_count = 0;
};

class FakeInput final : public IInput {
public:
    void poll() override {
        _previous = _current;
        _current = _pending;
        ++_poll_count;
    }

    [[nodiscard]] ButtonState current_buttons() const override { return _current; }
    [[nodiscard]] ButtonState previous_buttons() const override { return _previous; }
    [[nodiscard]] uint32_t poll_count() const { return _poll_count; }

    void set_button(ButtonBits button, bool pressed) { _pending.set(button, pressed); }

private:
    ButtonState _current;
    ButtonState _previous;
    ButtonState _pending;
    uint32_t _poll_count = 0;
};

class FakePower final : public IPower {
public:
    [[nodiscard]] PowerStatus read_status() const override { return _status; }
    [[nodiscard]] bool can_suspend() const override { return _allow_suspend; }
    void suspend() override {}

private:
    PowerStatus _status{};
    bool _allow_suspend = false;
};

class FakeTime final : public ITime {
public:
    [[nodiscard]] uint32_t ticks_ms() const override { return _ticks_ms; }
    void delay_ms(uint32_t duration_ms) override { _ticks_ms += duration_ms; }

private:
    uint32_t _ticks_ms = 0;
};

class FakeStorage final : public IStorage {
public:
    Status read(uint16_t key, void* buf, uint32_t size) const override {
        const auto it = _store.find(key);
        if (it == _store.end()) return Status::NOT_FOUND;
        if (it->second.size() != size) return Status::INVALID_SIZE;
        std::memcpy(buf, it->second.data(), size);
        return Status::OK;
    }

    Status write(uint16_t key, const void* buf, uint32_t size) override {
        const auto* p = static_cast<const uint8_t*>(buf);
        _store[key] = std::vector<uint8_t>(p, p + size);
        ++_write_count;
        return Status::OK;
    }

    Status commit() override { return Status::OK; }

    Status erase(uint16_t key) override {
        _store.erase(key);
        return Status::OK;
    }

    [[nodiscard]] bool exists(uint16_t key) const override { return _store.count(key) > 0; }

    [[nodiscard]] uint32_t size_of(uint16_t key) const override {
        const auto it = _store.find(key);
        return (it == _store.end()) ? 0u : static_cast<uint32_t>(it->second.size());
    }

    // ── 测试钩子 ──
    // 预置"出厂值"（不会自动进入 _store）
    void set_default(uint16_t key, const void* data, uint32_t size) {
        const auto* p = static_cast<const uint8_t*>(data);
        _defaults[key] = std::vector<uint8_t>(p, p + size);
    }

    // 把 _defaults 复制到 _store，模拟"开机读到 flash 已有值"
    void seed_defaults() { _store = _defaults; }

    void clear_all() { _store.clear(); }

    [[nodiscard]] uint32_t write_count() const { return _write_count; }

private:
    std::map<uint16_t, std::vector<uint8_t>> _store;
    std::map<uint16_t, std::vector<uint8_t>> _defaults;
    uint32_t _write_count = 0;
};

class FakePlatform final : public IPlatform {
public:
    explicit FakePlatform(Size display_size = {80, 80}) : _display(display_size) {}

    IDisplay& display() override { return _display; }
    IInput& input() override { return _input; }
    void write_audio_samples(const int16_t*, size_t count) override { _samples_written += count; }
    [[nodiscard]] size_t samples_written() const { return _samples_written; }
    IPower& power() override { return _power; }
    ITime& time() override { return _time; }
    IStorage& storage() override { return _storage; }

    FakeDisplay& fake_display() { return _display; }
    FakeInput& fake_input() { return _input; }
    FakeStorage& fake_storage() { return _storage; }

private:
    FakeDisplay _display;
    FakeInput _input;
    size_t _samples_written = 0;
    FakePower _power;
    FakeTime _time;
    FakeStorage _storage;
};

} // namespace handheld

#endif
