#pragma once
#include <sc2api/sc2_api.h>
#include "Bot.h"

using namespace sc2;

class Bot;

class TryBuild {
public:
	static bool TryBuildStructure(Bot *bot, ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
	static void TryBuildAll(Bot *bot);
	static bool TryBuildSupplyDepot(Bot *bot);
	static bool TryBuildFactory(Bot *bot);
	static bool TryBuildRefinery(Bot *bot);
	static bool TryBuildBarracks(Bot *bot);
	static bool TryBuildStarport(Bot *bot);
	static bool TryBuildArmory(Bot *bot);

};