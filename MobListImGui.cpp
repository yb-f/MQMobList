#include "MobListImGui.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include <cmath>

bool showMobListWindow = true;

void sortSpawnList(std::vector<SPAWNINFO*>& spawnList, eColumnId column, bool ascending) {
	std::sort(spawnList.begin(), spawnList.end(), [column, ascending](SPAWNINFO* a, SPAWNINFO* b)
		{
			switch (column) {
			case eColumnId::Id: return ascending ? a->SpawnID < b->SpawnID : a->SpawnID > b->SpawnID;
			case eColumnId::Lvl: return ascending ? a->Level < b->Level : a->Level > b->Level;

			case eColumnId::DisplayName: {
				int cmp = std::strcmp(a->DisplayedName, b->DisplayedName);
				return ascending ? cmp < 0 : cmp > 0;
			}
			case eColumnId::Name: {
				int cmp = std::strcmp(a->Name, b->Name);
				return ascending ? cmp < 0 : cmp > 0;
			}
			case eColumnId::Distance: {
				float distA = GetDistance3D(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z, a->X, a->Y, a->Z);
				float distB = GetDistance3D(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z, b->X, b->Y, b->Z);
				return ascending ? distA < distB : distA > distB;
			}
			case eColumnId::Body: {
				int cmp = std::strcmp(GetBodyTypeDesc(GetBodyType(a)), GetBodyTypeDesc(GetBodyType(b)));
				return ascending ? cmp < 0 : cmp > 0;
			}
			case eColumnId::Race: {
				int cmp = std::strcmp(a->GetRaceString(), b->GetRaceString());
				return ascending ? cmp < 0 : cmp > 0;
			}
			case eColumnId::Class: {
				int cmp = std::strcmp(a->GetClassString(), b->GetClassString());
				return ascending ? cmp < 0 : cmp > 0;
			}
			default: return false;
			}
		});
}

bool isRowVisible(int rowIndex, int visableStart, int visableEnd) {
	return rowIndex >= visableStart && rowIndex <= visableEnd;
}

void MobListImGui::drawMobList(std::vector<SPAWNINFO*>& spawnList)
{
	if (!showMobListWindow) return;
	if (ImGui::Begin("Mob List", &showMobListWindow, ImGuiWindowFlags_MenuBar))
	{
		drawOptMenu();
		drawSearchHeader();
		drawMobListTable(spawnList);
	}
	ImGui::End();
}

void MobListImGui::drawSearchHeader()
{
	ImGui::Text("Level Range");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(45);
	if (ImGui::InputInt("##LowLevel", &Filters::levelLow, 0))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Lowest level to display");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(45);
	if (ImGui::InputInt("##HighLevel", &Filters::levelHigh, 0))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Highest level to display");
	}

	ImGui::SameLine();
	ImGui::Text("Name");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	if (ImGui::InputText("##Name", &Filters::name))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Name filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##NameReverse", &Filters::nameReverse))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse filter name");
	}

	ImGui::Text("Distance");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	if (ImGui::InputInt("##DistanceLow", &Filters::minDistance, 0))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Minimum distance (closer than this will not be shown)");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50);
	if (ImGui::InputInt("##DistanceHigh", &Filters::maxDistance, 0))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Maximum Distance (further than this will not be shown)");
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(85);
	if (ImGui::BeginCombo("##TypeSelection", Filters::typeNames[Filters::typeSelection]))
	{
		for (int i = 0; i < Filters::typeNameCount; ++i)
		{
			bool is_selected = (Filters::typeSelection == i);
			if (ImGui::Selectable(Filters::typeNames[i], is_selected))
			{
				Filters::typeSelection = i;
				PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Type of mobs to display");
	}

	ImGui::SameLine();
	ImGui::Text("Direction");
	ImGui::SameLine();
	ImGui::SameLine();
	if (ImGui::Button("Clear Highlights"))
	{
		EzCommand("/highlight reset");
	}

	ImGui::Text("Body");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Body", &Filters::bodyType))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Body type filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##BodyReverse", &Filters::bodyReverse))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse filter body type");
	}

	ImGui::SameLine();
	ImGui::Text("Race");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Race", &Filters::raceName))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Race filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##RaceReverse", &Filters::raceReverse))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse Filter Race");
	}

	ImGui::SameLine();
	ImGui::Text("Class");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputText("##Class", &Filters::className))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Class filter");
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("##ClassReverse", &Filters::classReverse))
	{
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Reverse Filter class");
	}
}
ImVec4 getConColor(SPAWNINFO * spawn)
{
	switch (ConColor(spawn))
	{
	case CONCOLOR_GREY:
		return ImVec4(0.6196f, 0.6196f, 0.6196f, 1.0f); // Grey
	case CONCOLOR_GREEN:
		return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);        // Green
	case CONCOLOR_LIGHTBLUE:
		return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);        // Light Blue
	case CONCOLOR_BLUE:
		return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);         // Blue
	case CONCOLOR_WHITE:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);        // White
	case CONCOLOR_YELLOW:
		return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);        // Yellow
	case CONCOLOR_RED:
	default:
		return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);        // Red
	}
}
void MobListImGui::drawMobRow(SPAWNINFO* spawn)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::Selectable(std::to_string(spawn->SpawnID).c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Right click to highlight\nLeft click to navigate self\nCtrl + left click to navigate group");
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			WriteChatf(PLUGIN_MSG "Highlighting mobs named \ar%s", spawn->DisplayedName);
			DoCommandf("/highlight \"%s\"", spawn->DisplayedName);
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			if (ImGui::IsKeyDown(ImGuiKey_ModCtrl))
			{
				WriteChatf(PLUGIN_MSG "Navigating \aogroup \awto \ar%s \awID \ar%d", spawn->DisplayedName, spawn->SpawnID);
				DoCommandf("/dgae /nav id %d", spawn->SpawnID);
			}
			else
			{
				WriteChatf(PLUGIN_MSG "Navigating \aoself \awto \ar%s \awID \ar%d", spawn->DisplayedName, spawn->SpawnID);
				DoCommandf("/nav id %d", spawn->SpawnID);
			}
		}
	}
	ImGui::TableNextColumn();
	ImGui::Text("%d", spawn->Level);
	ImGui::TableNextColumn();
	if (Filters::conColor)
	{
		auto color = getConColor(spawn);
		ImGui::TextColored(color, "%s", spawn->DisplayedName);
		ImGui::TableNextColumn();
		ImGui::TextColored(color, "%s", spawn->Name);
	}
	else
	{
		ImGui::Text("%s", spawn->DisplayedName);
		ImGui::TableNextColumn();
		ImGui::Text("%s", spawn->Name);
	}
	ImGui::TableNextColumn();
	//calculate distance to spawn
	float dist = GetDistance3D(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z, spawn->X, spawn->Y, spawn->Z);
	ImGui::Text("%.1f", dist);
	ImGui::TableNextColumn();
	ImGui::Text("%.1f, %.1f, %.1f", spawn->Y, spawn->X, spawn->Z);
	ImGui::TableNextColumn();
	//get body type
	ImGui::Text("%s", GetBodyTypeDesc(GetBodyType(spawn)));
	ImGui::TableNextColumn();
	ImGui::Text("%s", spawn->GetRaceString());
	ImGui::TableNextColumn();
	ImGui::Text("%s", spawn->GetClassString());
	if (Filters::directionArrow)
	{
		ImGui::TableNextColumn();
		drawDirectionalArrow(spawn, ImGui::GetCursorScreenPos());
	}
}

