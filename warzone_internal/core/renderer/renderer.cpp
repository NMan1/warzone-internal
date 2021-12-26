#include "renderer.h"
#include "../utils/globals.h"
#include "../utils/utils.h"
#include "menu.h"

#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
ID3D12GraphicsCommandList* d3d12CommandList = nullptr;
ID3D12Fence* d3d12Fence = nullptr;
UINT64 d3d12FenceValue = 0;
ID3D12CommandQueue* d3d12CommandQueue = nullptr;

struct FrameContext {
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12Resource* main_render_target_resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

uint32_t buffersCounts = -1;
FrameContext* frameContext;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace renderer {
	ImFont* font;

	WNDPROC original_wndproc;

	bool init() {
		if (!globals::d3d_device) {
			utils::log("invalid d3d_device");
			return false;
		}

		d3d12CommandQueue = *(ID3D12CommandQueue**)((uintptr_t)globals::swapchain + 0x118);

		ImGui::CreateContext();

		unsigned char* pixels;
		int width, height;
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		menu::setup_theme();
		io.Fonts->AddFontDefault();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		io.IniFilename = NULL;

		CreateEvent(nullptr, false, false, nullptr);

		DXGI_SWAP_CHAIN_DESC sdesc;
		globals::swapchain->GetDesc(&sdesc);
		globals::width = sdesc.BufferDesc.Width;
		globals::height = sdesc.BufferDesc.Height;
		sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sdesc.OutputWindow = (HWND)globals::window;
		sdesc.Windowed = ((GetWindowLongPtr((HWND)globals::window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

		buffersCounts = sdesc.BufferCount;
		frameContext = new FrameContext[buffersCounts];

		D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
		descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorImGuiRender.NumDescriptors = buffersCounts;
		descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		if (globals::d3d_device->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&d3d12DescriptorHeapImGuiRender)) != S_OK) {
			return false;
		}

		ID3D12CommandAllocator* allocator;
		if (globals::d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK) {
			return false;
		}

		for (size_t i = 0; i < buffersCounts; i++) {
			frameContext[i].commandAllocator = allocator;
		}

		if (globals::d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&d3d12CommandList)) != S_OK || d3d12CommandList->Close() != S_OK)
			return false;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
		descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descriptorBackBuffers.NumDescriptors = buffersCounts;
		descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorBackBuffers.NodeMask = 1;

		if (globals::d3d_device->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)) != S_OK)
			return false;

		const auto rtvDescriptorSize = globals::d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

		for (size_t i = 0; i < buffersCounts; i++) {
			ID3D12Resource* pBackBuffer = nullptr;

			frameContext[i].main_render_target_descriptor = rtvHandle;
			globals::swapchain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			globals::d3d_device->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
			frameContext[i].main_render_target_resource = pBackBuffer;
			rtvHandle.ptr += rtvDescriptorSize;
		}

		ImGui_ImplWin32_Init((HWND)globals::window);
		ImGui_ImplDX12_Init(globals::d3d_device, buffersCounts,
			DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
			d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
			d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());

		font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		ImGui_ImplDX12_CreateDeviceObjects();		
	}

	void begin() {
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		//io.WantCaptureMouse = menu::open;

		if (menu::open) {
			menu::render();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
	}

	void end() {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();

		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		FrameContext& currentFrameContext = frameContext[globals::swapchain->GetCurrentBackBufferIndex()];
		currentFrameContext.commandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		d3d12CommandList->Reset(currentFrameContext.commandAllocator, nullptr);
		d3d12CommandList->ResourceBarrier(1, &barrier);
		d3d12CommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
		d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		d3d12CommandList->ResourceBarrier(1, &barrier);
		d3d12CommandList->Close();

		d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));
	}

	void draw_line(const ImVec2& from, const ImVec2& to, clr color, float thickness) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->AddLine(from, to, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), thickness);
	}

	void draw_circle(const ImVec2& position, float radius, clr color, float thickness, uint32_t segments) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->AddCircle(position, radius, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), segments, thickness);
	}

	void draw_rect(const ImVec2& from, const ImVec2& to, clr color, float rounding, float thickness) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->AddRect(from, to, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), rounding, NULL, thickness);
	}

	void draw_rect_filled(const ImVec2& from, const ImVec2& to, clr color, float rounding) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->AddRectFilled(from, to, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), rounding, NULL);
	}

	void draw_text(const std::string& text, const ImVec2& position, float size, clr color, bool center) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float y = 0.0f;
		int i = 0;

		ImVec2 textSize = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());

		if (center) {
			window->DrawList->AddText(font, size, { position.x - textSize.x / 2.0f, position.y  }, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), text.c_str());
		}
		else {
			window->DrawList->AddText(font, size, { position.x, position.y }, ImGui::GetColorU32({ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }), text.c_str());
		}
	}

	LRESULT wndproc(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
		ImGuiIO& io = ImGui::GetIO();
		switch (msg) {
		case WM_KEYDOWN: {
			if (param == VK_END)
				globals::settings::end_cheat = true;
			else if (param == VK_INSERT || param == VK_F5) // 0x4E = M
				menu::open = !menu::open;
			else if (param == VK_LBUTTON)
				globals::settings::no_recoil_key_toggle = true;			
			else if (param == VK_SHIFT)
				globals::settings::aimbot_key_toggle = true;
			break;
		case WM_KEYUP:
			if (param == VK_SHIFT)
				globals::settings::aimbot_key_toggle = false;
			break;
		case WM_LBUTTONDOWN:
			globals::settings::no_recoil_key_toggle = true;
			//globals::settings::aimbot_key_toggle = true;
			break;
		case WM_LBUTTONUP:
			globals::settings::no_recoil_key_toggle = false;
			//globals::settings::aimbot_key_toggle = false;
			break;
		}
		};

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, param, lparam))
			return true;
		return CallWindowProc(renderer::original_wndproc, hwnd, msg, param, lparam);
	}
}