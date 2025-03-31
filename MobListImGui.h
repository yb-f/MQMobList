#pragma once
#include "MQMobList.h"
#include <mq/Plugin.h>

void drawMobList(SpawnList& spawnList, Filters& filters);
void drawMenu(Filters& filters);
void drawSearchHeader(Filters& filters, SpawnList& spawnList);
void drawMobListTable(SpawnList& spawnList, Filters& filters);
void drawMobRow(const SpawnObject& spawn, const Filters& filters);
void drawDirectionalArrow(PlayerClient* spawn, const ImVec2& cursorPos);
