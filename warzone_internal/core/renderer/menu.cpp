#include "../utils/globals.h"
#include <string>
#include "menu.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <vector>

namespace menu {
	bool open = false;

	void setup_theme() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.f;
		style.WindowPadding = ImVec2(0, 0); // 8 or 9 x
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.ChildRounding = 0.0f;
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(8, 8);
		style.ItemInnerSpacing = ImVec2(8, 8);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 0.0f;
		style.ScrollbarSize = 6.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.0f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.CurveTessellationTol = 1.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1, 1, 1, .8); // grey
		//colors[ImGuiCol_Text] = ImVec4(.6f, .6f, .6f, 1.00f); // grey
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		colors[ImGuiCol_WindowBg] = { 0.133, 0.133, 0.133, 1.0f };
		colors[ImGuiCol_ChildBg] = { 0.149, 0.149, 0.149, 1 };
		colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 1.f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0, 0, .0f);
		colors[ImGuiCol_FrameBg] = { 0.149, 0.149, 0.149, 1 };
		colors[ImGuiCol_FrameBgHovered] = ImVec4(.6f, .6f, .6f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
		colors[ImGuiCol_ScrollbarGrab] = { 1, 0.321, 0.321, .70f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1, 0.321, 0.321, .80f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1, 0.321, 0.321, .80f);
		colors[ImGuiCol_Separator] = ImVec4(1, 0.027, 0.227, .75f);
		//colors[ImGuiCol_CheckMark] = { 1, 1, 1, .6 };
		colors[ImGuiCol_CheckMark] = { 1, 0.027, 0.227, 1 };
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.); //multicombo, combo selected item color.
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	}

	void render() {
		const ImVec2 window_size = { 360, 240 };

		ImGui::SetNextWindowSize(ImVec2(globals::width, globals::height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowBgAlpha(.45);
		ImGui::Begin("Background", &menu::open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove); {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.568, 0.533, 0.533, .2f));
			ImGui::PopStyleColor();
		}
		ImGui::End();

		ImGui::SetWindowPos({ globals::width / 2 - (window_size.x / 2), globals::height / 2 - (window_size.y / 2) }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(window_size);
		ImGui::Begin("overflow", &menu::open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			ImGui::PushStyleColor(ImGuiCol_Border, { 0.254, 0.254, 0.254, 1.0f });

			ImGui::SetCursorPos({ 5, 5 }); // our "margins"
			ImGui::BeginChild("background pane", { window_size.x - 10, window_size.y - 10 }, true); { // gives 5 pixel space on every side 
				auto window_pos = ImGui::GetWindowPos();
				ImGui::GetWindowDrawList()->AddRectFilled({ window_pos.x - 4, window_pos.y }, { window_pos.x + window_size.x + 4, window_pos.y + 2 }, ImGui::ColorConvertFloat4ToU32({ 1, 0.027, 0.227, 1 }));
			}
			ImGui::EndChild();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.176, 0.176, 0.176, 1.0 });
			ImGui::SetCursorPos({ 20, 20 });
			ImGui::BeginChild("settings pane", { 320, 200 }, true); {
				ImGui::Checkbox("toggle esp", &globals::settings::esp);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::SliderInt22("max render distance", &globals::settings::max_render_distance , 1, 500);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::Checkbox("box border", &globals::settings::box_border);
				ImGui::ColorPicker("box border color", &globals::settings::box_border_clr, ImGuiColorEditFlags_NoInputs);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::Checkbox("box filled", &globals::settings::box_filled);
				ImGui::ColorPicker("box filled color", &globals::settings::box_filled_clr, ImGuiColorEditFlags_NoInputs);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::Checkbox("snap lines", &globals::settings::snap_lines);
				ImGui::ColorPicker("snap lines color", &globals::settings::snap_lines_clr, ImGuiColorEditFlags_NoInputs);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::Checkbox("distance text", &globals::settings::distance_text);
				ImGui::ColorPicker("distance text color", &globals::settings::distance_text_clr, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("aimbot", &globals::settings::aimbot);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::SliderInt22("speed", &globals::settings::aimbot_speed, 1, 100);
				
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
				ImGui::SliderInt22("fov", &globals::settings::fov, 1, 90);

				ImGui::Checkbox("no recoil", &globals::settings::no_recoil);
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
}