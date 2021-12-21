#include "globals.h"

namespace globals {
	IDXGISwapChain3* swapchain = nullptr;

	ID3D12Device* d3d_device = nullptr;

	HANDLE window = 0;

	int width = 0;

	int height = 0;
}