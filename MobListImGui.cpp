#include "MobListImGui.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include <cmath>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include "Theme.h"

constexpr int ARROW_WIDTH = 5;
constexpr int ARROW_HEIGHT = 15;
constexpr int ARROW_SIZE = 25;
constexpr int TABLE_FLAGS = (ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | 
	ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit);


/**
 * \brief Check if item is in a visible row
 * 
 * \param rowIndex rowIndex of the item being checked
 * \param visableStart first visible row
 * \param visableEnd last visible row
 * \return true if visible 
 */
bool isRowVisible(int rowIndex, int visableStart, int visableEnd) {
	return rowIndex >= visableStart && rowIndex <= visableEnd;
}

/**
 * \brief Rotate point based off center points and angle
 * 
 * \param point the point we need to adjust
 * \param centerX center X point of the object to be drawn
 * \param centerY center Y point of the object to be drawn
 * \param angle angle of the object we are drawing
 * \return the new location of the point
 */
ImVec2 rotatePoint(ImVec2 point, float centerX, float centerY, float angle)
{
	float radians = angle * (PI / 180);
	float sinA = sin(radians);
	float cosA = cos(radians);

	float newX = cosA * (point.x - centerX) - sinA * (point.y - centerY) + centerX;
	float newY = sinA * (point.x - centerX) + cosA * (point.y - centerY) + centerY;
	
	return {newX, newY};
}

/**
 * \brief Determine consider color of spawn and return color value
 * 
 * \param spawn The spawn to be checked
 * \return ImVec4 color code
 */
ImVec4 getConColor(SPAWNINFO * spawn)
{
	switch (ConColor(spawn))
	{
	case CONCOLOR_GREY:
		return {0.6196f, 0.6196f, 0.6196f, 1.0f}; // Grey
	case CONCOLOR_GREEN:
		return {0.0f, 1.0f, 0.0f, 1.0f};        // Green
	case CONCOLOR_LIGHTBLUE:
		return {0.0f, 1.0f, 1.0f, 1.0f};        // Light Blue
	case CONCOLOR_BLUE:
		return {0.0f, 0.0f, 1.0f, 1.0f};         // Blue
	case CONCOLOR_WHITE:
		return {1.0f, 1.0f, 1.0f, 1.0f};        // White
	case CONCOLOR_YELLOW:
		return {1.0f, 1.0f, 0.0f, 1.0f};        // Yellow
	case CONCOLOR_RED:
	default:
		return {1.0f, 0.0f, 0.0f, 1.0f};        // Red
	}
}

/**
 * \brief Calculate the relative angle of player heading to spawn headingTo
 * 
 * \param spawn The spawn to determine the angle of
 * \return The relative angle to the spawn
 */
float calculateRelativeAngleTo(SPAWNINFO* spawn)
{
	float diffX = pLocalPlayer->X - spawn->X;
	float diffY = pLocalPlayer->Y - spawn->Y;
	float headingTo = atan2f(diffY, diffX) * 180.0f / PI + 90.0f;
	if (headingTo < 0.0f) {
		headingTo += 360.0f;
	}
	if (headingTo >= 360.0f) {
		headingTo -= 360.0f;
	}

	float heading = 360.0f - (pLocalPlayer->Heading * 0.703125f);

	float angle = headingTo - heading;
	if (angle < 0.0f)
	{
		angle += 360.0f;
	}
	if (angle >= 360.0f)
	{
		angle -= 360.0f;
	}
	return angle;
}

/// --------------------------------------------------------------------------------------------------------------

/**
 * \brief Main function for drawing the ImGui Mob List
 * 
 * \param spawnList the list of spawns
 * \param filters Filters object 
 */
void drawMobList(SpawnList& spawnList, Filters& filters)
{
	if (!filters.showMobListWindow) return;
	ImGuiStyle oldStyle = ImGuiTheme::ApplyTheme(filters.playerWinThemeId, filters.roundPlayerWin);
	if (ImGui::Begin("Mob List", &filters.showMobListWindow, ImGuiWindowFlags_MenuBar))
	{
		drawMenu(filters);
		drawSearchHeader(filters, spawnList);
		drawMobListTable(spawnList, filters);
	}
	ImGuiTheme::ResetTheme(oldStyle);
	ImGui::End();
}

