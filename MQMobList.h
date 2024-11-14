#pragma once

#include <mq/Plugin.h>
#include "MobListImGui.h"
#define PLUGIN_MSG "\ay[\agMobList\ay]\aw "
namespace Filters {
	extern const char* typeNames[];
	extern int typeNameCount;
	extern int levelLow;
	extern int levelHigh;
	extern std::string name;
	extern bool nameReverse;
	extern int minDistance;
	extern int maxDistance;
	extern bool directionArrow;
	extern int typeSelection;
	extern std::string bodyType;
	extern bool bodyReverse;
	extern std::string raceName;
	extern bool raceReverse;
	extern std::string className;
	extern bool classReverse;
	extern bool conColor;
};

extern std::chrono::steady_clock::time_point PulseTimer;