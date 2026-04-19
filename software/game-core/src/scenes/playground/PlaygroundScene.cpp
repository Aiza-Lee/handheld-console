#include "scenes/Playground/PlaygroundScene.h"
#include "core/common/ButtonBits.h"
#include "core/graphics/Color.h"
#include "core/graphics/Font.h"
#include "core/graphics/TextRenderer.h"
#include "core/runtime/ISceneHost.h"
#include "core/runtime/SceneType.h"
#include <cmath>

namespace handheld {

namespace {

constexpr Color BACKDROP = rgb565(8, 12, 30);
constexpr Color PRIMARY = rgb565(120, 200, 220);
constexpr Color SECONDARY = rgb565(90, 120, 200);
constexpr Color HIGHLIGHT = rgb565(210, 180, 235);
constexpr Color ALERT = rgb565(255, 145, 190);
constexpr Color MIST = rgb565(35, 45, 82);
constexpr Color SOFT_WHITE = rgb565(220, 235, 255);

int16_t _clamp_i16(int16_t value, int16_t low, int16_t high) {
	if (value < low) {
		return low;
	}
	if (value > high) {
		return high;
	}
	return value;
}

const char* _mode_name(uint8_t mode) {
	switch (mode) {
	case 0:
		return "GLASS GARDEN";
	case 1:
		return "LUNAR WALTZ";
	case 2:
		return "DREAM TIDE";
	case 3:
		return "SILK WAVE";
	case 4:
		return "AURORA BLOOM";
	default:
		return "UNKNOWN";
	}
}

} // namespace

void PlaygroundScene::enter(IPlatform& platform, ISceneHost& /*host*/) {
	IDisplay& display = platform.display();
	_x = static_cast<int16_t>(display.width() / 2);
	_y = static_cast<int16_t>(display.height() / 2);
	_mode = 0;
	_frame = 0;
	_accent_phase = 0;
	_stars_ready = false;
	display.clear(BACKDROP);
}

void PlaygroundScene::update(IPlatform& platform, ISceneHost& host) {
	++_frame;
	_accent_phase = static_cast<uint8_t>((_accent_phase + 1U) & 0x0FU);

	auto& input = platform.input();

	if (input.was_pressed(ButtonBits::SELECT)) {
		host.switch_to(SceneType::MENU);
		return;
	}

	if (input.was_pressed(ButtonBits::START)) {
		_mode = static_cast<uint8_t>((_mode + 1) % MODE_COUNT);
	}

	if (input.was_pressed(ButtonBits::A)) {
		_auto_move = !_auto_move;
	}

	if (input.was_pressed(ButtonBits::B)) {
		_clear_each_frame = !_clear_each_frame;
	}

	const int16_t nudge = input.is_down(ButtonBits::A) ? 2 : 1;

	if (input.is_down(ButtonBits::UP)) {
		_y = static_cast<int16_t>(_y - nudge);
	}
	if (input.is_down(ButtonBits::DOWN)) {
		_y = static_cast<int16_t>(_y + nudge);
	}
	if (input.is_down(ButtonBits::LEFT)) {
		_x = static_cast<int16_t>(_x - nudge);
	}
	if (input.is_down(ButtonBits::RIGHT)) {
		_x = static_cast<int16_t>(_x + nudge);
	}

	IDisplay& display = platform.display();
	_x = _clamp_i16(_x, 2, static_cast<int16_t>(display.width() - 3));
	_y = _clamp_i16(_y, 2, static_cast<int16_t>(display.height() - 3));

	if (_auto_move) {
		const double t = static_cast<double>(_frame) * 0.042;
		const auto cx = static_cast<int16_t>(display.width() / 2);
		const auto cy = static_cast<int16_t>(display.height() / 2);
		const auto rx = static_cast<int16_t>((display.width() / 2) - 10);
		const auto ry = static_cast<int16_t>((display.height() / 2) - 16);
		_x = static_cast<int16_t>(cx + (std::cos(t) * rx));
		_y = static_cast<int16_t>(cy + (std::sin(t * 1.37) * ry));
	}

	if (_stars_ready) {
		update_starfield(display);
	}
}

void PlaygroundScene::render(IPlatform& platform, ISceneHost& /*host*/) {
	IDisplay& display = platform.display();
	if (!_stars_ready) {
		init_stars(display);
	}

	if (_clear_each_frame) {
		display.clear(BACKDROP);
	}

	render_background(display);

	switch (_mode) {
	case 0:
		render_mode_grid_tunnel(display);
		break;
	case 1:
		render_mode_orbit_lab(display);
		break;
	case 2:
		render_mode_starflow(display);
		break;
	case 3:
		render_mode_signal_scope(display);
		break;
	case 4:
		render_mode_aurora_bloom(display);
		break;
	default:
		break;
	}
	render_hud(display);
	display.draw_rect(Rect{1, 1, static_cast<int16_t>(display.width() - 2), static_cast<int16_t>(display.height() - 2)}, MIST);
}

Rect PlaygroundScene::stage_rect(const IDisplay& display) const {
	const int16_t w = display.width();
	const int16_t h = display.height();
	const auto margin_x = static_cast<int16_t>(w < 180 ? 6 : 10);
	const int16_t top = 16;
	const int16_t bottom_reserved = 24;
	return {
		margin_x,
		top,
		static_cast<int16_t>(w - (margin_x * 2)),
		static_cast<int16_t>(h - top - bottom_reserved),
	};
}

Point PlaygroundScene::stage_center(const IDisplay& display) const {
	const Rect stage = stage_rect(display);
	return {
		static_cast<int16_t>(stage.x + (stage.width / 2)),
		static_cast<int16_t>(stage.y + (stage.height / 2)),
	};
}

void PlaygroundScene::render_stage_shell(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	display.draw_rect(stage, MIST);

	const int16_t inner_pad = 3;
	display.draw_rect(
		Rect{
			static_cast<int16_t>(stage.x + inner_pad),
			static_cast<int16_t>(stage.y + inner_pad),
			static_cast<int16_t>(stage.width - (inner_pad * 2)),
			static_cast<int16_t>(stage.height - (inner_pad * 2)),
		},
		SECONDARY);

	const int16_t wing = 10;
	display.draw_h_line(stage.x, stage.y, wing, HIGHLIGHT);
	display.draw_h_line(static_cast<int16_t>(stage.x + stage.width - wing), stage.y, wing, HIGHLIGHT);
	display.draw_h_line(stage.x, static_cast<int16_t>(stage.y + stage.height - 1), wing, HIGHLIGHT);
	display.draw_h_line(
		static_cast<int16_t>(stage.x + stage.width - wing),
		static_cast<int16_t>(stage.y + stage.height - 1),
		wing,
		HIGHLIGHT);
}

void PlaygroundScene::init_stars(IDisplay& display) {
	const Rect stage = stage_rect(display);
	const int16_t w = stage.width;
	const int16_t h = stage.height;

	for (size_t i = 0; i < _stars.size(); ++i) {
		const uint32_t a = hash_u32(static_cast<uint32_t>((i * 17) + 1));
		const uint32_t b = hash_u32(static_cast<uint32_t>((i * 29) + 7));
		const uint32_t c = hash_u32(static_cast<uint32_t>((i * 43) + 11));
		const uint32_t d = hash_u32(static_cast<uint32_t>((i * 71) + 19));

		_stars[i].x = static_cast<int16_t>(stage.x + (a % static_cast<uint32_t>(w)));
		_stars[i].y = static_cast<int16_t>(stage.y + (b % static_cast<uint32_t>(h)));
		_stars[i].speed = static_cast<uint8_t>(1 + (c % 4));
		_stars[i].layer = static_cast<uint8_t>(d % 3);
	}

	_stars_ready = true;
}

void PlaygroundScene::update_starfield(IDisplay& display) {
	const Rect stage = stage_rect(display);
	const int16_t w = stage.width;
	const int16_t h = stage.height;

	for (size_t i = 0; i < _stars.size(); ++i) {
		Star& s = _stars[i];
		const auto vx = static_cast<int16_t>((s.layer == 2U) ? 1 : 0);
		const auto vy = static_cast<int16_t>(_auto_move ? s.speed : 0);
		s.x = static_cast<int16_t>(s.x + vx);
		s.y = static_cast<int16_t>(s.y + vy);

		if (s.y >= stage.bottom()) {
			s.y = stage.y;
			s.x = static_cast<int16_t>(
				stage.x + (hash_u32(_frame + static_cast<uint32_t>(i * 31)) % static_cast<uint32_t>(w)));
		}

		if (s.x >= stage.right()) {
			s.x = stage.x;
		}
	}
}

void PlaygroundScene::render_background(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const int16_t stage_right = stage.right();
	const int16_t stage_bottom = stage.bottom();

	for (auto y = static_cast<int16_t>(stage.y + 8); y < stage_bottom; y += 18) {
		int16_t previous_x = stage.x;
		int16_t previous_y = y;
		for (auto x = static_cast<int16_t>(stage.x + 3); x < stage_right; x += 6) {
			const double t = (static_cast<double>(x) * 0.075) + (static_cast<double>(_frame) * 0.03);
			const auto ny = static_cast<int16_t>(y + (std::sin(t) * 4.0));
			display.draw_line(previous_x, previous_y, x, ny, (y % 36 == 14) ? MIST : SECONDARY);
			previous_x = x;
			previous_y = ny;
		}
	}

	for (auto s : _stars) {
			const Color star_color = (s.layer == 0U) ? MIST : SOFT_WHITE;
		display.draw_pixel(s.x, s.y, star_color);
		if (s.layer == 2U && s.x > stage.x) {
			display.draw_pixel(static_cast<int16_t>(s.x - 1), s.y, MIST);
		}
	}
}

void PlaygroundScene::render_mode_grid_tunnel(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const Point center = {
		_clamp_i16(_x, static_cast<int16_t>(stage.x + 3), static_cast<int16_t>(stage.x + stage.width - 4)),
		_clamp_i16(_y, static_cast<int16_t>(stage.y + 3), static_cast<int16_t>(stage.y + stage.height - 4)),
	};
	render_stage_shell(display);

	for (int16_t x = stage.x; x < static_cast<int16_t>(stage.x + stage.width); x += 26) {
		display.draw_line(center.x, center.y, x, stage.y, MIST);
		display.draw_line(center.x, center.y, x, static_cast<int16_t>(stage.y + stage.height - 1), MIST);
	}

	for (int16_t y = stage.y; y < static_cast<int16_t>(stage.y + stage.height); y += 18) {
		display.draw_line(center.x, center.y, stage.x, y, MIST);
		display.draw_line(center.x, center.y, static_cast<int16_t>(stage.x + stage.width - 1), y, MIST);
	}

	for (int16_t layer = 1; layer <= 4; ++layer) {
		const auto pulse = static_cast<int16_t>((_frame + static_cast<uint32_t>(layer * 11)) % 24);
		const auto inset = static_cast<int16_t>((layer * 11) + pulse);
		if (inset >= (stage.width / 2) || inset >= (stage.height / 2)) {
			continue;
		}
		display.draw_rect(
			Rect{
				static_cast<int16_t>(stage.x + inset),
				static_cast<int16_t>(stage.y + inset),
				static_cast<int16_t>(stage.width - (inset * 2)),
				static_cast<int16_t>(stage.height - (inset * 2)),
			},
			(layer % 2 == 0) ? SECONDARY : HIGHLIGHT);
	}

	draw_crosshair(display, center, 6, HIGHLIGHT);
}

void PlaygroundScene::render_mode_orbit_lab(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const Point center = stage_center(display);
	render_stage_shell(display);

	for (int16_t r = 12; r < static_cast<int16_t>((stage.height / 2) - 8); r += 10) {
		display.draw_rect(
			Rect{static_cast<int16_t>(center.x - r), static_cast<int16_t>(center.y - r), static_cast<int16_t>(r * 2), static_cast<int16_t>(r * 2)},
			(r % 20 == 0) ? PRIMARY : MIST);
	}

	const double t = static_cast<double>(_frame) * 0.058;
	const Point sat_a = {
		static_cast<int16_t>(center.x + (std::cos(t) * 34.0)),
		static_cast<int16_t>(center.y + (std::sin(t) * 20.0))
	};
	const Point sat_b = {
		static_cast<int16_t>(center.x + (std::cos(-t * 1.6) * 22.0)),
		static_cast<int16_t>(center.y + (std::sin(-t * 1.6) * 12.0))
	};
	const Point sat_c = {
		static_cast<int16_t>(center.x + (std::cos(t * 0.7) * 14.0)),
		static_cast<int16_t>(center.y + (std::sin(t * 0.7) * 28.0))
	};

	display.draw_line(center.x, center.y, sat_a.x, sat_a.y, SECONDARY);
	display.draw_line(center.x, center.y, sat_b.x, sat_b.y, SECONDARY);
	display.draw_line(center.x, center.y, sat_c.x, sat_c.y, SECONDARY);
	display.draw_line(sat_a.x, sat_a.y, sat_b.x, sat_b.y, HIGHLIGHT);
	display.draw_line(sat_b.x, sat_b.y, sat_c.x, sat_c.y, HIGHLIGHT);
	display.draw_line(sat_c.x, sat_c.y, sat_a.x, sat_a.y, HIGHLIGHT);

	display.fill_rect(Rect{static_cast<int16_t>(center.x - 2), static_cast<int16_t>(center.y - 2), 5, 5}, ALERT);
	display.fill_rect(Rect{static_cast<int16_t>(sat_a.x - 1), static_cast<int16_t>(sat_a.y - 1), 3, 3}, SOFT_WHITE);
	display.fill_rect(Rect{static_cast<int16_t>(sat_b.x - 1), static_cast<int16_t>(sat_b.y - 1), 3, 3}, SOFT_WHITE);
	display.fill_rect(Rect{static_cast<int16_t>(sat_c.x - 1), static_cast<int16_t>(sat_c.y - 1), 3, 3}, SOFT_WHITE);

	draw_crosshair(display, {_x, _y}, 5, HIGHLIGHT);
}

void PlaygroundScene::render_mode_starflow(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const int16_t w = display.width();
	const auto horizon = static_cast<int16_t>(stage.y + stage.height - 15);
	render_stage_shell(display);

	for (int16_t x = stage.x; x < static_cast<int16_t>(stage.x + stage.width); x += 4) {
		const double wave = std::sin((static_cast<double>(x) * 0.12) + (static_cast<double>(_frame) * 0.09));
		const auto y = static_cast<int16_t>(horizon + (wave * 3.0));
		display.draw_pixel(x, y, MIST);
	}

	for (auto lane = static_cast<int16_t>(stage.x + 6); lane < static_cast<int16_t>(stage.x + stage.width); lane += 28) {
		display.draw_line(stage_center(display).x, static_cast<int16_t>(stage.y + stage.height - 1), lane, horizon, SECONDARY);
	}

	const auto ship_y = static_cast<int16_t>(stage.y + stage.height - 22);
	const int16_t ship_x = _clamp_i16(_x, static_cast<int16_t>(stage.x + 8), static_cast<int16_t>(stage.x + stage.width - 9));
	display.draw_line(static_cast<int16_t>(ship_x - 8), ship_y, ship_x, static_cast<int16_t>(ship_y - 6), HIGHLIGHT);
	display.draw_line(ship_x, static_cast<int16_t>(ship_y - 6), static_cast<int16_t>(ship_x + 8), ship_y, HIGHLIGHT);
	display.draw_line(static_cast<int16_t>(ship_x - 8), ship_y, static_cast<int16_t>(ship_x + 8), ship_y, HIGHLIGHT);
	display.draw_line(ship_x, static_cast<int16_t>(ship_y - 6), ship_x, static_cast<int16_t>(ship_y + 2), SOFT_WHITE);

	const auto meteor_x = static_cast<int16_t>(stage.x + stage.width - 1 - ((_frame * 2U) % static_cast<uint32_t>(stage.width + 16)));
	const auto meteor_y = static_cast<int16_t>(stage.y + 8 + ((_frame / 2U) % 20U));
	if (meteor_x >= stage.x && meteor_x < static_cast<int16_t>(stage.x + stage.width)) {
		display.draw_line(meteor_x, meteor_y, static_cast<int16_t>(meteor_x + 6), static_cast<int16_t>(meteor_y - 2), ALERT);
		display.draw_pixel(meteor_x, meteor_y, SOFT_WHITE);
	}
}

void PlaygroundScene::render_mode_signal_scope(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const auto mid = static_cast<int16_t>(stage.y + (stage.height / 2));
	render_stage_shell(display);

	display.draw_rect(stage, MIST);

	auto prev_x = static_cast<int16_t>(stage.x + 2);
	int16_t prev_y = mid;
	for (auto x = static_cast<int16_t>(stage.x + 3); x < static_cast<int16_t>(stage.x + stage.width - 2); ++x) {
		const double t = (static_cast<double>(x) * 0.19) + (static_cast<double>(_frame) * 0.11);
		const double wave = (std::sin(t) * 7.0) + (std::sin(t * 0.37) * 3.0);
		const auto y = static_cast<int16_t>(mid + wave);
		display.draw_line(prev_x, prev_y, x, y, PRIMARY);
		prev_x = x;
		prev_y = y;
	}

	const auto scan_x = static_cast<int16_t>(stage.x + 2 + ((_frame / 2U) % static_cast<uint32_t>(stage.width - 4)));
	display.draw_v_line(scan_x, static_cast<int16_t>(stage.y + 2), static_cast<int16_t>(stage.height - 4), HIGHLIGHT);

	draw_crosshair(display, {_x, _y}, 5, ALERT);
}

void PlaygroundScene::render_mode_aurora_bloom(IDisplay& display) const {
	const Rect stage = stage_rect(display);
	const Point center = stage_center(display);
	render_stage_shell(display);

	for (int16_t k = 0; k < 9; ++k) {
		const double phase = (static_cast<double>(_frame) * 0.035) + (k * 0.62);
		const auto len = static_cast<int16_t>(18 + (std::sin(phase * 1.2) * 9.0));
		const auto dx = static_cast<int16_t>(std::cos(phase) * len);
		const auto dy = static_cast<int16_t>(std::sin(phase) * len);
		const Color petal = (k % 2 == 0) ? HIGHLIGHT : PRIMARY;
		display.draw_line(center.x, center.y, static_cast<int16_t>(center.x + dx), static_cast<int16_t>(center.y + dy), petal);
		display.draw_pixel(static_cast<int16_t>(center.x + dx), static_cast<int16_t>(center.y + dy), SOFT_WHITE);
	}

	for (int16_t ring = 8; ring <= 28; ring += 6) {
		for (int16_t deg = 0; deg < 360; deg += 28) {
			const double a = (static_cast<double>(deg) * 0.0174533) + (_frame * 0.006);
			const auto px = static_cast<int16_t>(center.x + (std::cos(a) * ring));
			const auto py = static_cast<int16_t>(center.y + (std::sin(a) * ring));
			if (px <= stage.x || px >= static_cast<int16_t>(stage.x + stage.width - 1)) {
				continue;
			}
			if (py <= stage.y || py >= static_cast<int16_t>(stage.y + stage.height - 1)) {
				continue;
			}
			display.draw_pixel(px, py, (ring % 12 == 0) ? ALERT : SOFT_WHITE);
		}
	}

	const Point focus = {
		_clamp_i16(_x, static_cast<int16_t>(stage.x + 4), static_cast<int16_t>(stage.x + stage.width - 5)),
		_clamp_i16(_y, static_cast<int16_t>(stage.y + 4), static_cast<int16_t>(stage.y + stage.height - 5)),
	};
	display.draw_line(center.x, center.y, focus.x, focus.y, ALERT);
	draw_crosshair(display, focus, 6, HIGHLIGHT);
	display.fill_rect(Rect{static_cast<int16_t>(center.x - 2), static_cast<int16_t>(center.y - 2), 5, 5}, ALERT);
}

void PlaygroundScene::render_hud(IDisplay& display) const {
	const Color bar = (_accent_phase < 8U) ? MIST : SECONDARY;
	display.fill_rect(Rect{0, 0, display.width(), 12}, bar);
	TextRenderer::draw_text(display, {3, 3}, "PLAYGROUND", SOFT_WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, {64, 3}, _mode_name(_mode), SOFT_WHITE, 1, COMPACT_FONT_3X5);

	const Color auto_color = _auto_move ? rgb565(140, 230, 170) : rgb565(210, 140, 170);
	const Color clear_color = _clear_each_frame ? rgb565(140, 230, 170) : rgb565(210, 140, 170);
	TextRenderer::draw_text(display, {3, 108}, "SHIFT MENU", HIGHLIGHT, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, {3, 116}, "START MODE", SOFT_WHITE, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, {74, 116}, "A AUTO", auto_color, 1, COMPACT_FONT_3X5);
	TextRenderer::draw_text(display, {114, 116}, "B TRACE", clear_color, 1, COMPACT_FONT_3X5);
}

void PlaygroundScene::draw_crosshair(IDisplay& display, Point center, int16_t radius, Color color) const {
	display.draw_h_line(static_cast<int16_t>(center.x - radius), center.y, static_cast<int16_t>(radius - 1), color);
	display.draw_h_line(static_cast<int16_t>(center.x + 2), center.y, static_cast<int16_t>(radius - 1), color);
	display.draw_v_line(center.x, static_cast<int16_t>(center.y - radius), static_cast<int16_t>(radius - 1), color);
	display.draw_v_line(center.x, static_cast<int16_t>(center.y + 2), static_cast<int16_t>(radius - 1), color);
	display.draw_pixel(center.x, center.y, SOFT_WHITE);
}

} // namespace handheld
