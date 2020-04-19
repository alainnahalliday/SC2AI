#include "TryBuild.h"

bool TryBuild::TryBuildStructure(Bot *bot, ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {
	const ObservationInterface* observation = bot->Observation();

	//Make this more efficient by limiting the set of units returned by GetUnits to the unit_type?

	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self);

	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}

		if (unit->unit_type == unit_type) {
			unit_to_build = unit;
		}
	}

	//This moves it out of the mineral line for the most part, but it's really not a good solution.
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();
	if (!unit_to_build) {
		return false;
	}

	Point2D targetLocation = Point2D(unit_to_build->pos.x + rx * 16.0f, unit_to_build->pos.y + ry * 16.0f);
	if (bot->Query()->Placement(ability_type_for_structure, targetLocation)) {
		bot->Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
			Point2D(unit_to_build->pos.x + rx * 16.0f, unit_to_build->pos.y + ry * 16.0f));
	}

	return true;
}

void TryBuild::TryBuildAll(Bot *bot) {

	TryBuildSupplyDepot(bot);
	TryBuildBarracks(bot);
	TryBuildFactory(bot);
	TryBuildArmory(bot);
	TryBuildStarport(bot);
	TryBuildRefinery(bot);

}
bool TryBuild::TryBuildSupplyDepot(Bot *bot) {
	const ObservationInterface* observation = bot->Observation();

	/* Leaving this out temporarily while trying to find a proper trigger for 2nd refinery
	//If we have 1 supply depot, build a refinery.
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) == 1) {
		TryBuildRefinery(bot);
	}
	*/
	//As production ramps up (ie multiple barracks) we want to build supply depots more frequently
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 1 && observation->GetFoodUsed() >= observation->GetFoodCap() - 6) {
		return TryBuildStructure(bot, ABILITY_ID::BUILD_SUPPLYDEPOT);
	}
	else if (bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 2 && bot->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > 1 && observation->GetFoodUsed() >= observation->GetFoodCap() - 12) {
		return TryBuildStructure(bot, ABILITY_ID::BUILD_SUPPLYDEPOT);
	}
	else {
		if (bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 2 && observation->GetFoodUsed() >= observation->GetFoodCap() - 2) {
			return TryBuildStructure(bot, ABILITY_ID::BUILD_SUPPLYDEPOT);
		}
	}
	//try and build a supply depot

	return false;
}
bool TryBuild::TryBuildFactory(Bot *bot) {
	const ObservationInterface* observation = bot->Observation();

	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) {
		return false;
	}
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) >= MAX_FACTORY_COUNT) {
		return false;
	}

	return TryBuildStructure(bot, ABILITY_ID::BUILD_FACTORY);

}
bool TryBuild::TryBuildRefinery(Bot *bot) {

	const ObservationInterface* observation = bot->Observation();

	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}
	//delay 2nd refinery
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) > 0 && bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 2) {
		return false;
	}
	Units units = observation->GetUnits(Unit::Alliance::Self);
	const Unit* unit_to_build = nullptr;
	for (const auto& unit : units) {
		if (unit->unit_type == UNIT_TYPEID::TERRAN_SCV) {
			unit_to_build = unit;
		}
	}
	const Unit* vespene_target = bot->FindNearestVespeneGeyser(unit_to_build->pos);
	if (!vespene_target)
		return false;
	bot->Actions()->UnitCommand(unit_to_build, ABILITY_ID::BUILD_REFINERY, vespene_target);

	return true;

}

bool TryBuild::TryBuildBarracks(Bot *bot) {
	const ObservationInterface* observation = bot->Observation();

	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
		return false;
	}

	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > MAX_BARRACKS_COUNT) {
		return false;
	}

	return TryBuildStructure(bot, ABILITY_ID::BUILD_BARRACKS);
}

bool TryBuild::TryBuildStarport(Bot *bot) {
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1 || bot->CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) > 0) {
		return false;
	}

	return TryBuildStructure(bot, ABILITY_ID::BUILD_STARPORT);
}

bool TryBuild::TryBuildArmory(Bot *bot) {
	if (bot->CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) > 1 || bot->CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
		return false;
	}

	return TryBuildStructure(bot, ABILITY_ID::BUILD_ARMORY);
}