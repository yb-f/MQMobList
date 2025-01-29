#pragma once

#include <mq/Plugin.h>
#define PLUGIN_MSG "\ay[\agMobList\ay]\aw "

/**
 * \brief A class for the filter criteria used in filtering the spawn list.
 * 
 * Members of this class are used for filtering spawns to create a list of only the desired spawns.
 */
class Filters {
public:
	/// Values do not change during execution 
	/// Filters by Type
	std::vector <std::pair<const char*, eSpawnType>> spawnTypeNames = {
			{"PC", PC},
			{"NPC", NPC},
			{"Untargetable", UNTARGETABLE},
			{"Mount", MOUNT},
			{"Pet", PET},
			{"Corpse", CORPSE},
			{"Chest", CHEST},
			{"Trigger", TRIGGER},
			{"Trap", TRAP},
			{"Timer", TIMER},
			{"Item", ITEM},
			{"Mercenary", MERCENARY},
			{"Aura", AURA},
			{"Object", OBJECT},
			{"Banner", BANNER},
			{"Campfire", CAMPFIRE},
			{"Flyer", FLYER}
	};
	int typeNameCount;
	int typeSelection;
	/// Filters by level 
	int levelLow;
	int levelHigh;
	/// Filters by name
	std::string name;
	bool nameReverse;
	/// Filters by distance
	int minDistance;
	int maxDistance;
	/// Filters by body type
	std::string bodyType;
	bool bodyReverse;
	/// Filters by race	
	std::string raceName;
	bool raceReverse;
	/// Filters by class
	std::string className;
	bool classReverse;
	/// UI Settings
	bool conColor = true;
	bool directionArrow = false;
	bool showMobListWindow = false
		;
	/// Sort triggers
	unsigned int prevColumn = -1;
	bool prevAscending;
	bool refreshTriggered = false;
	bool welcomeSent = false;
	/// Server Type
	int serverType;

	Filters() {
		typeNameCount = spawnTypeNames.size();
		levelLow = 1;
		levelHigh = 150;
		name = "";
		nameReverse = false;
		minDistance = 0;
		maxDistance = 10000;
		typeSelection = 1;
		bodyType = "";
		bodyReverse = false;
		raceName = "";
		raceReverse = false;
		className = "";
		classReverse = false;
		conColor = true;
		directionArrow = false;
		serverType = gBuild;
	}
	
	void resetFilters(Filters& filters)
	{
		filters = Filters();
	}
};

void createSpawnList();