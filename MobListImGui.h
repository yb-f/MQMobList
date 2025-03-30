#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>

void drawMobList(SpawnList& spawnList, Filters& filters);
void drawMenu(Filters& filters);
void drawSearchHeader(Filters& filters, SpawnList& spawnList);
void drawMobListTable(SpawnList& spawnList, Filters filters);
void drawMobRow(const SpawnObject& spawn, Filters filters);
void drawDirectionalArrow(PlayerClient* spawn, ImVec2 cursorPos);
