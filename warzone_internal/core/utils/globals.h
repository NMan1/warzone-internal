#pragma once

struct IDXGISwapChain3;

struct ID3D12Device;

struct ID3D11DeviceContext;

typedef void* HANDLE;

namespace globals {
	extern IDXGISwapChain3* swapchain;

	extern ID3D12Device* d3d_device;

	extern HANDLE window;

	extern int width;

	extern int height;
}