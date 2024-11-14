#include "MobListImGui.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "MQMobList.h"
#include <shared_mutex>

extern std::shared_mutex spawnListMutex;

bool showMobListWindow = true;

void MobListImGui::drawMobList(const std::vector<SPAWNINFO*>& spawnList)
{
	if (ImGui::Begin("Mob List", &showMobListWindow, ImGuiWindowFlags_None))
	{
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
	ImGui::Checkbox("##DirectionArrow", &Filters::directionArrow);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Display directional arrow");
	}
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

//TODO  add sort functionality, and arrow
void MobListImGui::drawMobListTable(const std::vector<SPAWNINFO*>& spawnList)
{
	std::shared_lock<std::shared_mutex> readLock(spawnListMutex);

	if (spawnList.empty())
	{
		ImGui::Text("Spawn list empty");
	}
	else
	{
		if (ImGui::BeginTable("##List table", 10, MobListImGui::tableFlags))
		{
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_None, 0, eColumnId::Id);
			ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_None, 0, eColumnId::Lvl);
			ImGui::TableSetupColumn("Display Name", ImGuiTableColumnFlags_None, 0, eColumnId::DisplayName);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0, eColumnId::Name);
			ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_None, 0, eColumnId::Distance);
			ImGui::TableSetupColumn("Loc", ImGuiTableColumnFlags_None, 0, eColumnId::Loc);
			ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_None, 0, eColumnId::Body);
			ImGui::TableSetupColumn("Race", ImGuiTableColumnFlags_None, 0, eColumnId::Race);
			ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_None, 0, eColumnId::Class);
			ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_None, 0, eColumnId::Direction);
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			for (auto& spawn : spawnList)
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
						DoCommandf("/highlight '%s'", spawn->DisplayedName);
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
				ImGui::Text("%s", spawn->DisplayedName);
				ImGui::TableNextColumn();
				ImGui::Text("%s", spawn->Name);
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
				ImGui::TableNextColumn();
				ImGui::Text("Directional arrow goes here");

			}
			ImGui::EndTable();
		}
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
