#ifndef CORE_GRAPHICS_FONT_H
#define CORE_GRAPHICS_FONT_H

#include "core/graphics/Geometry.h"

#include <array>
#include <cstdint>

namespace handheld {

namespace detail {

using GlyphBitmap = std::array<uint8_t, 5>;
using PackedGlyph3x5 = uint16_t;

static_assert(sizeof(PackedGlyph3x5) < sizeof(GlyphBitmap));

inline constexpr GlyphBitmap GLYPH_SPACE = {0x00, 0x00, 0x00, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_EXCLAMATION = {0x00, 0x00, 0x5F, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_QUOTE = {0x00, 0x03, 0x00, 0x03, 0x00};
inline constexpr GlyphBitmap GLYPH_APOSTROPHE = {0x00, 0x05, 0x03, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_LEFT_PAREN = {0x00, 0x1C, 0x22, 0x41, 0x00};
inline constexpr GlyphBitmap GLYPH_RIGHT_PAREN = {0x00, 0x41, 0x22, 0x1C, 0x00};
inline constexpr GlyphBitmap GLYPH_PLUS = {0x08, 0x08, 0x3E, 0x08, 0x08};
inline constexpr GlyphBitmap GLYPH_COMMA = {0x00, 0x50, 0x30, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_HYPHEN = {0x08, 0x08, 0x08, 0x08, 0x08};
inline constexpr GlyphBitmap GLYPH_PERIOD = {0x00, 0x60, 0x60, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_SLASH = {0x20, 0x10, 0x08, 0x04, 0x02};
inline constexpr GlyphBitmap GLYPH_COLON = {0x00, 0x36, 0x36, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_SEMICOLON = {0x00, 0x56, 0x36, 0x00, 0x00};
inline constexpr GlyphBitmap GLYPH_EQUALS = {0x14, 0x14, 0x14, 0x14, 0x14};
inline constexpr GlyphBitmap GLYPH_BACKSLASH = {0x02, 0x04, 0x08, 0x10, 0x20};
inline constexpr GlyphBitmap GLYPH_QUESTION = {0x02, 0x01, 0x51, 0x09, 0x06};
inline constexpr GlyphBitmap GLYPH_UNDERSCORE = {0x40, 0x40, 0x40, 0x40, 0x40};

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

[[nodiscard]] constexpr GlyphBitmap basic_glyph_5x7(char c) {
	switch (normalize_glyph_char(c)) {
	case ' ': return GLYPH_SPACE;
	case '!': return GLYPH_EXCLAMATION;
	case '"': return GLYPH_QUOTE;
	case '\'': return GLYPH_APOSTROPHE;
	case '(': return GLYPH_LEFT_PAREN;
	case ')': return GLYPH_RIGHT_PAREN;
	case '+': return GLYPH_PLUS;
	case ',': return GLYPH_COMMA;
	case '-': return GLYPH_HYPHEN;
	case '.': return GLYPH_PERIOD;
	case '/': return GLYPH_SLASH;
	case ':': return GLYPH_COLON;
	case ';': return GLYPH_SEMICOLON;
	case '=': return GLYPH_EQUALS;
	case '\\': return GLYPH_BACKSLASH;
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
	case '_': return GLYPH_UNDERSCORE;
	default: return GLYPH_QUESTION;
	}
}

[[nodiscard]] constexpr PackedGlyph3x5 pack_glyph_3x5(uint8_t column0, uint8_t column1, uint8_t column2) {
	return static_cast<PackedGlyph3x5>(
		(column0 & 0x1F)
		| ((column1 & 0x1F) << 5)
		| ((column2 & 0x1F) << 10));
}

[[nodiscard]] constexpr GlyphBitmap unpack_glyph_3x5(PackedGlyph3x5 packed) {
	const auto column0 = static_cast<uint8_t>(packed & 0x1F);
	const auto column1 = static_cast<uint8_t>((packed >> 5) & 0x1F);
	const auto column2 = static_cast<uint8_t>((packed >> 10) & 0x1F);
	return {column0, column1, column2, 0x00, 0x00};
}

inline constexpr PackedGlyph3x5 COMPACT_GLYPH_SPACE = pack_glyph_3x5(0x00, 0x00, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_EXCLAMATION = pack_glyph_3x5(0x00, 0x17, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_QUOTE = pack_glyph_3x5(0x03, 0x00, 0x03);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_APOSTROPHE = pack_glyph_3x5(0x00, 0x03, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_LEFT_PAREN = pack_glyph_3x5(0x00, 0x0E, 0x11);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_RIGHT_PAREN = pack_glyph_3x5(0x11, 0x0E, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_PLUS = pack_glyph_3x5(0x04, 0x0E, 0x04);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_COMMA = pack_glyph_3x5(0x00, 0x10, 0x08);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_HYPHEN = pack_glyph_3x5(0x04, 0x04, 0x04);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_PERIOD = pack_glyph_3x5(0x00, 0x10, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_SLASH = pack_glyph_3x5(0x18, 0x04, 0x03);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_COLON = pack_glyph_3x5(0x00, 0x0A, 0x00);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_SEMICOLON = pack_glyph_3x5(0x00, 0x14, 0x08);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_EQUALS = pack_glyph_3x5(0x0A, 0x0A, 0x0A);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_BACKSLASH = pack_glyph_3x5(0x03, 0x04, 0x18);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_QUESTION = pack_glyph_3x5(0x01, 0x15, 0x02);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_UNDERSCORE = pack_glyph_3x5(0x10, 0x10, 0x10);

inline constexpr PackedGlyph3x5 COMPACT_GLYPH_0 = pack_glyph_3x5(0x0E, 0x11, 0x0E);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_1 = pack_glyph_3x5(0x12, 0x1F, 0x10);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_2 = pack_glyph_3x5(0x19, 0x15, 0x12);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_3 = pack_glyph_3x5(0x11, 0x15, 0x0A);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_4 = pack_glyph_3x5(0x07, 0x04, 0x1F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_5 = pack_glyph_3x5(0x17, 0x15, 0x09);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_6 = pack_glyph_3x5(0x0E, 0x15, 0x08);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_7 = pack_glyph_3x5(0x01, 0x1D, 0x03);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_8 = pack_glyph_3x5(0x0A, 0x15, 0x0A);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_9 = pack_glyph_3x5(0x02, 0x15, 0x0E);

inline constexpr PackedGlyph3x5 COMPACT_GLYPH_A = pack_glyph_3x5(0x1E, 0x05, 0x1E);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_B = pack_glyph_3x5(0x1F, 0x15, 0x0A);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_C = pack_glyph_3x5(0x0E, 0x11, 0x11);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_D = pack_glyph_3x5(0x1F, 0x11, 0x0E);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_E = pack_glyph_3x5(0x1F, 0x15, 0x15);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_F = pack_glyph_3x5(0x1F, 0x05, 0x05);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_G = pack_glyph_3x5(0x0E, 0x11, 0x1D);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_H = pack_glyph_3x5(0x1F, 0x04, 0x1F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_I = pack_glyph_3x5(0x11, 0x1F, 0x11);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_J = pack_glyph_3x5(0x08, 0x10, 0x0F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_K = pack_glyph_3x5(0x1F, 0x04, 0x1B);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_L = pack_glyph_3x5(0x1F, 0x10, 0x10);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_M = pack_glyph_3x5(0x1F, 0x06, 0x1F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_N = pack_glyph_3x5(0x1F, 0x0E, 0x1F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_O = pack_glyph_3x5(0x0E, 0x11, 0x0E);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_P = pack_glyph_3x5(0x1F, 0x05, 0x02);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_Q = pack_glyph_3x5(0x0E, 0x19, 0x1E);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_R = pack_glyph_3x5(0x1F, 0x0D, 0x16);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_S = pack_glyph_3x5(0x12, 0x15, 0x09);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_T = pack_glyph_3x5(0x01, 0x1F, 0x01);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_U = pack_glyph_3x5(0x0F, 0x10, 0x0F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_V = pack_glyph_3x5(0x07, 0x18, 0x07);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_W = pack_glyph_3x5(0x1F, 0x0C, 0x1F);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_X = pack_glyph_3x5(0x1B, 0x04, 0x1B);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_Y = pack_glyph_3x5(0x03, 0x1C, 0x03);
inline constexpr PackedGlyph3x5 COMPACT_GLYPH_Z = pack_glyph_3x5(0x19, 0x15, 0x13);

[[nodiscard]] constexpr GlyphBitmap compact_glyph_3x5(char c) {
	switch (normalize_glyph_char(c)) {
	case ' ': return unpack_glyph_3x5(COMPACT_GLYPH_SPACE);
	case '!': return unpack_glyph_3x5(COMPACT_GLYPH_EXCLAMATION);
	case '"': return unpack_glyph_3x5(COMPACT_GLYPH_QUOTE);
	case '\'': return unpack_glyph_3x5(COMPACT_GLYPH_APOSTROPHE);
	case '(': return unpack_glyph_3x5(COMPACT_GLYPH_LEFT_PAREN);
	case ')': return unpack_glyph_3x5(COMPACT_GLYPH_RIGHT_PAREN);
	case '+': return unpack_glyph_3x5(COMPACT_GLYPH_PLUS);
	case ',': return unpack_glyph_3x5(COMPACT_GLYPH_COMMA);
	case '-': return unpack_glyph_3x5(COMPACT_GLYPH_HYPHEN);
	case '.': return unpack_glyph_3x5(COMPACT_GLYPH_PERIOD);
	case '/': return unpack_glyph_3x5(COMPACT_GLYPH_SLASH);
	case ':': return unpack_glyph_3x5(COMPACT_GLYPH_COLON);
	case ';': return unpack_glyph_3x5(COMPACT_GLYPH_SEMICOLON);
	case '=': return unpack_glyph_3x5(COMPACT_GLYPH_EQUALS);
	case '\\': return unpack_glyph_3x5(COMPACT_GLYPH_BACKSLASH);
	case '0': return unpack_glyph_3x5(COMPACT_GLYPH_0);
	case '1': return unpack_glyph_3x5(COMPACT_GLYPH_1);
	case '2': return unpack_glyph_3x5(COMPACT_GLYPH_2);
	case '3': return unpack_glyph_3x5(COMPACT_GLYPH_3);
	case '4': return unpack_glyph_3x5(COMPACT_GLYPH_4);
	case '5': return unpack_glyph_3x5(COMPACT_GLYPH_5);
	case '6': return unpack_glyph_3x5(COMPACT_GLYPH_6);
	case '7': return unpack_glyph_3x5(COMPACT_GLYPH_7);
	case '8': return unpack_glyph_3x5(COMPACT_GLYPH_8);
	case '9': return unpack_glyph_3x5(COMPACT_GLYPH_9);
	case 'A': return unpack_glyph_3x5(COMPACT_GLYPH_A);
	case 'B': return unpack_glyph_3x5(COMPACT_GLYPH_B);
	case 'C': return unpack_glyph_3x5(COMPACT_GLYPH_C);
	case 'D': return unpack_glyph_3x5(COMPACT_GLYPH_D);
	case 'E': return unpack_glyph_3x5(COMPACT_GLYPH_E);
	case 'F': return unpack_glyph_3x5(COMPACT_GLYPH_F);
	case 'G': return unpack_glyph_3x5(COMPACT_GLYPH_G);
	case 'H': return unpack_glyph_3x5(COMPACT_GLYPH_H);
	case 'I': return unpack_glyph_3x5(COMPACT_GLYPH_I);
	case 'J': return unpack_glyph_3x5(COMPACT_GLYPH_J);
	case 'K': return unpack_glyph_3x5(COMPACT_GLYPH_K);
	case 'L': return unpack_glyph_3x5(COMPACT_GLYPH_L);
	case 'M': return unpack_glyph_3x5(COMPACT_GLYPH_M);
	case 'N': return unpack_glyph_3x5(COMPACT_GLYPH_N);
	case 'O': return unpack_glyph_3x5(COMPACT_GLYPH_O);
	case 'P': return unpack_glyph_3x5(COMPACT_GLYPH_P);
	case 'Q': return unpack_glyph_3x5(COMPACT_GLYPH_Q);
	case 'R': return unpack_glyph_3x5(COMPACT_GLYPH_R);
	case 'S': return unpack_glyph_3x5(COMPACT_GLYPH_S);
	case 'T': return unpack_glyph_3x5(COMPACT_GLYPH_T);
	case 'U': return unpack_glyph_3x5(COMPACT_GLYPH_U);
	case 'V': return unpack_glyph_3x5(COMPACT_GLYPH_V);
	case 'W': return unpack_glyph_3x5(COMPACT_GLYPH_W);
	case 'X': return unpack_glyph_3x5(COMPACT_GLYPH_X);
	case 'Y': return unpack_glyph_3x5(COMPACT_GLYPH_Y);
	case 'Z': return unpack_glyph_3x5(COMPACT_GLYPH_Z);
	case '_': return unpack_glyph_3x5(COMPACT_GLYPH_UNDERSCORE);
	default: return unpack_glyph_3x5(COMPACT_GLYPH_QUESTION);
	}
}

} // namespace detail

struct BitmapFont {
	Size glyph_size = {5, 7};
	int16_t advance_x = 6;
	int16_t line_height = 8;
	using GlyphLookup = detail::GlyphBitmap (*)(char);
	GlyphLookup glyph_lookup = detail::basic_glyph_5x7;

	[[nodiscard]] constexpr detail::GlyphBitmap glyph_for(char c) const {
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
