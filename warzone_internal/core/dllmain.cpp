#include <d3d11.h>
#include <stdio.h>
#include "imports/crt/crt.h"
#include "imports/lazy_importer.h"
#include "utils/globals.h"

bool init = false;
extern "C" HRESULT __declspec(dllexport) HookMain(IDXGISwapChain* swapchain, UINT sync, UINT flags) {
	if (!init) {
		init = true;
		(globals::swapchain = swapchain)->GetDevice(__uuidof(globals::d3d_device), (void**)&globals::d3d_device);
		globals::render = new renderer(swapchain, globals::d3d_device, L"Tahoma");
	}

	globals::render->begin();

	globals::render->draw_text({ 15, 10 }, L"overflow", colors::MediumVioletRed);
	globals::render->draw_text({ globals::render->width / 2, globals::render->height / 2 }, L"owned by overflow\npremium cheating solutions", { 0.894, 0.129, 0.129, 1}, FW1_RESTORESTATE | FW1_CENTER | FW1_VCENTER, 120);

	globals::render->draw();
	globals::render->end();
	
	auto present_orignal_address = 0xffffffffffff;
	if (LI_FN(GetAsyncKeyState).safe_cached()(VK_END) & 1) {
		delete globals::render;
		uintptr_t relative_address{};
		memcpy(&relative_address, (void*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x2), 0x4);
		*(uintptr_t*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x6 + relative_address) = present_orignal_address;
	}

	using original = HRESULT(*)(IDXGISwapChain* swapchain, UINT sync, UINT flags);
	return ((original)present_orignal_address)(swapchain, sync, flags);
}
