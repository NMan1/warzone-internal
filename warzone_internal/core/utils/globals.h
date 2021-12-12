#pragma once
#include "..\renderer\renderer.h"

namespace globals {
	extern renderer* render;

	extern IDXGISwapChain* swapchain;

	extern ID3D11Device* d3d_device;
}