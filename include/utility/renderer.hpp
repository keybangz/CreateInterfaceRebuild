#pragma once

#include "sdk/sdk.hpp"
#include "utility/color.hpp"

namespace renderer {
	enum TextFlags : std::uint32_t {
		AlignCenterHorizontal = 1 << 0,
		AlignCenterVertical = 1 << 1,
		AlignTop = 1 << 2,
		AlignBottom = 1 << 3,
		AlignLeft = 1 << 4,
		AlignRight = 1 << 5
	};

	bool initialize(IDXGISwapChain* swap_chain);

	void initialize_imgui(HWND window_handle);

	void destroy();

	void destroy_imgui();

	void begin_frame();

	void end_frame();

	bool create_render_target(IDXGISwapChain* swap_chain);

	void cleanup_device();

	void cleanup_render_target();

	void draw_line(glm::vec2 p1, glm::vec2 p2, Color color);

	void draw_rectangle(glm::vec2 position, glm::vec2 size, Color color);

	void draw_rectangle(glm::vec4 area, Color color);

	void draw_rectangle_filled(glm::vec2 position, glm::vec2 size, Color color);

	void draw_rectangle_filled(glm::vec4 area, Color color);

	void draw_text(glm::vec2 position, Color color, std::string_view text, std::uint32_t flags = 0, bool outline = true);
}