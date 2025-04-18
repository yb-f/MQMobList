#pragma once

#include <mq/Plugin.h>
#define PLUGIN_MSG "\ay[\agMobList\ay]\aw "

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

/**
 * \brief A class for the filter criteria used in filtering the spawn list.
 * 
 * Members of this class are used for filtering spawns to create a list of only the desired spawns.
 */
class Filters {
public:
	float debounceTimer = 0.0f;
	const float debounceDelay = 0.2f;
	/// Filters by Type
	static inline std::vector<std::pair<const char*, eSpawnType>> spawnTypeNames = {
		{"None", NONE},
		{"PC", PC},
		{"Mount", MOUNT},
		{"Pet", PET},
		{"PCPet", PCPET},
		{"NPCPet", NPCPET},
		{"XTarHater", XTARHATER},
		{"NPC", NPC},
		{"Corpse", CORPSE},
		{"Trigger", TRIGGER},
		{"Trap", TRAP},
		{"Timer", TIMER},
		{"Untargetable", UNTARGETABLE},
		{"Chest", CHEST},
		{"Item", ITEM},
		{"Aura", AURA},
		{"Object", OBJECT},
		{"Banner", BANNER},
		{"Campfire", CAMPFIRE},
		{"Mercenary", MERCENARY},
		{"Flyer", FLYER},
		{"NpcCorpse", NPCCORPSE},
		{"PcCorpse", PCCORPSE},
	};
	eSpawnType typeSelection = eSpawnType::NPC;
	/// Filters by level 
	int levelLow = 1;
	int levelHigh = 150;
	/// Filters by name
	std::string name;
	bool nameReverse = false;
	/// Filters by distance
	int minDistance = 0;
	int maxDistance = 10000;
	/// Filters by body type
	std::string bodyType;
	bool bodyReverse = false;
	/// Filters by race	
	std::string raceName;
	bool raceReverse = false;
	/// Filters by class
	std::string className;
	bool classReverse = false;
	/// UI Settings
	bool conColor = true;
	bool directionArrow = false;
	bool showMobListWindow = false;
	/// Theme Settings
	int playerWinThemeId = 10;
	bool roundPlayerWin = false;
	bool drawPicker = false;
	/// Sort triggers
	unsigned int prevColumn = -1;
	bool prevAscending = true;
	bool spawnAdded = false;
	bool welcomeSent = false;
	/// Server Type
	int serverType = gBuild;

	Filters() {}
	
	void resetFilters()
	{
		levelLow = 1;
		levelHigh = 150;
		name.clear();
		nameReverse = false;
		minDistance = 0;
		maxDistance = 10000;
		typeSelection = eSpawnType::NPC;
		bodyType.clear();
		bodyReverse = false;
		raceName.clear();
		raceReverse = false;
		className.clear();
		classReverse = false;
	}
};

class SpawnObject {
public:
	PlayerClient* m_pSpawn = nullptr; //pointer to the spawn
	bool m_bDisplayed = false; // should this spawn be displayed
	bool m_bFiltersMismatch = false; // Does this match the selected filters
	bool m_bDistanceMismatch = false;  // Is this within the selected distance
	float m_fDistanceSq = 0.0f; //Calculated distance to spawn SQUARED

	SpawnObject(PlayerClient* pSpawn)
		: m_pSpawn(pSpawn) {}

	void UpdateDistance(float x, float y, float z) {
		if (m_pSpawn) {
			m_fDistanceSq = Get3DDistanceSquared(m_pSpawn->X, m_pSpawn->Y, m_pSpawn->Z, x, y, z);
		}
	}

	bool CheckDistance(const Filters& filters) const {
		float maxDistSq = static_cast<float>(filters.maxDistance) * filters.maxDistance;
		float minDistSq = static_cast<float>(filters.minDistance) * filters.minDistance;

		return (m_fDistanceSq >= minDistSq && m_fDistanceSq <= maxDistSq);
	}

	void UpdateDistanceFlag(const Filters& filters) {
		if (m_bFiltersMismatch) {
			return;
		}
		UpdateDistance(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z);

		if (!CheckDistance(filters)) {
			m_bDistanceMismatch = true;
			m_bDisplayed = false;
		}
		else {
			m_bDistanceMismatch = false;
			m_bDisplayed = true;
		}
	}

	float GetDisplayDistance() const {
		return sqrtf(m_fDistanceSq);
	}
};

class SpawnList {
private:
	std::vector<SpawnObject> spawns;

public:
	void AddSpawn(PlayerClient* pSpawn, Filters& filters) {
		spawns.push_back(SpawnObject(pSpawn));
		matchAllFilters(spawns.back(), filters);
		filters.spawnAdded = true;
	}

	void RemoveSpawn(PlayerClient* pSpawn) {
		auto it = std::find_if(spawns.begin(), spawns.end(),
			[pSpawn](const SpawnObject& obj) { return obj.m_pSpawn == pSpawn; });

		if (it != spawns.end()) {
			spawns.erase(it);  // Removes the spawn from the list
		}
	}

	void Clear() {
		spawns.clear();
	}

	bool matchFilter(std::string_view spawnValue, std::string_view filterValue, bool reverseFilter)
	{
		if (filterValue.empty())
			return true;
		bool isMatch = (ci_find_substr(spawnValue, filterValue) != -1);
		return reverseFilter ? !isMatch : isMatch;
	}

