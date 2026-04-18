#ifndef CORE_GRAPHICS_FONT_H
#define CORE_GRAPHICS_FONT_H

#include "core/graphics/Geometry.h"

#include <array>
#include <cstdint>

namespace handheld {

namespace detail {

using GlyphBitmap = std::array<uint8_t, 5>;

inline constexpr GlyphBitmap GLYPH_SPACE = {0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_EXCLAMATION = {0x00, 0x00, 0x5F, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_HYPHEN = {0x08, 0x08, 0x08, 0x08, 0x08};
inline constexpr GlyphBitmap GLYPH_PERIOD = {0x00, 0x60, 0x60, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_SLASH = {0x20, 0x10, 0x08, 0x04, 0x02};
inline constexpr GlyphBitmap GLYPH_COLON = {0x00, 0x36, 0x36, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_QUESTION = {0x02, 0x01, 0x51, 0x09, 0x06};

inline constexpr GlyphBitmap GLYPH_0 = {0x3E, 0x51, 0x49, 0x45, 0x3E};
inline constexpr GlyphBitmap GLYPH_1 = {0x00, 0x42, 0x7F, 0x40, 0x00};
inline constexpr GlyphBitmap GLYPH_2 = {0x42, 0x61, 0x51, 0x49, 0x46};
inline constexpr GlyphBitmap GLYPH_3 = {0x21, 0x41, 0x45, 0x4B, 0x31};
inline constexpr GlyphBitmap GLYPH_4 = {0x18, 0x14, 0x12, 0x7F, 0x10};
inline constexpr GlyphBitmap GLYPH_5 = {0x27, 0x45, 0x45, 0x45, 0x39};
inline constexpr GlyphBitmap GLYPH_6 = {0x3C, 0x4A, 0x49, 0x49, 0x30};
inline constexpr GlyphBitmap GLYPH_7 = {0x01, 0x71, 0x09, 0x05, 0x03};
inline constexpr GlyphBitmap GLYPH_8 = {0x36, 0x49, 0x49, 0x49, 0x36};
inline constexpr GlyphBitmap GLYPH_9 = {0x06, 0x49, 0x49, 0x29, 0x1E};

inline constexpr GlyphBitmap GLYPH_A = {0x7E, 0x11, 0x11, 0x11, 0x7E};
inline constexpr GlyphBitmap GLYPH_B = {0x7F, 0x49, 0x49, 0x49, 0x36};
inline constexpr GlyphBitmap GLYPH_C = {0x3E, 0x41, 0x41, 0x41, 0x22};
inline constexpr GlyphBitmap GLYPH_D = {0x7F, 0x41, 0x41, 0x22, 0x1C};
inline constexpr GlyphBitmap GLYPH_E = {0x7F, 0x49, 0x49, 0x49, 0x41};
inline constexpr GlyphBitmap GLYPH_F = {0x7F, 0x09, 0x09, 0x09, 0x01};
inline constexpr GlyphBitmap GLYPH_G = {0x3E, 0x41, 0x49, 0x49, 0x7A};
inline constexpr GlyphBitmap GLYPH_H = {0x7F, 0x08, 0x08, 0x08, 0x7F};
inline constexpr GlyphBitmap GLYPH_I = {0x00, 0x41, 0x7F, 0x41, 0x00};
inline constexpr GlyphBitmap GLYPH_J = {0x20, 0x40, 0x41, 0x3F, 0x01};
inline constexpr GlyphBitmap GLYPH_K = {0x7F, 0x08, 0x14, 0x22, 0x41};
inline constexpr GlyphBitmap GLYPH_L = {0x7F, 0x40, 0x40, 0x40, 0x40};
inline constexpr GlyphBitmap GLYPH_M = {0x7F, 0x02, 0x0C, 0x02, 0x7F};
inline constexpr GlyphBitmap GLYPH_N = {0x7F, 0x04, 0x08, 0x10, 0x7F};
inline constexpr GlyphBitmap GLYPH_O = {0x3E, 0x41, 0x41, 0x41, 0x3E};
inline constexpr GlyphBitmap GLYPH_P = {0x7F, 0x09, 0x09, 0x09, 0x06};
inline constexpr GlyphBitmap GLYPH_Q = {0x3E, 0x41, 0x51, 0x21, 0x5E};
inline constexpr GlyphBitmap GLYPH_R = {0x7F, 0x09, 0x19, 0x29, 0x46};
inline constexpr GlyphBitmap GLYPH_S = {0x46, 0x49, 0x49, 0x49, 0x31};
inline constexpr GlyphBitmap GLYPH_T = {0x01, 0x01, 0x7F, 0x01, 0x01};
inline constexpr GlyphBitmap GLYPH_U = {0x3F, 0x40, 0x40, 0x40, 0x3F};
inline constexpr GlyphBitmap GLYPH_V = {0x1F, 0x20, 0x40, 0x20, 0x1F};
inline constexpr GlyphBitmap GLYPH_W = {0x3F, 0x40, 0x38, 0x40, 0x3F};
inline constexpr GlyphBitmap GLYPH_X = {0x63, 0x14, 0x08, 0x14, 0x63};
inline constexpr GlyphBitmap GLYPH_Y = {0x07, 0x08, 0x70, 0x08, 0x07};
inline constexpr GlyphBitmap GLYPH_Z = {0x61, 0x51, 0x49, 0x45, 0x43};

[[nodiscard]] constexpr char normalize_glyph_char(char c) {
	if (c >= 'a' && c <= 'z') {
		return static_cast<char>(c - ('a' - 'A'));
	}

	return c;
}

[[nodiscard]] constexpr const GlyphBitmap& basic_glyph_5x7(char c) {
	switch (normalize_glyph_char(c)) {
	case ' ': return GLYPH_SPACE;
	case '!': return GLYPH_EXCLAMATION;
	case '-': return GLYPH_HYPHEN;
	case '.': return GLYPH_PERIOD;
	case '/': return GLYPH_SLASH;
	case ':': return GLYPH_COLON;
	case '0': return GLYPH_0;
	case '1': return GLYPH_1;
	case '2': return GLYPH_2;
	case '3': return GLYPH_3;
	case '4': return GLYPH_4;
	case '5': return GLYPH_5;
	case '6': return GLYPH_6;
	case '7': return GLYPH_7;
	case '8': return GLYPH_8;
	case '9': return GLYPH_9;
	case 'A': return GLYPH_A;
	case 'B': return GLYPH_B;
	case 'C': return GLYPH_C;
	case 'D': return GLYPH_D;
	case 'E': return GLYPH_E;
	case 'F': return GLYPH_F;
	case 'G': return GLYPH_G;
	case 'H': return GLYPH_H;
	case 'I': return GLYPH_I;
	case 'J': return GLYPH_J;
	case 'K': return GLYPH_K;
	case 'L': return GLYPH_L;
	case 'M': return GLYPH_M;
	case 'N': return GLYPH_N;
	case 'O': return GLYPH_O;
	case 'P': return GLYPH_P;
	case 'Q': return GLYPH_Q;
	case 'R': return GLYPH_R;
	case 'S': return GLYPH_S;
	case 'T': return GLYPH_T;
	case 'U': return GLYPH_U;
	case 'V': return GLYPH_V;
	case 'W': return GLYPH_W;
	case 'X': return GLYPH_X;
	case 'Y': return GLYPH_Y;
	case 'Z': return GLYPH_Z;
	default: return GLYPH_QUESTION;
	}
}

[[nodiscard]] constexpr GlyphBitmap make_glyph_3x5(uint8_t column0, uint8_t column1, uint8_t column2) {
	return {column0, column1, column2, 0x00, 0x00};
}

inline constexpr GlyphBitmap COMPACT_GLYPH_SPACE = make_glyph_3x5(0x00, 0x00, 0x00);
inline constexpr GlyphBitmap COMPACT_GLYPH_EXCLAMATION = make_glyph_3x5(0x00, 0x17, 0x00);
inline constexpr GlyphBitmap COMPACT_GLYPH_HYPHEN = make_glyph_3x5(0x04, 0x04, 0x04);
inline constexpr GlyphBitmap COMPACT_GLYPH_PERIOD = make_glyph_3x5(0x00, 0x10, 0x00);
inline constexpr GlyphBitmap COMPACT_GLYPH_SLASH = make_glyph_3x5(0x18, 0x04, 0x03);
inline constexpr GlyphBitmap COMPACT_GLYPH_COLON = make_glyph_3x5(0x00, 0x0A, 0x00);
inline constexpr GlyphBitmap COMPACT_GLYPH_QUESTION = make_glyph_3x5(0x01, 0x15, 0x02);

inline constexpr GlyphBitmap COMPACT_GLYPH_0 = make_glyph_3x5(0x0E, 0x11, 0x0E);
inline constexpr GlyphBitmap COMPACT_GLYPH_1 = make_glyph_3x5(0x12, 0x1F, 0x10);
inline constexpr GlyphBitmap COMPACT_GLYPH_2 = make_glyph_3x5(0x19, 0x15, 0x12);
inline constexpr GlyphBitmap COMPACT_GLYPH_3 = make_glyph_3x5(0x11, 0x15, 0x0A);
inline constexpr GlyphBitmap COMPACT_GLYPH_4 = make_glyph_3x5(0x07, 0x04, 0x1F);
inline constexpr GlyphBitmap COMPACT_GLYPH_5 = make_glyph_3x5(0x17, 0x15, 0x09);
inline constexpr GlyphBitmap COMPACT_GLYPH_6 = make_glyph_3x5(0x0E, 0x15, 0x08);
inline constexpr GlyphBitmap COMPACT_GLYPH_7 = make_glyph_3x5(0x01, 0x1D, 0x03);
inline constexpr GlyphBitmap COMPACT_GLYPH_8 = make_glyph_3x5(0x0A, 0x15, 0x0A);
inline constexpr GlyphBitmap COMPACT_GLYPH_9 = make_glyph_3x5(0x02, 0x15, 0x0E);

inline constexpr GlyphBitmap COMPACT_GLYPH_A = make_glyph_3x5(0x1E, 0x05, 0x1E);
inline constexpr GlyphBitmap COMPACT_GLYPH_B = make_glyph_3x5(0x1F, 0x15, 0x0A);
inline constexpr GlyphBitmap COMPACT_GLYPH_C = make_glyph_3x5(0x0E, 0x11, 0x11);
inline constexpr GlyphBitmap COMPACT_GLYPH_D = make_glyph_3x5(0x1F, 0x11, 0x0E);
inline constexpr GlyphBitmap COMPACT_GLYPH_E = make_glyph_3x5(0x1F, 0x15, 0x15);
inline constexpr GlyphBitmap COMPACT_GLYPH_F = make_glyph_3x5(0x1F, 0x05, 0x05);
inline constexpr GlyphBitmap COMPACT_GLYPH_G = make_glyph_3x5(0x0E, 0x11, 0x1D);
inline constexpr GlyphBitmap COMPACT_GLYPH_H = make_glyph_3x5(0x1F, 0x04, 0x1F);
inline constexpr GlyphBitmap COMPACT_GLYPH_I = make_glyph_3x5(0x11, 0x1F, 0x11);
inline constexpr GlyphBitmap COMPACT_GLYPH_J = make_glyph_3x5(0x08, 0x10, 0x0F);
inline constexpr GlyphBitmap COMPACT_GLYPH_K = make_glyph_3x5(0x1F, 0x04, 0x1B);
inline constexpr GlyphBitmap COMPACT_GLYPH_L = make_glyph_3x5(0x1F, 0x10, 0x10);
inline constexpr GlyphBitmap COMPACT_GLYPH_M = make_glyph_3x5(0x1F, 0x06, 0x1F);
inline constexpr GlyphBitmap COMPACT_GLYPH_N = make_glyph_3x5(0x1F, 0x0E, 0x1F);
inline constexpr GlyphBitmap COMPACT_GLYPH_O = make_glyph_3x5(0x0E, 0x11, 0x0E);
inline constexpr GlyphBitmap COMPACT_GLYPH_P = make_glyph_3x5(0x1F, 0x05, 0x02);
inline constexpr GlyphBitmap COMPACT_GLYPH_Q = make_glyph_3x5(0x0E, 0x19, 0x1E);
inline constexpr GlyphBitmap COMPACT_GLYPH_R = make_glyph_3x5(0x1F, 0x0D, 0x16);
inline constexpr GlyphBitmap COMPACT_GLYPH_S = make_glyph_3x5(0x12, 0x15, 0x09);
inline constexpr GlyphBitmap COMPACT_GLYPH_T = make_glyph_3x5(0x01, 0x1F, 0x01);
inline constexpr GlyphBitmap COMPACT_GLYPH_U = make_glyph_3x5(0x0F, 0x10, 0x0F);
inline constexpr GlyphBitmap COMPACT_GLYPH_V = make_glyph_3x5(0x07, 0x18, 0x07);
inline constexpr GlyphBitmap COMPACT_GLYPH_W = make_glyph_3x5(0x1F, 0x0C, 0x1F);
inline constexpr GlyphBitmap COMPACT_GLYPH_X = make_glyph_3x5(0x1B, 0x04, 0x1B);
inline constexpr GlyphBitmap COMPACT_GLYPH_Y = make_glyph_3x5(0x03, 0x1C, 0x03);
inline constexpr GlyphBitmap COMPACT_GLYPH_Z = make_glyph_3x5(0x19, 0x15, 0x13);

[[nodiscard]] constexpr const GlyphBitmap& compact_glyph_3x5(char c) {
	switch (normalize_glyph_char(c)) {
	case ' ': return COMPACT_GLYPH_SPACE;
	case '!': return COMPACT_GLYPH_EXCLAMATION;
	case '-': return COMPACT_GLYPH_HYPHEN;
	case '.': return COMPACT_GLYPH_PERIOD;
	case '/': return COMPACT_GLYPH_SLASH;
	case ':': return COMPACT_GLYPH_COLON;
	case '0': return COMPACT_GLYPH_0;
	case '1': return COMPACT_GLYPH_1;
	case '2': return COMPACT_GLYPH_2;
	case '3': return COMPACT_GLYPH_3;
	case '4': return COMPACT_GLYPH_4;
	case '5': return COMPACT_GLYPH_5;
	case '6': return COMPACT_GLYPH_6;
	case '7': return COMPACT_GLYPH_7;
	case '8': return COMPACT_GLYPH_8;
	case '9': return COMPACT_GLYPH_9;
	case 'A': return COMPACT_GLYPH_A;
	case 'B': return COMPACT_GLYPH_B;
	case 'C': return COMPACT_GLYPH_C;
	case 'D': return COMPACT_GLYPH_D;
	case 'E': return COMPACT_GLYPH_E;
	case 'F': return COMPACT_GLYPH_F;
	case 'G': return COMPACT_GLYPH_G;
	case 'H': return COMPACT_GLYPH_H;
	case 'I': return COMPACT_GLYPH_I;
	case 'J': return COMPACT_GLYPH_J;
	case 'K': return COMPACT_GLYPH_K;
	case 'L': return COMPACT_GLYPH_L;
	case 'M': return COMPACT_GLYPH_M;
	case 'N': return COMPACT_GLYPH_N;
	case 'O': return COMPACT_GLYPH_O;
	case 'P': return COMPACT_GLYPH_P;
	case 'Q': return COMPACT_GLYPH_Q;
	case 'R': return COMPACT_GLYPH_R;
	case 'S': return COMPACT_GLYPH_S;
	case 'T': return COMPACT_GLYPH_T;
	case 'U': return COMPACT_GLYPH_U;
	case 'V': return COMPACT_GLYPH_V;
	case 'W': return COMPACT_GLYPH_W;
	case 'X': return COMPACT_GLYPH_X;
	case 'Y': return COMPACT_GLYPH_Y;
	case 'Z': return COMPACT_GLYPH_Z;
	default: return COMPACT_GLYPH_QUESTION;
	}
}

} // namespace detail

struct BitmapFont {
	Size glyph_size = {5, 7};
	int16_t advance_x = 6;
	int16_t line_height = 8;
	using GlyphLookup = const detail::GlyphBitmap& (*)(char);
	GlyphLookup glyph_lookup = detail::basic_glyph_5x7;

	[[nodiscard]] constexpr const detail::GlyphBitmap& glyph_for(char c) const {
		return glyph_lookup(c);
	}
};

inline constexpr BitmapFont BASIC_FONT_5X7{};
inline constexpr BitmapFont COMPACT_FONT_3X5 = {
	{3, 5},
	4,
	6,
	detail::compact_glyph_3x5,
};

} // namespace handheld

#endif
