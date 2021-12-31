#include "globals.h"
#include "../renderer/renderer.h"
#include <cstdint>

namespace globals {
	IDXGISwapChain3* swapchain = nullptr;

	ID3D12Device* d3d_device = nullptr;

	HANDLE window = 0;

	uintptr_t base = 0;

	int width = 0;

	int height = 0;

	namespace settings {
		bool end_cheat = false;

		bool esp = true;

		int max_render_distance = 250;

		bool box_border = true;

		clr box_border_clr = { 255, 7, 58, 255 };

		bool box_filled = true;

		clr box_filled_clr = { 51, 51, 51, 55 };

		bool distance_text = true;

		clr distance_text_clr = { 255, 7, 58, 255 };

		bool snap_lines = true;

		clr snap_lines_clr = { 51, 51, 51, 225 };

		bool aimbot = false;

		bool aimbot_key_toggle = false;

		int aimbot_speed = 10;

		int fov = 15;

		bool no_recoil = true;

		bool no_recoil_key_toggle = false;
	}
}