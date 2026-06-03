#ifndef CORE_EFFECTS_PARTICLE_SYSTEM_H
#define CORE_EFFECTS_PARTICLE_SYSTEM_H

#include "core/graphics/Color.h"
#include "platform/interfaces/IDisplay.h"

#include <cstddef>
#include <cstdint>

namespace handheld {

// 固定容量粒子系统 — 可在不同场景间复用。
// N 为编译期常量，限制最大粒子数以保证嵌入式性能。
template <size_t N> class ParticleSystem {
public:
    struct Particle {
        int8_t x, y;
        int8_t vx, vy;
        uint8_t life; // 0 = 已死亡
        Color color;
    };

    ParticleSystem() = default;

    // 发射一个粒子（自动复用死亡槽位）
    void emit(int8_t x, int8_t y, int8_t vx, int8_t vy, uint8_t life, Color color) {
        // 优先复用死亡槽
        for (size_t i = 0; i < _count; ++i) {
            if (_parts[i].life == 0) {
                _parts[i].x = x;
                _parts[i].y = y;
                _parts[i].vx = vx;
                _parts[i].vy = vy;
                _parts[i].life = life;
                _parts[i].color = color;
                return;
            }
        }
        // 未达上限则新增
        if (_count < N) {
            _parts[_count].x = x;
            _parts[_count].y = y;
            _parts[_count].vx = vx;
            _parts[_count].vy = vy;
            _parts[_count].life = life;
            _parts[_count].color = color;
            ++_count;
        }
    }

    // 每帧更新：移动粒子并衰减生命
    void update() {
        for (size_t i = 0; i < _count;) {
            Particle& p = _parts[i];
            if (p.life == 0) {
                // 与末尾交换后缩减
                p = _parts[_count - 1];
                --_count;
            } else {
                p.x = static_cast<int8_t>(p.x + p.vx);
                p.y = static_cast<int8_t>(p.y + p.vy);
                --p.life;
                ++i;
            }
        }
    }

    // 渲染所有活跃粒子（逐像素绘制）
    void render(IDisplay& display) const {
        for (size_t i = 0; i < _count; ++i) {
            if (_parts[i].life > 0) display.draw_pixel(_parts[i].x, _parts[i].y, _parts[i].color);
        }
    }

    // 清除所有粒子
    void clear() { _count = 0; }

    [[nodiscard]] size_t active_count() const {
        size_t n = 0;
        for (size_t i = 0; i < _count; ++i)
            if (_parts[i].life > 0) ++n;
        return n;
    }

    [[nodiscard]] static constexpr size_t capacity() { return N; }

private:
    Particle _parts[N]{};
    size_t _count = 0;
};

} // namespace handheld

#endif
