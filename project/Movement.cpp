#include "Movement.h"

using namespace sc2;

//take in two buildings with addons and swap them. Used only for Terran.

bool Movement::RallyToRamp(Bot * bot, const Unit * building) {
	if (building == NULL) {
		return false;
	}
	Point2D rallyTarget;

	switch (bot->spawnCorner) {
	case SPAWN_CORNER::TOP_LEFT:
		rallyTarget = Point2D(building->pos.x + 10, building->pos.y + 10);
		break;
	case SPAWN_CORNER::TOP_RIGHT:
		rallyTarget = Point2D(building->pos.x - 10, building->pos.y + 10);
		break;
	case SPAWN_CORNER::BOTTOM_LEFT:
		rallyTarget = Point2D(building->pos.x + 10, building->pos.y - 10);
		break;
	case SPAWN_CORNER::BOTTOM_RIGHT:
		rallyTarget = Point2D(building->pos.x - 10, building->pos.y - 10);
		break;
	default:
		return false;
	}
	
	bot->Actions()->UnitCommand(building, ABILITY_ID::RALLY_BUILDING, rallyTarget);

	return true;
}

bool Movement::Swap(Bot * bot, Unit * addonSource, Unit * addonTarget) {

	if (!(addonSource->unit_type == UNIT_TYPEID::TERRAN_BARRACKS || addonSource->unit_type == UNIT_TYPEID::TERRAN_FACTORY || addonSource->unit_type == UNIT_TYPEID::TERRAN_STARPORT)) {
		return false;
	}
	if (!(addonTarget->unit_type == UNIT_TYPEID::TERRAN_BARRACKS || addonTarget->unit_type == UNIT_TYPEID::TERRAN_FACTORY || addonTarget->unit_type == UNIT_TYPEID::TERRAN_STARPORT)) {
		return false;
	}

	if (!addonSource->add_on_tag) {
		return false;
	}
	Point3D locationSource = addonSource->pos;
	Point3D locationTarget = addonTarget->pos;

	bot->Actions()->UnitCommand(addonSource, ABILITY_ID::LIFT);
	bot->Actions()->UnitCommand(addonTarget, ABILITY_ID::LIFT);

	bot->Actions()->UnitCommand(addonSource, ABILITY_ID::LAND, locationTarget);
	bot->Actions()->UnitCommand(addonTarget, ABILITY_ID::LAND, locationSource);

	return true;
	
}

int Movement::FindSpawnCorner(Bot * bot) {

	Point2D enemyBase = bot->Observation()->GetGameInfo().enemy_start_locations.front();
	Point2D botBase = bot->Observation()->GetStartLocation();

	float dx = enemyBase.x - botBase.x;
	float dy = enemyBase.y - botBase.y;

	if (dx > 100 && dy < 100) {
		return SPAWN_CORNER::BOTTOM_LEFT;
	}
	else if (dx > 100 && dy > 100) {
		return SPAWN_CORNER::TOP_LEFT;
	}
	else if (dx < 100 && dy < 100) {
		return SPAWN_CORNER::BOTTOM_RIGHT;
	}
	else if (dx < 100 && dy > 100) {
		return SPAWN_CORNER::TOP_RIGHT;
	}
	return -1;
}