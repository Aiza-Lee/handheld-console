#ifndef PLATFORM_INTERFACES_ISTORAGE_H
#define PLATFORM_INTERFACES_ISTORAGE_H

#include <cstdint>

namespace handheld {

// 持久化键值存储：用户设置、音量、高分等需要跨冷启动保留的数据。
// 资产（精灵/关卡）已统一为编译期 const 数组，不走此接口。
//
// 容量合约（按硬件 flash 后端推导）：
//   - 总键数 ≤ 256 — key 0x0000..0x00FF 是"系统设置"区，
//                    key 0x0100..0x01FF 是"per-游戏高分"区，详见 StorageKeys.h
//   - 单值长度 ≤ 16 字节
//   - 总占用 ≈ 256 键 × 16 字节 + 64 字节位图 = 4 KB
//   - 当前三个后端：NullStorage（无存储）、FakeStorage（host RAM，无显式上限）、
//                   SdlPlatform::Storage（host 文件系统，无显式上限）。
//   - 未来 Mspm0Storage（MSPM0G3507 flash）落地时按此合约 enforce；违反时返 NO_SPACE。
//
// key 是编译期已知的 uint16_t 槽位 ID（沿用早期的 uint16_t 资源槽位设计），
// value 是定长字节块。read/write 必须按相同 size 调用；长度不匹配返 INVALID_SIZE。
class IStorage {
public:
    IStorage() = default;
    IStorage(const IStorage&) = delete;
    IStorage(IStorage&&) = delete;
    IStorage& operator=(const IStorage&) = delete;
    IStorage& operator=(IStorage&&) = delete;
    virtual ~IStorage() = default;

    // ── 容量合约常量 ──
    //   接口层承诺不写超过这些限制的键/值；违反时由 backend 决定如何响应
    //   （NullStorage 返 IO_ERROR，FakeStorage 静默接受，未来的 Mspm0Storage 返 NO_SPACE）。
    static constexpr uint16_t MAX_KEYS         = 256;
    static constexpr uint32_t MAX_VALUE_BYTES  = 16;

    // 操作结果 — 区分 "键不存在" 与 "硬件失败" 是高分/设置逻辑必需。
    enum class Status : uint8_t {
        OK = 0,
        NOT_FOUND,     // 键不存在
        IO_ERROR,      // 硬件/文件系统失败
        NO_SPACE,      // 空间不足 (MSPM0 扇区耗尽 / 合约违反)
        INVALID_SIZE,  // 已有记录长度与请求的 size 不匹配
    };

    // 读 key 到 buf，size 字节。NOT_FOUND/INVALID_SIZE 时不写 buf。
    virtual Status read(uint16_t key, void* buf, uint32_t size) const = 0;

    // 写 key，size 字节。本地实现可缓冲，*不一定* 落盘 —— 见 commit()。
    virtual Status write(uint16_t key, const void* buf, uint32_t size) = 0;

    // 显式刷盘。MSPM0 闪存实现合并多次 write 一次性编程；SDL 实现可 no-op。
    virtual Status commit() = 0;

    // 删除 key
    virtual Status erase(uint16_t key) = 0;

    // 探测 key 是否存在
    [[nodiscard]] virtual bool exists(uint16_t key) const = 0;

    // 取 key 长度（字节），不存在返 0
    [[nodiscard]] virtual uint32_t size_of(uint16_t key) const = 0;
};

} // namespace handheld

#endif
