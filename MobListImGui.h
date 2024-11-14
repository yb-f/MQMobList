#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>
#include "MobListImGui.h"
#include <mq/imgui/Widgets.h>

enum eColumnId {
	Id,
	Lvl,
	DisplayName,
	Name,
	Distance,
	Loc,
	Body,
	Race,
	Class,
	Direction
};

class MobListImGui
{
public:
	static void drawMobList(const std::vector<SPAWNINFO*>& spawnList);
	static void drawMobListTable(const std::vector<SPAWNINFO*>& spawnList);
	static void drawSearchHeader();

	static constexpr int FIRST_WINDOW_WIDTH = 445;
	static constexpr int FIRST_WINDOW_HEIGHT = 490;


	static constexpr int tableFlags = (ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg 
		| ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit);

	//static constexpr int columnFlags = (ImGuiTableColumnFlags_wid)
};

namespace ImGui {
	IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback = nullptr, void* user_data = nullptr);
}