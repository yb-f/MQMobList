#pragma once

#include <mq/Plugin.h>
#include "MobListImGui.h"
#define PLUGIN_MSG "\ay[\agMobList\ay]\aw "

class Filters {
public:
	static const char* typeNames[];
	static int typeNameCount;
	static int levelLow;
	static int levelHigh;
	static std::string name;
	static bool nameReverse;
	static int minDistance;
	static int maxDistance;
	static int typeSelection;
	static std::string bodyType;
	static bool bodyReverse;
	static std::string raceName;
	static bool raceReverse;
	static std::string className;
	static bool classReverse;
	static bool conColor;
	static bool directionArrow;

	static void initFilters();
};

extern std::chrono::steady_clock::time_point PulseTimer;