/**
 * \brief Draw menu bar and items in ImGui window
 * 
 * \param filters Filters object
 */
void drawMenu(Filters& filters)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Show Con Colors", NULL, filters.conColor))
			{
				filters.conColor = !filters.conColor;
				WriteChatf(PLUGIN_MSG "Show Con Colors is now \ag%s", filters.conColor ? "enabled" : "disabled");
			}
			if (ImGui::MenuItem("Show Directional Arrow", NULL, filters.directionArrow))
			{
				filters.directionArrow = !filters.directionArrow;
				WriteChatf(PLUGIN_MSG "Show Directional Arrow is now \ag%s", filters.directionArrow ? "enabled" : "disabled");
			}
			if (ImGui::MenuItem("Select Theme", NULL, filters.drawPicker))
			{
				filters.drawPicker = !filters.drawPicker;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

/**
 * \brief Draw objects to control search criteria
 * 
 * \param filters Filters object
 */
void drawSearchHeader(Filters& filters, SpawnList& spawnList)
{
	if (filters.drawPicker)
	{
		filters.playerWinThemeId = ImGuiTheme::DrawThemePicker(filters.playerWinThemeId, "Mob List");
	}
	ImGui::Text("Level Range");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(45);
	if (ImGui::InputInt("##LowLevel", &filters.levelLow, 0))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Lowest level to display");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(45);
	if (ImGui::InputInt("##HighLevel", &filters.levelHigh, 0))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Highest level to display");
	}
	ImGui::SameLine();
	ImGui::Text("Name");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	if (ImGui::InputText("##Name", &filters.name))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Name filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##NameReverse", &filters.nameReverse))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse filter name");
	}

	ImGui::Text("Distance");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	if (ImGui::InputInt("##DistanceLow", &filters.minDistance, 0))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Minimum distance (closer than this will not be shown)");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	if (ImGui::InputInt("##DistanceHigh", &filters.maxDistance, 0))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Maximum Distance (further than this will not be shown)");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(85);
	if (ImGui::BeginCombo("##TypeSelection", filters.spawnTypeNames[filters.typeSelection].first))
	{
		for (size_t i = 0; i < filters.spawnTypeNames.size(); ++i)
		{
			bool is_selected = (filters.typeSelection == i);
			if (ImGui::Selectable(filters.spawnTypeNames[i].first, is_selected))
			{
				filters.typeSelection = filters.spawnTypeNames[i].second;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		filters.debounceTimer = filters.debounceDelay;
		ImGui::EndCombo();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Type of mobs to display");
	}

	ImGui::SameLine();
	ImGui::Text("Direction");
	if (ImGui::Checkbox("##DirectionArrow", &filters.directionArrow))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	ImGui::SameLine();
	ImGui::SameLine();
	if (ImGui::Button("Clear Highlights"))
	{
		EzCommand("/highlight reset");
	}

	ImGui::Text("Body");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Body", &filters.bodyType))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Body type filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##BodyReverse", &filters.bodyReverse))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse filter body type");
	}

	ImGui::SameLine();
	ImGui::Text("Race");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Race", &filters.raceName))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Race filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##RaceReverse", &filters.raceReverse))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse Filter Race");
	}

	ImGui::SameLine();
	ImGui::Text("Class");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Class", &filters.className))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Class filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##ClassReverse", &filters.classReverse))
	{
		filters.debounceTimer = filters.debounceDelay;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse Filter class");
	}
	ImGui::Text("Debounce Timer: %.3f", filters.debounceTimer);
	if (filters.debounceTimer > 0.0f)
	{
		filters.debounceTimer -= ImGui::GetIO().DeltaTime; // Decrease timer based on frame time.
		if (filters.debounceTimer <= 0.0f)
		{
			filters.debounceTimer = 0.0f;
			// Trigger update after debounce delay
			spawnList.updateDisplayedSpawns(filters); // Trigger the update of displayed spawns.
		}
	}
}

