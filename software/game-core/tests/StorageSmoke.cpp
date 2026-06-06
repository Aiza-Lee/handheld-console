#include "core/persistence/StorageKeys.h"
#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"
#include "core/runtime/ScreenType.h"
#include "core/graphics/Color.h"
#include "platform/interfaces/IStorage.h"
#include "scenes/settings/SettingsScreen.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <cstdint>

int main() {
    using handheld::IStorage;

    // 测试 1: read/write/exists/erase 基础往返
    {
        handheld::FakePlatform platform;
        auto& s = platform.storage();

        uint8_t v = 42;
        assert(s.write(0x0001, &v, sizeof(v)) == IStorage::Status::OK);
        assert(s.exists(0x0001));
        assert(s.size_of(0x0001) == 1);

        v = 0;
        assert(s.read(0x0001, &v, sizeof(v)) == IStorage::Status::OK);
        assert(v == 42);

        assert(s.erase(0x0001) == IStorage::Status::OK);
        assert(!s.exists(0x0001));
    }

    // 测试 2: NOT_FOUND / INVALID_SIZE 区分
    {
        handheld::FakePlatform platform;
        auto& s = platform.storage();

        uint8_t buf = 0;
        assert(s.read(0x0999, &buf, sizeof(buf)) == IStorage::Status::NOT_FOUND);
        assert(!s.exists(0x0999));
        assert(s.size_of(0x0999) == 0);

        s.write(0x0001, "AB", 2);
        assert(s.read(0x0001, &buf, sizeof(buf)) == IStorage::Status::INVALID_SIZE);
        assert(s.size_of(0x0001) == 2);

        // read 失败时不应污染 buf
        assert(buf == 0);
    }

    // 测试 3: commit 幂等 + write_count 计数
    {
        handheld::FakePlatform platform;
        auto& s = platform.storage();

        assert(s.commit() == IStorage::Status::OK);
        assert(platform.fake_storage().write_count() == 0);

        uint8_t v = 7;
        s.write(0x0001, &v, sizeof(v));
        s.write(0x0002, &v, sizeof(v));
        s.write(0x0003, &v, sizeof(v));
        assert(platform.fake_storage().write_count() == 3);

        s.commit();   // 多次 commit 不应影响计数
        s.commit();
        assert(platform.fake_storage().write_count() == 3);
    }

    // 测试 4: seed_defaults 钩子 —— 模拟"开机读到 flash 已有值"
    {
        handheld::FakePlatform platform;
        auto& fs = platform.fake_storage();

        uint8_t seed = 88;
        fs.set_default(0x0001, &seed, sizeof(seed));
        fs.seed_defaults();

        uint8_t v = 0;
        assert(platform.storage().read(0x0001, &v, sizeof(v)) == IStorage::Status::OK);
        assert(v == 88);
    }

    // 测试 5: 状态跨 runner.tick() 持久（单平台实例内不丢）
    {
        handheld::DefaultScreenFactory factory;
        handheld::FakePlatform platform;
        handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::MENU);

        auto& s = platform.storage();
        uint8_t v = 55;
        s.write(0x0042, &v, sizeof(v));
        assert(platform.fake_storage().write_count() == 1);

        // 跑 50 帧
        for (int i = 0; i < 50; ++i) {
            runner.tick();
        }

        // 键仍然存在
        v = 0;
        assert(s.read(0x0042, &v, sizeof(v)) == IStorage::Status::OK);
        assert(v == 55);
        // 50 帧内不应触发额外 write（菜单场景不写 storage）
        assert(platform.fake_storage().write_count() == 1);
    }

    // 测试 6: SettingsScreen 集成 —— 改音量 → B 退出 → 键值落库
    {
        handheld::DefaultScreenFactory factory;
        handheld::FakePlatform platform;
        handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::SETTINGS);

        // 跑一帧让 enter() 完成（无存储数据，会保留默认 5/10）
        runner.tick();

        auto& audio = runner.audio();
        audio.set_bgm_volume(35);
        audio.set_sfx_volume(70);

        // 模拟按 B —— 会触发 write + commit + switch_to(MENU)
        platform.fake_input().set_button(handheld::ButtonBits::B, true);
        runner.tick();
        platform.fake_input().set_button(handheld::ButtonBits::B, false);

        // 验证存储里写入了 35 / 70
        auto& s = platform.storage();
        uint8_t bgm = 0, sfx = 0;
        assert(s.read(handheld::storage::KEY_BGM_VOLUME, &bgm, sizeof(bgm)) == IStorage::Status::OK);
        assert(s.read(handheld::storage::KEY_SFX_VOLUME, &sfx, sizeof(sfx)) == IStorage::Status::OK);
        assert(bgm == 35);
        assert(sfx == 70);
    }

    // 测试 7: SettingsScreen 集成 —— 模拟"开机时 storage 已有值" → enter() 恢复音量
    {
        handheld::DefaultScreenFactory factory;
        handheld::FakePlatform platform;
        auto& fs = platform.fake_storage();

        // 预置 flash 已有 60 / 25
        uint8_t seed_bgm = 60, seed_sfx = 25;
        fs.set_default(handheld::storage::KEY_BGM_VOLUME, &seed_bgm, sizeof(seed_bgm));
        fs.set_default(handheld::storage::KEY_SFX_VOLUME, &seed_sfx, sizeof(seed_sfx));
        fs.seed_defaults();

        handheld::ScreenRunner runner(platform, factory, handheld::ScreenType::SETTINGS);
        runner.tick();   // enter() 触发 read + 恢复音量

        // 验证音量被恢复（不是 AudioEngine 默认的 5/10）
        assert(runner.audio().bgm_volume() == 60);
        assert(runner.audio().sfx_volume() == 25);
    }

    return 0;
}