	void matchAllFilters(SpawnObject& pSpawnObject, const Filters& filters)
	{
		PlayerClient* pSpawn = pSpawnObject.m_pSpawn;

		bool nonDistMismatch = false;
		if (GetSpawnType(pSpawnObject.m_pSpawn) != filters.typeSelection)
		{
			nonDistMismatch = true;
		}
		else if(pSpawn->Level < filters.levelLow || pSpawn->Level > filters.levelHigh)
		{
			nonDistMismatch = true;
		}
		else if (!filters.name.empty() &&
			!matchFilter(pSpawn->DisplayedName, filters.name, filters.nameReverse) &&
			!matchFilter(pSpawn->Name, filters.name, filters.nameReverse))
		{
			nonDistMismatch = true;
		}
		else if (!filters.raceName.empty() && !matchFilter(pSpawn->GetRaceString(), filters.raceName, filters.raceReverse))
		{
			nonDistMismatch = true;
		}
		else if (!filters.className.empty() && !matchFilter(pSpawn->GetClassString(), filters.className, filters.classReverse))
		{
			nonDistMismatch = true;
		}
		else if (!filters.bodyType.empty() && !matchFilter(GetBodyTypeDesc(GetBodyType(pSpawn)), filters.bodyType, filters.bodyReverse))
		{
			nonDistMismatch = true;
		}
		if (nonDistMismatch)
		{
			pSpawnObject.m_bFiltersMismatch = true;
			pSpawnObject.m_bDisplayed = false;
			return;
		}
		pSpawnObject.m_bFiltersMismatch = false;

		pSpawnObject.UpdateDistance(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z);

		if (!pSpawnObject.CheckDistance(filters)) {
			pSpawnObject.m_bDistanceMismatch = true;
			pSpawnObject.m_bDisplayed = false;
		}
		else {
			pSpawnObject.m_bDistanceMismatch = false;
			pSpawnObject.m_bDisplayed = true;
		}
	}

	void updateDisplayedSpawns(const Filters& filters)
	{
		for (auto& spawnObject : spawns)
		{
			matchAllFilters(spawnObject, filters);
		}
	}

	void updateSpawnDistances(const Filters& filters)
	{
		for (auto& spawnObject : spawns)
		{
			spawnObject.UpdateDistanceFlag(filters);
		}
	}

	void sortSpawns(ImGuiID column, bool ascending, Filters& filters) {
		if (column != filters.prevColumn || ascending != filters.prevAscending || filters.spawnAdded) {
			std::sort(spawns.begin(), spawns.end(), [&](const SpawnObject& a, const SpawnObject& b) {
				if (a.m_bDisplayed != b.m_bDisplayed)
					return a.m_bDisplayed > b.m_bDisplayed;
				PlayerClient* spawnA = a.m_pSpawn;
				PlayerClient* spawnB = b.m_pSpawn;
				
				switch (column) {
				case TableColumnID::ID:
					return ascending ? spawnA->SpawnID < spawnB->SpawnID : spawnA->SpawnID > spawnB->SpawnID;
				case TableColumnID::Level:
					return ascending ? spawnA->Level < spawnB->Level : spawnA->Level > spawnB->Level;
				case TableColumnID::DisplayName: {
					int cmp = ci_string_compare(spawnA->DisplayedName, spawnB->DisplayedName);
					return ascending ? cmp < 0 : cmp > 0;
				}
				case TableColumnID::Name: {
					int cmp = ci_string_compare(spawnA->Name, spawnB->Name);
					return ascending ? cmp < 0 : cmp > 0;
				}
				case TableColumnID::Surname: {
					int cmp = ci_string_compare(spawnA->Lastname, spawnB->Lastname);
					return ascending ? cmp < 0 : cmp > 0;
				}
				case TableColumnID::Distance: {
					float distA = GetDistance3D(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z, spawnA->X, spawnA->Y, spawnA->Z);
					float distB = GetDistance3D(pLocalPlayer->X, pLocalPlayer->Y, pLocalPlayer->Z, spawnB->X, spawnB->Y, spawnB->Z);
					return ascending ? distA < distB : distA > distB;
				}
				case TableColumnID::Body: {
					int cmp = ci_string_compare(GetBodyTypeDesc(GetBodyType(spawnA)), GetBodyTypeDesc(GetBodyType(spawnB)));
					return ascending ? cmp < 0 : cmp > 0;
				}
				case TableColumnID::Race: {
					int cmp = ci_string_compare(spawnA->GetRaceString(), spawnB->GetRaceString());
					return ascending ? cmp < 0 : cmp > 0;
				}
				case TableColumnID::Class: {
					int cmp = ci_string_compare(spawnA->GetClassString(), spawnB->GetClassString());
					return ascending ? cmp < 0 : cmp > 0;
				}
				default:
					return false;
				}
			});
			filters.prevColumn = column;
			filters.prevAscending = ascending;
			filters.spawnAdded = false;
		}
	}

	bool empty() const {
		return spawns.empty();
	}

	size_t size() const {
		return spawns.size();
	}

	int visibleSize() const {
		int count = 0;
		for (const SpawnObject& spawn : spawns) {
			if (spawn.m_bDisplayed) {
				count++;
			}
		}
		return count;
	}

	SpawnObject& operator[](size_t index) { return spawns[index]; }

	const std::vector<SpawnObject>& getSpawns() const {
		return spawns;
	}
};

void createSpawnList();

void drawMobList(SpawnList& spawnList, Filters& filters);
void drawMenu(Filters& filters);
void drawSearchHeader(Filters& filters, SpawnList& spawnList);
void drawMobListTable(SpawnList& spawnList, Filters& filters);
void drawMobRow(const SpawnObject& spawn, const Filters& filters);
void drawDirectionalArrow(PlayerClient* spawn, const ImVec2& cursorPos);