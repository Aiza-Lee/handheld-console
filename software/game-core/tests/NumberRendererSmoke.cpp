#include "core/graphics/NumberRenderer.h"
#include "core/graphics/TextRenderer.h"
#include "tests/support/FakePlatform.h"

#include <cassert>
#include <cstring>

int main() {
    char buf[16];
    using handheld::itoa_dec;
    using handheld::itoa_hex;

    itoa_dec(int32_t{0}, buf);              assert(strcmp(buf, "0") == 0);
    itoa_dec(int32_t{1}, buf);              assert(strcmp(buf, "1") == 0);
    itoa_dec(int32_t{9999}, buf);           assert(strcmp(buf, "9999") == 0);
    itoa_dec(int32_t{10000}, buf);          assert(strcmp(buf, "10000") == 0);
    itoa_dec(int32_t{-1}, buf);             assert(strcmp(buf, "-1") == 0);
    itoa_dec(int32_t{-2147483647}, buf);    assert(strcmp(buf, "-2147483647") == 0);
    itoa_dec(int32_t{-2147483648}, buf);    assert(strcmp(buf, "-2147483648") == 0);
    itoa_dec(uint32_t{4294967295u}, buf);   assert(strcmp(buf, "4294967295") == 0);
    itoa_dec(uint16_t{65535}, buf);         assert(strcmp(buf, "65535") == 0);

    itoa_hex(0xABCDu, buf);                 assert(strcmp(buf, "abcd") == 0);
    itoa_hex(0xABu, buf, 4, true);          assert(strcmp(buf, "00ab") == 0);
    itoa_hex(0u, buf);                      assert(strcmp(buf, "0") == 0);

    handheld::FakePlatform platform;
    const uint32_t before = platform.fake_display().draw_pixel_count();
    handheld::TextRenderer::draw_int(platform.display(), 0, 0, -42, handheld::Color::WHITE);
    handheld::TextRenderer::draw_uint(platform.display(), 0, 0, 12345u, handheld::Color::WHITE,
                                      handheld::COMPACT_FONT_3X5);
    assert(platform.fake_display().draw_pixel_count() > before);
    return 0;
}
