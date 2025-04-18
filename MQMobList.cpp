
// MQMobList.cpp : Defines the entry point for the DLL application.
//

#include "MQMobList.h"
#include <mq/Plugin.h>

PreSetup("MQMobList");
PLUGIN_VERSION(0.2);

SpawnList spawnList;
uint32_t bmMobListRefresh;
bool bZoning = false;

Filters filters;

/**
 * /brief Generate a list of spawns matching the currently set filters
 * 
 * Iterate over all spawns and compare them against currently set filters. Add spawns matching these filters to spawnList
 * 
 */
void createSpawnList()
{
	spawnList.Clear();
	PlayerClient* pSpawn = pSpawnList;
	while (pSpawn)
	{
		spawnList.AddSpawn(pSpawn, filters);
		pSpawn = pSpawn->pNext;
	}
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
			return;
		}
		if (ci_equals(arg, "reset"))
		{
			filters.resetFilters();
			return;
		}
	}
	else
	{
		printHelp();
	}
}

PLUGIN_API void OnBeginZone()
{
	spawnList.Clear();
	bZoning = true;
	// DebugSpewAlways("MQImMap::OnBeginZone()");
}

PLUGIN_API void OnEndZone()
{
	bZoning = false;
}
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQMobList::Initializing version %f", MQ2Version);
	// Examples:
	AddCommand("/moblist", moblistCmd);
	bmMobListRefresh = AddMQ2Benchmark("Moblist Refresh ImGui");
	// AddMQ2Data("mytlo", MyTLOData);
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQMobList::Shutting down");
	RemoveCommand("/moblist");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
}

PLUGIN_API void OnAddSpawn(PlayerClient* pSpawn)
{
	spawnList.AddSpawn(pSpawn, filters);
}

PLUGIN_API void OnRemoveSpawn(PlayerClient* pSpawn)
{
	spawnList.RemoveSpawn(pSpawn);
}

PLUGIN_API void OnPulse()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!filters.welcomeSent && GetGameState() == GAMESTATE_INGAME)
		{
			filters.welcomeSent = true;
			WriteChatf(PLUGIN_MSG "Welcome to MQ Mob List.");
			WriteChatf(PLUGIN_MSG "Use \ay/moblist help\aw to show help.");
		}
	}

}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME && !bZoning)
	{
		EnterMQ2Benchmark(bmMobListRefresh);
		drawMobList(spawnList, filters);
		ExitMQ2Benchmark(bmMobListRefresh);
	}
}
