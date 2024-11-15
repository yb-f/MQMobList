#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>
#include <mq/imgui/Widgets.h>

#define ARROW_WIDTH 5
#define ARROW_HEIGHT 15
#define ARROW_SIZE 25

extern bool showMobListWindow;

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
	static void drawDirectionalArrow(SPAWNINFO* spawn, ImVec2 cursorPos);
	static void drawMobRow(SPAWNINFO* spawn);
	static void drawMobList(std::vector<SPAWNINFO*>& spawnList);
	static void drawMobListTable(std::vector<SPAWNINFO*>& spawnList);
	static void drawSearchHeader();
	static void drawOptMenu();
	static constexpr int FIRST_WINDOW_WIDTH = 445;
	static constexpr int FIRST_WINDOW_HEIGHT = 490;


	static constexpr int tableFlags = (ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg 
		| ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit);
};

namespace ImGui {
	IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback = nullptr, void* user_data = nullptr);
}