/**
 * \brief Create the table for spawns to be displayed in
 * 
 * \param spawnList The List of spawns
 * \param filters Filters object
 */
void drawMobListTable(SpawnList& spawnList, Filters filters)
{
	if (spawnList.empty())
	{
		ImGui::Text("Spawn list empty");
		return;
	}

	if (ImGui::BeginTable("##List table", 10 + filters.directionArrow, TABLE_FLAGS))
	{
		ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort, 0, TableColumnID::ID);
		ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_None, 0, TableColumnID::Level);
		ImGui::TableSetupColumn("Display Name", ImGuiTableColumnFlags_None, 0, TableColumnID::DisplayName);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0, TableColumnID::Name);
		ImGui::TableSetupColumn("Surname", ImGuiTableColumnFlags_None, 0, TableColumnID::Surname);
		ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_None, 0, TableColumnID::Distance);
		ImGui::TableSetupColumn("Loc", ImGuiTableColumnFlags_NoSort, 0, TableColumnID::Location);
		ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_None, 0, TableColumnID::Body);
		ImGui::TableSetupColumn("Race", ImGuiTableColumnFlags_None, 0, TableColumnID::Race);
		ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_None, 0, TableColumnID::Class);
		if (filters.directionArrow)
		{
			ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_NoSort, 0, TableColumnID::Direction);
		}
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();
		ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();

		if (sortSpecs && sortSpecs->SpecsCount > 0)
		{
			const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs->Specs[0];
			ImGuiID columnId = sortSpec.ColumnUserID;
			bool ascending = (sortSpec.SortDirection == ImGuiSortDirection_Ascending);

			spawnList.sortSpawns(columnId, ascending, filters);
		}
		
		ImGuiListClipper clipper;
		clipper.Begin(spawnList.visibleSize());

		while (clipper.Step())
		{
			for (int rowIndex = clipper.DisplayStart; rowIndex < clipper.DisplayEnd; ++rowIndex)
			{
				const SpawnObject& spawn = spawnList[rowIndex];
				if (!spawn.m_bDisplayed)
					break;

				drawMobRow(spawn, filters);
			}
		}
		ImGui::EndTable();
	}
}

/**
 * \brief Draw the row of an individual spawn within the table
 * 
 * \param spawn The spawn to be displayed
 * \param filters Filters object
 */
