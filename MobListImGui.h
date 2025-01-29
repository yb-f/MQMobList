#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>

enum TableColumnID {
	ID = 0,
	Level = 1,
	DisplayName = 2,
	Name = 3,
	Surname = 4,
	Distance = 5,
	Location = 6,
	Body = 7,
	Race = 8,
	Class = 9,
	Direction = 10
};

void drawMobList(std::vector<SPAWNINFO*>& spawnList, Filters& filters);
void drawMenu(Filters& filters);
void drawSearchHeader(Filters& filters);
void drawMobListTable(std::vector<SPAWNINFO*>& spawnList, Filters filters);
void drawMobRow(SPAWNINFO* spawn, Filters filters);
void drawDirectionalArrow(SPAWNINFO* spawn, ImVec2 cursorPos);
