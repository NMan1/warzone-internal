#include "imports/lazy_importer.h"
#include "utils/globals.h"
#include "renderer/renderer.h"
#include "utils/utils.h"
#include "game/game.h"
#include "game/offsets.h"
#include "game/features/features.h"
#include <dxgi1_4.h>

#define VERSION "1.2"

extern "C" { unsigned int _fltused = 1; }

struct __declspec(uuid("189819f1-1db6-4b57-be54-1821339b85f7")) ID3D12Device;

__declspec(dllexport) uintptr_t present_orignal_address = 0xffffffffffff;

bool init = false;
extern "C" long __declspec(dllexport) hook_main(IDXGISwapChain3* swapchain, UINT sync, UINT flags) {
	if (!init) {
		utils::log_init("D:\\overflow.log");
		utils::log("starting");

		globals::window = game::get_window();
		if (!globals::window) {
			utils::log("invalid window");
		}

		if (!SUCCEEDED((globals::swapchain = swapchain)->GetDevice(__uuidof(ID3D12Device), (void**)&globals::d3d_device))) {
			utils::log("failed to get d3d device");
        }

		renderer::init();

		renderer::original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW((HWND)globals::window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(renderer::wndproc)));

		globals::base = (uintptr_t)GetModuleHandleA(NULL);	

		game::client_info = decryption::get_client_info(__readgsqword(0x60));
		if (!game::client_info) {
			utils::log("invalid client info");
			exit(0);
		}

		game::client_info_base = decryption::get_client_info_base(game::client_info, __readgsqword(0x60));
		if (!game::client_info_base) {
			utils::log("invalid client info base");
			exit(0);
		}

		auto ref_temp = decryption::get_ref_def();
		if (!ref_temp) {
			utils::log("invalid ref def ptr");
			exit(0);
		}

		game::ref_def = *(game::ref_def_t*)ref_temp;

		utils::log("intialzied");
		init = true;
	}
	
	renderer::begin();
	renderer::draw_text("overflow", { 15, 25 }, 18, { 255, 255, 255, 255 }); 
	renderer::draw_text("    ver " + std::string(VERSION), {15, 42}, 15, { 255, 7, 58, 255});

	const auto bone_base = decryption::get_bone_base(__readgsqword(0x60));
	char buf[64];
	sprintf(buf, "bone_base %p", bone_base);
	renderer::draw_text(buf, { 15, 200 }, 16, { 255, 50, 50, 255 });

	const auto bone_index = decryption::get_bone_index(7);
	sprintf(buf, "bone_index %p", bone_index);
	renderer::draw_text(buf, { 15, 215 }, 16, { 255, 50, 50, 255 });

	if (game::client_info) {
		const auto bone_base_pos = game::get_bone_base_pos(game::client_info);
		sprintf(buf, "bone_base_pos %d, %d, %d", bone_base_pos.x, bone_base_pos.y, bone_base_pos.z);
		renderer::draw_text(buf, { 15, 230 }, 16, { 255, 50, 50, 255 });
	}

	static bool new_game = true;
	auto ref_def_ptr = decryption::get_ref_def();
	if (ref_def_ptr && game::in_game()) {
		if (new_game) {
			decryption::update();
			new_game = false;
		}

		game::ref_def = *(game::ref_def_t*)ref_def_ptr;

		features::esp();

		if (globals::settings::no_recoil && globals::settings::no_recoil_key_toggle) {
			features::no_recoil();
			globals::settings::no_recoil_key_toggle = false;
		}
	}
	else {
		new_game = true;
	}

	renderer::end();

	if (globals::settings::end_cheat) { 
		SetWindowLongPtrW((HWND)globals::window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(renderer::wndproc));
		uintptr_t relative_address{};
		auto intruction_address = utils::pattern_scan((uintptr_t)LI_MODULE("DiscordHook64.dll").safe_cached(), "FF 15 ?? ?? ?? ?? 89 C6 48 8D ?? ?? ?? E8 ?? ?? ?? ?? 48 8B ?? ?? ?? 48 31 ?? E8 ?? ?? ?? ?? 89 F0 48 83 C4 ?? 5B 5F 5E C3");
		memcpy(&relative_address, (void*)(intruction_address + 0x2), 0x4);
		*(uintptr_t*)(intruction_address + 0x6 + relative_address) = present_orignal_address;
	}

	using original = long(*)(IDXGISwapChain3* swapchain, UINT sync, UINT flags);
	return ((original)present_orignal_address)(swapchain, sync, flags);
}
