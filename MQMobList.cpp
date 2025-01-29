
// MQMobList.cpp : Defines the entry point for the DLL application.
//

#include <mq/Plugin.h>

#include "MobListImGui.h"
#include "MQMobList.h"
#include <shared_mutex>

PreSetup("MQMobList");
PLUGIN_VERSION(0.1);

std::vector<SPAWNINFO*> spawnList;

Filters filters;

bool matchFilter(std::string_view value, std::string_view filter, bool reverseFilter)
{
	if (filter.empty()) return true;

	bool isMatch = (ci_find_substr(value, filter) != -1);

	return reverseFilter ? !isMatch : isMatch;
}

/**
 * /brief Performs comparisons for all filters against a given spawn
 * 
 * This filter checks a series of filters (distance, name, class, race, body type) against a given spawn.
 * If any of the matches are failed the spawn is invalid and we return false.
 * 
 * \param pSpawn A pointer to the spawn object that is being checked
 * \return Returns true if all filters are matched, or false if not.
 */
bool matchAllFilters(SPAWNINFO* pSpawn)
{
	if (pSpawn == nullptr)
		return false;
	
	/// Compare spawn type of pSpawn with our filter.
	if (GetSpawnType(pSpawn) != filters.spawnTypeNames[filters.typeSelection].second)
		return false;

	float distSquared = Get3DDistanceSquared(pSpawn->X, pSpawn->Y, pSpawn->Z, pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z);
	/// Compare dist^2 against minDist^2 and maxDist^2
	if (distSquared < filters.minDistance * filters.minDistance || distSquared > filters.maxDistance * filters.maxDistance)
		return false;

	if (filters.name[0] != '\0' && !matchFilter(pSpawn->DisplayedName, filters.name, filters.nameReverse))
	{
		/// fail name check
		return false;
	}

	if (filters.className[0] != '\0' && !matchFilter(pSpawn->GetClassString(), filters.className, filters.classReverse))
	{
		/// fail class name check
		return false;
	}

	if (filters.raceName[0] != '\0' && !matchFilter(pSpawn->GetRaceString(), filters.raceName, filters.raceReverse))
	{
		/// fail race name check
		return false;
	}

	if (filters.bodyType[0] != '\0' && !matchFilter(GetBodyTypeDesc(GetBodyType(pSpawn)), filters.bodyType, filters.bodyReverse))
	{
		/// fail body type check
		return false;
	}
	/// Spawn matches all filters
	return true;
}

/**
 * /brief Generate a list of spawns matching the currently set filters
 * 
 * Iterate over all spawns and compare them against currently set filters. Add spawns matching these filters to spawnList
 * 
 */
void createSpawnList()
{
	spawnList.clear();
	SPAWNINFO* pSpawn = pSpawnList;
	while (pSpawn)
	{
		if (matchAllFilters(pSpawn))
		{
			/// filters matched, add spawn to spawn list.
			spawnList.emplace_back(pSpawn);
		}
		pSpawn = pSpawn->pNext;
	}
	filters.refreshTriggered = true;
}

/**
 * \brief Prints the help message for the `/moblist help` command.
 * 
 * Prints a list of valid commands and their function for the MobList plugin. 
 */
void printHelp()
{
	WriteChatf(PLUGIN_MSG "\ar/moblist show		\ag--- Show UI");
	WriteChatf(PLUGIN_MSG "\ar/moblist hide		\ag--- Hide UI");
	WriteChatf(PLUGIN_MSG "\ar/moblist refresh	\ag--- Refresh spawn list");
	WriteChatf(PLUGIN_MSG "\ar/moblist reset	\ag--- Reset all filters");
	WriteChatf(PLUGIN_MSG "\ar/moblist help		\ag--- Show this help message");
}

/**
 * \brief Command handler for the plugin's `/ml` command.
 * 
 * Handle commands issued via the `/ml` command.
 *  * 
 * \param pChar Pointer to the PlayerClient. (the character issuing the command)
 * \param szLine The command issued
 * 
 * \note  Valid commands are:
 *	-/moblist show: Show UI
 *	-/moblist hide: Hide UI
 *	-/moblist refresh: Refresh spawn list
 *	-/moblist reset: Reset filters
 *	-/moblist help: Display help
 */
void moblistCmd(PlayerClient* pChar, const char* szLine) {
	char arg[MAX_STRING] = {};
	GetMaybeQuotedArg(arg, MAX_STRING, szLine, 1);
	if (arg[0] != '\0')
	{
		if (ci_equals(arg, "help"))
		{
			printHelp();
			return;
		}
		if (ci_equals(arg, "show"))
		{
			filters.showMobListWindow = true;
			return;
		}
		if (ci_equals(arg, "hide"))
		{
			filters.showMobListWindow = false;
			return;
		}
		if (ci_equals(arg, "refresh"))
		{
			createSpawnList();
		}
		if (ci_equals(arg, "reset"))
		{
			filters.resetFilters(filters);
			return;
		}
	}
	else
	{
		printHelp();
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
	AddCommand("/moblist", moblistCmd);
	// AddMQ2Data("mytlo", MyTLOData);
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQTaskHud::Shutting down");
	RemoveCommand("/moblist");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
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
	createSpawnList();
	// DebugSpewAlways("MQtest::OnAddSpawn(%s)", pNewSpawn->Name);
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
	createSpawnList();
	// DebugSpewAlways("MQtest::OnRemoveSpawn(%s)", pSpawn->Name);
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
	// DebugSpewAlways("MQtest::OnZoned()");
	createSpawnList();
}

PLUGIN_API void OnPulse()
{
	if (!filters.welcomeSent)
	{
		filters.welcomeSent = true;
		WriteChatf(PLUGIN_MSG "Welcome to MQ Mob List.");
		WriteChatf(PLUGIN_MSG "Use \ay/moblist help\aw to show help.");
	}
	// Run only after timer is up
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		drawMobList(spawnList, filters);
	}
}
