#pragma once

struct IDXGISwapChain3;

struct ID3D12Device;

struct ID3D11DeviceContext;

typedef void* HANDLE;

typedef unsigned __int64  uintptr_t;

struct clr;

namespace globals {
	extern IDXGISwapChain3* swapchain;

	extern ID3D12Device* d3d_device;

	extern HANDLE window;

	extern uintptr_t base;

	extern int width;

	extern int height;

	namespace settings {
		extern bool end_cheat;

		extern bool esp;

		extern int max_render_distance;

		extern bool box_border;

		extern clr box_border_clr;

		extern bool box_filled;

		extern clr box_filled_clr;

		extern bool distance_text;

		extern clr distance_text_clr;

		extern bool snap_lines;

		extern clr snap_lines_clr;

		extern bool aimbot;

		extern bool aimbot_key_toggle;

		extern int aimbot_speed;

		extern int fov;

		extern bool no_recoil;
		
		extern bool no_recoil_key_toggle;
	}
}