#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>

void drawMobList(std::vector<SPAWNINFO*>& spawnList, Filters& filters);
void drawMenu(Filters& filters);
void drawSearchHeader(Filters& filters);
void drawMobListTable(std::vector<SPAWNINFO*>& spawnList, Filters filters);
void drawMobRow(SPAWNINFO* spawn, Filters filters);
void drawDirectionalArrow(SPAWNINFO* spawn, ImVec2 cursorPos);
