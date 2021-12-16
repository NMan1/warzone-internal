#include "globals.h"

namespace globals {
	IDXGISwapChain* swapchain = nullptr;

	ID3D11Device* d3d_device = nullptr;

	ID3D11DeviceContext* d3d_context = nullptr;

	nk_context* nuklear_ctx = nullptr;

	int width = 0;

	int height = 0;
}