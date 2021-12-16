#include "renderer/nuklear_defines.h"
#include "imports/crt/crt.h"
#include "imports/lazy_importer.h"
#include "utils/globals.h"

extern "C" { unsigned int _fltused = 1; }

bool init = false;
extern "C" HRESULT __declspec(dllexport) hook_main(IDXGISwapChain* swapchain, UINT sync, UINT flags) {
	if (!init) {
		(globals::swapchain = swapchain)->GetDevice(__uuidof(globals::d3d_device), (void**)&globals::d3d_device);
		globals::d3d_device->GetImmediateContext(&globals::d3d_context);

		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		crt::memset(&swap_chain_desc, 0, sizeof(swap_chain_desc));
		globals::swapchain->GetDesc(&swap_chain_desc);

		globals::width = swap_chain_desc.BufferDesc.Width;
		globals::height = swap_chain_desc.BufferDesc.Height;

		globals::nuklear_ctx = nk_d3d11_init(globals::d3d_device, globals::width, globals::height, MAX_VERTEX_BUFFER, MAX_INDEX_BUFFER);

		nk_font_atlas* atlas;
		nk_d3d11_font_stash_begin(&atlas);
		nk_d3d11_font_stash_end();

		init = true;
	}
	
	struct nk_style* s = &globals::nuklear_ctx->style;
	nk_style_push_color(globals::nuklear_ctx, &s->window.background, nk_rgba(0, 0, 0, 0));
	nk_style_push_style_item(globals::nuklear_ctx, &s->window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));

	nk_begin(globals::nuklear_ctx, "Demo", nk_rect(0, 0, globals::width, globals::height), NK_WINDOW_BACKGROUND | NK_WINDOW_NO_INPUT);

	nk_color c = { 0xFF,0xFF,0xFF,0xFF };
	nk_stroke_line(&globals::nuklear_ctx->current->buffer, 10, 10, 100, 100, 2.0f, c);
	nk_draw_text(&globals::nuklear_ctx->current->buffer, { 10, 10, 100, 100 }, "TEST", 4, globals::nuklear_ctx->style.font, c, c);

	nk_end(globals::nuklear_ctx);

	nk_style_pop_color(globals::nuklear_ctx);
	nk_style_pop_style_item(globals::nuklear_ctx);

	auto present_orignal_address = 0xffffffffffff;
	if (LI_FN(GetAsyncKeyState).safe_cached()(VK_END) & 1) {
		uintptr_t relative_address{};
		crt::memcpy(&relative_address, (void*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x2), 0x4);
		*(uintptr_t*)((uintptr_t)LI_MODULE("gameoverlayrenderer64.dll").safe_cached() + 0x891ad + 0x6 + relative_address) = present_orignal_address;
	}

	using original = HRESULT(*)(IDXGISwapChain* swapchain, UINT sync, UINT flags);
	return ((original)present_orignal_address)(swapchain, sync, flags);
}
