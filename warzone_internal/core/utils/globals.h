#pragma once
#include <d3d11.h>

struct nk_context;

namespace globals {
	extern IDXGISwapChain* swapchain;

	extern ID3D11Device* d3d_device;

	extern ID3D11DeviceContext* d3d_context;

	extern nk_context* nuklear_ctx;

	extern int width;

	extern int height;
}