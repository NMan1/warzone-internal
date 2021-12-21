#include "imports/syscall/syscall.h"
#include "imports/lazy_importer.h"
#include "utils/globals.h"
#include "renderer/renderer.h"
#include "utils/utils.h"
#include <dxgi1_4.h>

extern "C" { unsigned int _fltused = 1; }

struct __declspec(uuid("189819f1-1db6-4b57-be54-1821339b85f7")) ID3D12Device;

bool init = false;
extern "C" long __declspec(dllexport) hook_main(IDXGISwapChain3* swapchain, UINT sync, UINT flags) {
	if (!init) {
		globals::window = FindWindowA(NULL, "Call of Duty: Modern Warfare");
		if (!SUCCEEDED((globals::swapchain = swapchain)->GetDevice(__uuidof(ID3D12Device), (void**)&globals::d3d_device))) {
			MessageBoxA(0, "failed to get d3d12 device", "", MB_OK);
		}
		renderer::init();
		init = true;
	}
	
	renderer::begin();
	renderer::draw_text("overflow", { 15, 10 }, 12, { 255, 0, 0, 255 });
	renderer::end();

	static volatile auto present_orignal_address = 0xffffffffffff;
	if (syscall<short>(0x1044, 0x23) & 1) { // VK_END
		uintptr_t relative_address{};
		memcpy(&relative_address, (void*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x2), 0x4);
		*(uintptr_t*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x6 + relative_address) = present_orignal_address;
	}

	using original = long(*)(IDXGISwapChain3* swapchain, UINT sync, UINT flags);
	return ((original)present_orignal_address)(swapchain, sync, flags);
}
