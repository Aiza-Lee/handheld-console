#ifndef CORE_GRAPHICS_TEXT_RENDERER_H
#define CORE_GRAPHICS_TEXT_RENDERER_H

#include <algorithm>

#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "platform/interfaces/IDisplay.h"

namespace handheld {

class TextRenderer {
public:
	[[nodiscard]] static Size measure_text(const char* text,
		int16_t scale = 1,
		const BitmapFont& font = BASIC_FONT_5X7
	) {
		if (text == nullptr || *text == '\0' || scale <= 0) {
			return {};
		}

		int16_t max_width = 0;
		int16_t line_width = 0;
		int16_t line_count = 1;
		bool has_visible_glyph = false;

		for (const char* current = text; *current != '\0'; ++current) {
			if (*current == '\n') {
				max_width = std::max(line_width, max_width);
				line_width = 0;
				++line_count;
				continue;
			}

			has_visible_glyph = true;
			if (line_width == 0) {
				line_width = static_cast<int16_t>(font.glyph_size.width * scale);
			} else {
				line_width = static_cast<int16_t>(line_width + (font.advance_x * scale));
			}
		}

		max_width = std::max(line_width, max_width);

		if (!has_visible_glyph) {
			return {0, static_cast<int16_t>(font.glyph_size.height * scale)};
		}

		const auto height = static_cast<int16_t>(
			((line_count - 1) * font.line_height * scale) + (font.glyph_size.height * scale));
		return {max_width, height};
	}

	static void draw_text(IDisplay& display,
		Point origin,
		const char* text,
		Color color,
		int16_t scale = 1,
		const BitmapFont& font = BASIC_FONT_5X7
	) {
		if (text == nullptr || scale <= 0) {
			return;
		}

		Point cursor = origin;
		for (const char* current = text; *current != '\0'; ++current) {
			if (*current == '\n') {
				cursor.x = origin.x;
				cursor.y = static_cast<int16_t>(cursor.y + (font.line_height * scale));
				continue;
			}

			draw_glyph(display, cursor, *current, color, scale, font);
			cursor.x = static_cast<int16_t>(cursor.x + (font.advance_x * scale));
		}
	}

	static void draw_glyph(IDisplay& display,
		Point origin,
		char c,
		Color color,
		int16_t scale = 1,
		const BitmapFont& font = BASIC_FONT_5X7
	) {
		if (scale <= 0) {
			return;
		}

		const auto& glyph = font.glyph_for(c);
		for (int16_t x = 0; x < font.glyph_size.width; ++x) {
			const uint8_t column_bits = glyph[x];
			for (int16_t y = 0; y < font.glyph_size.height; ++y) {
				if ((column_bits & (1U << y)) == 0U) {
					continue;
				}

				const auto draw_x = static_cast<int16_t>(origin.x + (x * scale));
				const auto draw_y = static_cast<int16_t>(origin.y + (y * scale));
				if (scale == 1) {
					display.draw_pixel(draw_x, draw_y, color);
					continue;
				}

				display.fill_rect({draw_x, draw_y, scale, scale}, color);
			}
		}
	}
};

} // namespace handheld

#endif