ImVec2 rotatePoint(ImVec2 point, float centerX, float centerY, float angle)
{
	float radians = angle * (PI / 180);
	float sinA = sin(radians);
	float cosA = cos(radians);

	float newX = cosA * (point.x - centerX) - sinA * (point.y - centerY) + centerX;
	float newY = sinA * (point.x - centerX) + cosA * (point.y - centerY) + centerY;
	
	return ImVec2(newX, newY);
}

float calculateAngleTo(SPAWNINFO* spawn)
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

void MobListImGui::drawDirectionalArrow(SPAWNINFO* spawn, ImVec2 cursorPos)
{
	float angle = calculateAngleTo(spawn);
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

void MobListImGui::drawMobListTable(std::vector<SPAWNINFO*>& spawnList)
{
	if (spawnList.empty())
	{
		ImGui::Text("Spawn list empty");
		return;
	}
		
	if (ImGui::BeginTable("##List table", 9 + Filters::directionArrow, MobListImGui::tableFlags))
	{
		ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_None, 0, eColumnId::Id);
		ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_None, 0, eColumnId::Lvl);
		ImGui::TableSetupColumn("Display Name", ImGuiTableColumnFlags_None, 0, eColumnId::DisplayName);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0, eColumnId::Name);
		ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_DefaultSort, 0, eColumnId::Distance);
		ImGui::TableSetupColumn("Loc", ImGuiTableColumnFlags_NoSort, 0, eColumnId::Loc);
		ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_None, 0, eColumnId::Body);
		ImGui::TableSetupColumn("Race", ImGuiTableColumnFlags_None, 0, eColumnId::Race);
		ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_None, 0, eColumnId::Class);
		if (Filters::directionArrow)
		{
			ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_NoSort, 0, eColumnId::Direction);
		}
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();
		ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();

		if (sortSpecs && sortSpecs->SpecsCount > 0)
		{
			const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs->Specs[0];
			eColumnId columnId = static_cast<eColumnId>(sortSpec.ColumnUserID);
			bool ascending = (sortSpec.SortDirection == ImGuiSortDirection_Ascending);

			sortSpawnList(spawnList, columnId, ascending);
		}
		
		ImGuiListClipper clipper;
		clipper.Begin(spawnList.size());

		while (clipper.Step())
		{
			for (int rowIndex = clipper.DisplayStart; rowIndex < clipper.DisplayEnd; ++rowIndex)
			{
				SPAWNINFO* spawn = spawnList[rowIndex];
				drawMobRow(spawn);
			}
		
		}
		ImGui::EndTable();
	}
}

void MobListImGui::drawOptMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Show Con Colors", NULL, Filters::conColor))
			{
				Filters::conColor = !Filters::conColor;
				WriteChatf("Show Con Colors is now \ag%s", Filters::conColor ? "enabled" : "disabled");
			}
			if (ImGui::MenuItem("Show Directional Arrow", NULL, Filters::directionArrow))
			{
				Filters::directionArrow = !Filters::directionArrow;
				WriteChatf("Show Directional Arrow is now \ag%s", Filters::directionArrow ? "enabled" : "disabled");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}
