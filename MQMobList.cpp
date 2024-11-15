// MQMobList.cpp : Defines the entry point for the DLL application.
//

#include <mq/Plugin.h>

#include "MobListImGui.h"
#include "MQMobList.h"
#include <shared_mutex>

PreSetup("MQMobList");
PLUGIN_VERSION(0.1);

std::vector<SPAWNINFO*> spawnList;

std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();


const char* Filters::typeNames[] = {
	"PC", "NPC", "Untargetable", "Mount", "Pet", "Corpse", "Chest", "Trigger", "Trap",
	"Timer", "Item", "Mercenary", "Aura", "Object", "Banner", "Campfire", "Flyer"
};
int Filters::typeNameCount = 17;
int Filters::levelLow = 1;
int Filters::levelHigh = 150;
std::string Filters::name = "";
bool Filters::nameReverse = false;
int Filters::minDistance = 0;
int Filters::maxDistance = 10000;
int Filters::typeSelection = 1;
std::string Filters::bodyType = "";
bool Filters::bodyReverse = false;
std::string Filters::raceName = "";
bool Filters::raceReverse = false;
std::string Filters::className = "";
bool Filters::classReverse = false;
bool Filters::conColor = false;
bool Filters::directionArrow = false;

void Filters::initFilters()
{
	const char* typeNames[] = { "PC", "NPC", "Untargetable", "Mount", "Pet", "Corpse", "Chest", "Trigger", "Trap",
	"Timer", "Item", "Mercenary", "Aura", "Object", "Banner", "Campfire", "Flyer" };
	int typeNameCount     = 17;
	int levelLow          = 1;
	int levelHigh         = 150;
	std::string name      = "";
	bool nameReverse      = false;
	int minDistance       = 0;
	int maxDistance       = 10000;
	int typeSelection     = 1;
	std::string bodyType  = "";
	bool bodyReverse      = false;
	std::string raceName  = "";
	bool raceReverse      = false;
	std::string className = "";
	bool classReverse     = false;
	bool conColor         = false;
	bool directionArrow   = false;
}

eSpawnType getSpawnTypeFromSelection(int typeSelection)
{
	switch (typeSelection)
	{
	case 0: return eSpawnType::PC;
	case 1: return eSpawnType::NPC;
	case 2: return eSpawnType::UNTARGETABLE;
	case 3: return eSpawnType::MOUNT;
	case 4: return eSpawnType::PET;
	case 5: return eSpawnType::CORPSE;
	case 6: return eSpawnType::CHEST;
	case 7: return eSpawnType::TRIGGER;
	case 8: return eSpawnType::TRAP;
	case 9: return eSpawnType::TIMER;
	case 10: return eSpawnType::ITEM;
	case 11: return eSpawnType::MERCENARY;
	case 12: return eSpawnType::AURA;
	case 13: return eSpawnType::OBJECT;
	case 14: return eSpawnType::BANNER;
	case 15: return eSpawnType::CAMPFIRE;
	case 16: return eSpawnType::FLYER;
	default: return eSpawnType::NONE;
	}
}

bool matchFilter(const std::string& value, const std::string& filter, bool reverseFilter)
{
	if (filter.empty()) return true;
	bool isMatch = (ci_find_substr(value, filter) != -1);

	return reverseFilter ? !isMatch : isMatch;
}

bool matchAllFilters(SPAWNINFO* pSpawn)
{
	float dist = GetDistance3D(pSpawn->X, pSpawn->Y, pSpawn->Z, pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z);
	if (dist < Filters::minDistance || dist > Filters::maxDistance)
	{
		//fail distance check
		return false;
	}
	if (!Filters::name.empty() && !matchFilter(pSpawn->DisplayedName, Filters::name, Filters::nameReverse))
	{
		return false;
	}

	if (!Filters::className.empty() && !matchFilter(pSpawn->GetClassString(), Filters::className, Filters::classReverse))
	{
		return false;
	}

	if (!Filters::raceName.empty() && !matchFilter(pSpawn->GetRaceString(), Filters::raceName, Filters::raceReverse))
	{
		return false;
	}

	if (!Filters::bodyType.empty() && !matchFilter(GetBodyTypeDesc(GetBodyType(pSpawn)), Filters::bodyType, Filters::bodyReverse))
	{
		return false;
	}
	return true;
}

void createSpawnList()
{
	
	spawnList.clear();
	SPAWNINFO* pSpawn = pSpawnList;
	MQSpawnSearch SearchSpawn;
	ClearSearchSpawn(&SearchSpawn);
	SearchSpawn.SpawnType = getSpawnTypeFromSelection(Filters::typeSelection);
	SearchSpawn.MinLevel = Filters::levelLow;
	SearchSpawn.MaxLevel = Filters::levelHigh;
	while (pSpawn)
	{
		if (SpawnMatchesSearch(&SearchSpawn, pLocalPlayer, pSpawn))
		{
			if (matchAllFilters(pSpawn))
			{
				spawnList.emplace_back(pSpawn);
			}
		}
		pSpawn = pSpawn->pNext;
	}
}

void mlCmd(PlayerClient* pChar, const char* szLine) {
	char arg[MAX_STRING] = {};
	GetMaybeQuotedArg(arg, MAX_STRING, szLine, 1);
	if (strlen(arg))
	{
		if (ci_equals(arg, "help"))
		{
			WriteChatf(PLUGIN_MSG "\ar/ml show	\ag--- Show UI");
			WriteChatf(PLUGIN_MSG "\ar/ml hide	\ag--- Hide UI");
			WriteChatf(PLUGIN_MSG "\r/ml reset	\ag--- Reset all filters");
			WriteChatf(PLUGIN_MSG "\r/ml help	\ag--- Show this help message");
			return;
		}
		if (ci_equals(arg, "show"))
		{
			showMobListWindow = true;
			return;
		}
		if (ci_equals(arg, "hide"))
		{
			showMobListWindow = false;
			return;
		}
		if (ci_equals(arg, "reset"))
		{
			Filters::initFilters();
			return;
		}
	}
	else
	{
		WriteChatf(PLUGIN_MSG "\ar/ml show	\ag--- Show UI");
		WriteChatf(PLUGIN_MSG "\ar/ml hide	\ag--- Hide UI");
		WriteChatf(PLUGIN_MSG "\r/ml reset	\ag--- Reset all filters");
		WriteChatf(PLUGIN_MSG "\r/ml help	\ag--- Show this help message");
	}
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQMobList::Initializing version %f", MQ2Version);
	if (GetGameState() == GAMESTATE_INGAME)
	{
		createSpawnList();
	}
	// Examples:
	AddCommand("/ml", mlCmd);
	// AddXMLFile("MQUI_MyXMLFile.xml");
	// AddMQ2Data("mytlo", MyTLOData);
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQTaskHud::Shutting down");
	RemoveCommand("/ml");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
}

PLUGIN_API void OnPulse()
{
	// Run only after timer is up
	if (std::chrono::steady_clock::now() > PulseTimer)
	{
		// Wait 5 seconds before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::seconds(1);
		if (GetGameState() == GAMESTATE_INGAME)
		{
			createSpawnList();
		}
	}
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		MobListImGui::drawMobList(spawnList);
	}
}
