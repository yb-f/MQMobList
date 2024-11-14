// MQMobList.cpp : Defines the entry point for the DLL application.
//

#include <mq/Plugin.h>

#include "MobListImGui.h"
#include "MQMobList.h"
#include <shared_mutex>

PreSetup("MQMobList");
PLUGIN_VERSION(0.1);

std::shared_mutex spawnListMutex;
std::vector<SPAWNINFO*> spawnList;
std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();

namespace Filters {
	const char* typeNames[] = { "PC", "NPC", "Untargetable", "Mount", "Pet", "Corpse", "Chest", "Trigger", "Trap",
		"Timer", "Item", "Mercenary", "Aura", "Object", "Banner", "Campfire", "Flyer" };
	int typeNameCount = 17;
	int Filters::levelLow = 1;
	int Filters::levelHigh = 150;
	std::string Filters::name = "";
	bool Filters::nameReverse = false;
	int Filters::minDistance = 0;
	int Filters::maxDistance = 10000;
	bool Filters::directionArrow = false;
	int Filters::typeSelection = 1;
	std::string Filters::bodyType = "";
	bool Filters::bodyReverse = false;
	std::string Filters::raceName = "";
	bool Filters::raceReverse = false;
	std::string Filters::className = "";
	bool Filters::classReverse = false;
	bool Filters::conColor = true;
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

void createSpawnList()
{
	std::unique_lock<std::shared_mutex> writeLock(spawnListMutex);

	auto setStringField = [](const std::string& src, char* dest, size_t maxLength) {
		if (!src.empty()) {
			std::strncpy(dest, src.c_str(), maxLength - 1); // Copy up to maxLength - 1 characters
			dest[maxLength - 1] = '\0'; // Ensure null-termination
		}
		else {
			dest[0] = '\0'; // Clear the field if the source is empty
		}
	};
	spawnList.clear();
	MQSpawnSearch SearchSpawn;
	ClearSearchSpawn(&SearchSpawn);

	SearchSpawn.SpawnType = getSpawnTypeFromSelection(Filters::typeSelection);
	SearchSpawn.MinLevel = Filters::levelLow;
	SearchSpawn.MaxLevel = Filters::levelHigh;
	//SearchSpawn.Radius = Filters::maxDistance;
	for (int i = 1; i <= CountMatchingSpawns(&SearchSpawn, pLocalPlayer, false); ++i)
	{
		bool addToSpawnList = true;
		SPAWNINFO* pSpawn = NthNearestSpawn(&SearchSpawn, i, pLocalPlayer, false);
		if (SpawnMatchesSearch(&SearchSpawn, pLocalPlayer, pSpawn))
		{
			// Level check
			float dist = GetDistance3D(pSpawn->X, pSpawn->Y, pSpawn->Z, pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z);
			if (dist < Filters::minDistance || dist > Filters::maxDistance)
			{
				continue;
			}
			// Name filter check
			if (!Filters::name.empty())
			{
				if (!Filters::nameReverse)
				{
					if (ci_find_substr(pSpawn->DisplayedName, Filters::name) == -1) // If the name is NOT found in the DisplayedName
					{
						addToSpawnList = false;
					}
				}
				else
				{
					if (ci_find_substr(pSpawn->DisplayedName, Filters::name) != -1) // If the name IS found in the DisplayedName
					{
						addToSpawnList = false;
					}
				}
			}
			// Class type filter check
			if (!Filters::className.empty())
			{
				if (!Filters::classReverse)
				{
					if (ci_find_substr(pSpawn->GetClassString(), Filters::className) == -1) // If the class name is NOT found in the GetClassString
					{
						addToSpawnList = false; 
					}
				}
				else 
				{
					if (ci_find_substr(pSpawn->GetClassString(), Filters::className) != -1) // If the class name IS found in the GetClassString
					{
						addToSpawnList = false;
					}
				}
			}
			// Race filter check
			if (!Filters::raceName.empty())
			{
				if (!Filters::raceReverse)
				{
					if (ci_find_substr(pSpawn->GetRaceString(), Filters::raceName) == -1) // If the race name is NOT found in the GetRaceString
					{
						addToSpawnList = false;
					}
				}
				else
				{
					if (ci_find_substr(pSpawn->GetRaceString(), Filters::raceName) != -1) // If the race name IS found in the GetRaceString
					{
						addToSpawnList = false;
					}
				}
			}

			// Body filter check
			if (!Filters::bodyType.empty())
			{
				if (!Filters::bodyReverse)
				{
					if (ci_find_substr(GetBodyTypeDesc(GetBodyType(pSpawn)), Filters::bodyType) == -1) // If the body type is NOT found
					{
						addToSpawnList = false;
					}
				}
				else
				{
					if (ci_find_substr(GetBodyTypeDesc(GetBodyType(pSpawn)), Filters::bodyType) != -1) // If the body type IS found
					{
						addToSpawnList = false;
					}
				}
			}

			// If spawn passes all checks, add it to the list
			if (addToSpawnList)
			{
				spawnList.emplace_back(pSpawn);
			}
		}
	}
}


/**
 * Avoid Globals if at all possible, since they persist throughout your program.
 * But if you must have them, here is the place to put them.
 */
// bool ShowMQMobListWindow = true;

/**
 * @fn InitializePlugin
 *
 * This is called once on plugin initialization and can be considered the startup
 * routine for the plugin.
 */
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQMobList::Initializing version %f", MQ2Version);
	if (GetGameState() == GAMESTATE_INGAME)
	{
		createSpawnList();
	}
	// Examples:
	// AddCommand("/mycommand", MyCommand);
	// AddXMLFile("MQUI_MyXMLFile.xml");
	// AddMQ2Data("mytlo", MyTLOData);
}

