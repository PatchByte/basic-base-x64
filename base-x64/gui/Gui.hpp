#pragma once
#include "../common.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

namespace base::gui
{
	enum class ImGuiMenu
	{
		Home,
		Network,
		Settings
	};

	enum class ImGuiThemes
	{
		Nemesis,
		Dark,
		AnotherDarkTheme,
		VGui
	};

	void InitializeTheme(ImGuiThemes Theme);
	void Initialize();
	void Render();
	void Tick();
	void Uninitialize();

	extern ImFont* g_SourceSansPro;
	extern ImFont* g_MaterialDesign;
	extern ImFont* g_Neon;

	extern ImGuiMenu g_CurrentMenu;
}

namespace ImGui
{

	void ButtonMenu(const char* name, base::gui::ImGuiMenu* current_menu, base::gui::ImGuiMenu my_menu);
	bool BeginPanel(const char* name, ImVec2 siZe);
	void EndPanel();

}