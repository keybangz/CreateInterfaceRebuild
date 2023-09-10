#pragma once

#include <array>

class Color {
public:
	Color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a = 255) : rgba_{ r, g, b, a } {}

	static Color from_hex(const std::uint32_t hex) {
		return Color(hex >> 16, hex >> 8, hex, hex >> 24);
	}

	std::uint8_t r() const {
		return rgba_[0];
	}

	std::uint8_t g() const {
		return rgba_[1];
	}

	std::uint8_t b() const {
		return rgba_[2];
	}

	std::uint8_t a() const {
		return rgba_[3];
	}

private:
	std::array<std::uint8_t, 4> rgba_;
};

#define COLOR_WHITE Color(255, 255, 255)
#define COLOR_BLACK Color(0, 0, 0)
#define COLOR_RED Color(255, 0, 0)
#define COLOR_GREEN Color(0, 255, 0)
#define COLOR_BLUE Color(0, 0, 255)
#define COLOR_YELLOW Color(255, 255, 0)