/**
 * @fn ShutdownPlugin
 *
 * This is called once when the plugin has been asked to shutdown.  The plugin has
 * not actually shut down until this completes.
 */
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQMobList::Shutting down");

	// Examples:
	// RemoveCommand("/mycommand");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
}

PLUGIN_API void SetGameState(int GameState)
{
	// DebugSpewAlways("MQMobList::SetGameState(%d)", GameState);
}

/**
 * @fn OnPulse
 *
 * This is called each time MQ2 goes through its heartbeat (pulse) function.
 *
 * Because this happens very frequently, it is recommended to have a timer or
 * counter at the start of this call to limit the amount of times the code in
 * this section is executed.
 */
PLUGIN_API void OnPulse()
{
	static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
	// Run only after timer is up
	if (std::chrono::steady_clock::now() > PulseTimer)
	{
		// Wait 5 seconds before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(2500);
		DebugSpewAlways("MQMobList::OnPulse()");
		std::thread spawnListThread([]() {
			createSpawnList();
			});
		spawnListThread.detach();
	}
}

/**
 * @fn OnAddSpawn
 *
 * This is called each time a spawn is added to a zone (ie, something spawns). It is
 * also called for each existing spawn when a plugin first initializes.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnEndZone is
 * called and before @ref OnZoned is called.
 *
 * @param pNewSpawn PSPAWNINFO - The spawn that was added
 */
PLUGIN_API void OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	// DebugSpewAlways("MQMobList::OnAddSpawn(%s)", pNewSpawn->Name);
}

/**
 * @fn OnRemoveSpawn
 *
 * This is called each time a spawn is removed from a zone (ie, something despawns
 * or is killed).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnBeginZone is
 * called.
 *
 * @param pSpawn PSPAWNINFO - The spawn that was removed
 */
PLUGIN_API void OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	// DebugSpewAlways("MQMobList::OnRemoveSpawn(%s)", pSpawn->Name);
}

/**
 * @fn OnAddGroundItem
 *
 * This is called each time a ground item is added to a zone (ie, something spawns).
 * It is also called for each existing ground item when a plugin first initializes.
 *
 * When zoning, this is called for all ground items in the zone after @ref OnEndZone
 * is called and before @ref OnZoned is called.
 *
 * @param pNewGroundItem PGROUNDITEM - The ground item that was added
 */
PLUGIN_API void OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
	// DebugSpewAlways("MQMobList::OnAddGroundItem(%d)", pNewGroundItem->DropID);
}

/**
 * @fn OnRemoveGroundItem
 *
 * This is called each time a ground item is removed from a zone (ie, something
 * despawns or is picked up).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all ground items in the zone after
 * @ref OnBeginZone is called.
 *
 * @param pGroundItem PGROUNDITEM - The ground item that was removed
 */
PLUGIN_API void OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
	// DebugSpewAlways("MQMobList::OnRemoveGroundItem(%d)", pGroundItem->DropID);
}

/**
 * @fn OnBeginZone
 *
 * This is called just after entering a zone line and as the loading screen appears.
 */
PLUGIN_API void OnBeginZone()
{
	// DebugSpewAlways("MQMobList::OnBeginZone()");
}

/**
 * @fn OnEndZone
 *
 * This is called just after the loading screen, but prior to the zone being fully
 * loaded.
 *
 * This should occur before @ref OnAddSpawn and @ref OnAddGroundItem are called. It
 * always occurs before @ref OnZoned is called.
 */
PLUGIN_API void OnEndZone()
{
	// DebugSpewAlways("MQMobList::OnEndZone()");
}

/**
 * @fn OnZoned
 *
 * This is called after entering a new zone and the zone is considered "loaded."
 *
 * It occurs after @ref OnEndZone @ref OnAddSpawn and @ref OnAddGroundItem have
 * been called.
 */
PLUGIN_API void OnZoned()
{
	// DebugSpewAlways("MQMobList::OnZoned()");
}

PLUGIN_API void OnUpdateImGui()
{
	MobListImGui::drawMobList(spawnList);
}
