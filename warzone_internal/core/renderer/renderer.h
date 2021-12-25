#pragma once
#include "imgui/imgui.h"
#include <Windows.h>
#include <string>

struct clr {
	float r, g, b, a;
};

namespace renderer {
	extern WNDPROC original_wndproc;

	bool init();

	void begin();

	void end();

	void menu();

	void draw_line(const ImVec2& from, const ImVec2& to, clr color, float thickness);

	void draw_circle(const ImVec2& position, float radius, clr color, float thickness, uint32_t segments);

	void draw_rect(const ImVec2& from, const ImVec2& to, clr color, float rounding, float thickness);

	void draw_rect_filled(const ImVec2& from, const ImVec2& to, clr color, float rounding = 0);

	void draw_text(const std::string& text, const ImVec2& position, float size, clr color, bool center=false);

	LRESULT wndproc(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
}