void drawMobRow(const SpawnObject& spawn, Filters filters)
{
	if (!spawn.m_bDisplayed)
		return;

	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::Selectable(std::to_string(spawn.m_pSpawn->SpawnID).c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
	if (ImGui::IsItemHovered())
	{
		if (filters.serverType == 4)
		{
			ImGui::SetTooltip("Left click to target\nCtrl + left click to navigate self\nCtrl + Shift + left click to navigate group\nRight click to highlight\nCtrl + right click to search mob on allakhazam");	
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				WriteChatf(PLUGIN_MSG "Highlighting mobs named \ar%s", spawn.m_pSpawn->DisplayedName);
				DoCommandf("/highlight \"%s\"", spawn.m_pSpawn->DisplayedName);
			}
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyDown(ImGuiKey_ModShift))
				{
					WriteChatf(PLUGIN_MSG "Navigating \aogroup \awto \ar%s \awID \ar%d", spawn.m_pSpawn->DisplayedName, spawn.m_pSpawn->SpawnID);
					DoCommandf("/dgae /nav id %d", spawn.m_pSpawn->SpawnID);
				}
				else if (ImGui::IsKeyDown(ImGuiKey_ModCtrl))
				{
					WriteChatf(PLUGIN_MSG "Navigating \aoself \awto \ar%s \awID \ar%d", spawn.m_pSpawn->DisplayedName, spawn.m_pSpawn->SpawnID);
					DoCommandf("/nav id %d", spawn.m_pSpawn->SpawnID);
				}
				else
				{
					WriteChatf(PLUGIN_MSG "Targeting \ar%s \awID \ar%d", spawn.m_pSpawn->DisplayedName, spawn.m_pSpawn->SpawnID);
					pTarget = spawn.m_pSpawn;
				}
			}
		}
		else {
			ImGui::SetTooltip("Right click to highlight\nLeft click to navigate self\nCtrl + left click to navigate group");
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				WriteChatf(PLUGIN_MSG "Highlighting mobs named \ar%s", spawn.m_pSpawn->DisplayedName);
				DoCommandf("/highlight \"%s\"", spawn.m_pSpawn->DisplayedName);
			}
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (ImGui::IsKeyDown(ImGuiKey_ModCtrl))
				{
					WriteChatf(PLUGIN_MSG "Navigating \aogroup \awto \ar%s \awID \ar%d", spawn.m_pSpawn->DisplayedName, spawn.m_pSpawn->SpawnID);
					DoCommandf("/dgae /nav id %d", spawn.m_pSpawn->SpawnID);
				}
				else
				{
					WriteChatf(PLUGIN_MSG "Navigating \aoself \awto \ar%s \awID \ar%d", spawn.m_pSpawn->DisplayedName, spawn.m_pSpawn->SpawnID);
					DoCommandf("/nav id %d", spawn.m_pSpawn->SpawnID);
				}
			}
		}
	}
	ImGui::TableNextColumn();
	ImGui::Text("%d", spawn.m_pSpawn->Level);
	ImGui::TableNextColumn();
	if (filters.conColor)
	{
		auto color = getConColor(spawn.m_pSpawn);
		ImGui::TextColored(color, "%s", spawn.m_pSpawn->DisplayedName);
		ImGui::TableNextColumn();
		ImGui::TextColored(color, "%s", spawn.m_pSpawn->Name);
	}
	else
	{
		ImGui::Text("%s", spawn.m_pSpawn->DisplayedName);
		ImGui::TableNextColumn();
		ImGui::Text("%s", spawn.m_pSpawn->Name);
	}
	ImGui::TableNextColumn();
	//Surname
	ImGui::Text("%s", spawn.m_pSpawn->Lastname);
	ImGui::TableNextColumn();
	//calculate distance to spawn
	ImGui::Text("%.1f", sqrt(spawn.m_fDistance));
	ImGui::TableNextColumn();
	ImGui::Text("%.1f, %.1f, %.1f", spawn.m_pSpawn->Y, spawn.m_pSpawn->X, spawn.m_pSpawn->Z);
	ImGui::TableNextColumn();
	//get body type
	ImGui::Text("%s", GetBodyTypeDesc(GetBodyType(spawn.m_pSpawn)));
	ImGui::TableNextColumn();
	ImGui::Text("%s", spawn.m_pSpawn->GetRaceString());
	ImGui::TableNextColumn();
	ImGui::Text("%s", spawn.m_pSpawn->GetClassString());
	if (filters.directionArrow)
	{
		ImGui::TableNextColumn();
		drawDirectionalArrow(spawn.m_pSpawn, ImGui::GetCursorScreenPos());
	}
}

/**
 * \brief Draw a rotating arrow pointing the relative direction to the spawn
 * 
 * \param spawn The spawn we are drawing for
 * \param cursorPos The current position of the cursor
 */
void drawDirectionalArrow(SPAWNINFO* spawn, ImVec2 cursorPos)
{
	float angle = calculateRelativeAngleTo(spawn);
	ImVec2 p1 = ImVec2(cursorPos.x + ARROW_SIZE / 2, cursorPos.y);
	ImVec2 p2 = ImVec2(p1.x + ARROW_WIDTH, p1.y + ARROW_HEIGHT);
	ImVec2 p3 = ImVec2(p1.x - ARROW_WIDTH, p1.y + ARROW_HEIGHT);
	float centerX = (p1.x + p2.x + p3.x) / 3;
	float centerY = (p1.y + p2.y + p3.y) / 3;

	p1 = rotatePoint(p1, centerX, centerY, angle);
	p2 = rotatePoint(p2, centerX, centerY, angle);
	p3 = rotatePoint(p3, centerX, centerY, angle);

	ImGui::GetWindowDrawList()->AddTriangleFilled(p1, p2, p3, IM_COL32(0, 255, 0, 255));
}
