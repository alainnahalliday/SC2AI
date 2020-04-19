#include "Bot.h"

using namespace sc2;

void Bot::OnGameStart() {
	std::cout << "Launching SC2 Game. Playing as Terran versus Random. GLHF" << std::endl;
	delayCounter = 0;
	spawnCorner = Movement::FindSpawnCorner(this);
}
//On every step of the coordinator
//Using an offset to prevent checking more frequently than needed
void Bot::OnStep() {

	delayCounter++;
	if (delayCounter == GAME_CHECK_DELAY_FACTOR) {
		if (CheckAttackCondition()) {
			TryMicro();
		}
		TryBuild::TryBuildAll(this);
		delayCounter = 0;
	}
}

size_t Bot::CountUnitType(UNIT_TYPEID unit_type) {
	return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

bool Bot::CheckAttackCondition() {
	return Observation()->GetFoodArmy() > ARMY_SUPPLY_ATTACK_TRIGGER;
}

//All-in strategy
void Bot::Attack() {
	TryMicro();
}

void Bot::TryMicro() {
	Units units = Observation()->GetUnits();
	for (const auto & u : units) {
		if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
			Actions()->UnitCommand(u, ABILITY_ID::ATTACK_ATTACK,
				Observation()->GetGameInfo().enemy_start_locations.front());
			if (u->engaged_target_tag && u->buffs.empty()) {
				Actions()->UnitCommand(u, ABILITY_ID::EFFECT_STIM_MARINE);
			}
		}
		else if (u->unit_type == UNIT_TYPEID::TERRAN_SIEGETANK) {
			Actions()->UnitCommand(u, ABILITY_ID::ATTACK_ATTACK,
				Observation()->GetGameInfo().enemy_start_locations.front());
			if (u->engaged_target_tag) {
				Actions()->UnitCommand(u, ABILITY_ID::MORPH_SIEGEMODE);
			}
			else {
				Actions()->UnitCommand(u, ABILITY_ID::MORPH_UNSIEGE);
			}
		}
		else if (u->unit_type == UNIT_TYPEID::TERRAN_MEDIVAC) {
			Actions()->UnitCommand(u, ABILITY_ID::ATTACK, Observation()->GetGameInfo().enemy_start_locations.front());
		}
		else if (u->unit_type == UNIT_TYPEID::TERRAN_MARAUDER) {
			Actions()->UnitCommand(u, ABILITY_ID::ATTACK, Observation()->GetGameInfo().enemy_start_locations.front());
		}

	}

}


void Bot::OnUnitIdle(const Unit* unit) {
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
			break;
		}
		if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < MAX_SCV_COUNT) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: {
		if (unit->energy > 25) {
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE, FindNearestMineralPath(unit->pos));
			break;
		}
		if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < MAX_SCV_COUNT) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		Units units = Observation()->GetUnits(Unit::Alliance::Self);
		const Unit* target = nullptr;
		
		for (const auto& u : units) {
			if (u->unit_type == UNIT_TYPEID::TERRAN_REFINERY) {
				if (u->assigned_harvesters < 3) {
					target = u;
					break;
				}
			}
		}
		//If there is no suitable vespene geyser, go to a mineral patch and start mining
		//Note this is might not be advisable if trying a proxy build
		if (!target) {
			target = FindNearestMineralPath(unit->pos);
		}
		//If no minerals are nearby, do nothing
		if (!target) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::HARVEST_GATHER, target);
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
	
		Movement::RallyToRamp(this, unit);
		if (!unit->add_on_tag && CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 6 && CountUnitType(UNIT_TYPEID::TERRAN_TECHLAB) < 2) {
			if (Query()->Placement(ABILITY_ID::BUILD_TECHLAB, Point2D(unit->pos.x + 2.5, unit->pos.y - 1), unit)){
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
				}
			break;
		}
		else if (!unit->add_on_tag && CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 6 && CountUnitType(UNIT_TYPEID::TERRAN_TECHLAB) >= 2) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_REACTOR_BARRACKS);
			break;
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARAUDER);
			if (unit->orders.empty()) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
			}
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORY: {
		Actions()->UnitCommand(unit, ABILITY_ID::RALLY_BUILDING, (Observation()->GetStartLocation()));

		bool attempted = false;
		if (!unit->add_on_tag && !attempted) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_FACTORY);
			attempted = true;
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SIEGETANK);
		break;
	}
	case UNIT_TYPEID::TERRAN_STARPORT: {
		Actions()->UnitCommand(unit, ABILITY_ID::RALLY_BUILDING, (Observation()->GetStartLocation()));

		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MEDIVAC);
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		break;
	}
	case UNIT_TYPEID::TERRAN_SIEGETANK: {
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB: {
		Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_STIMPACK);
		if (unit->orders.empty()) {
			Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_COMBATSHIELD);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_MEDIVAC: {
		Actions()->UnitCommand(unit, ABILITY_ID::SMART);
	}
	default: {
		break;
	}
	}
}

const Unit* Bot::FindNearestMineralPath(const Point2D& start) {
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}

	return target;
}

const Unit* Bot::FindNearestVespeneGeyser(const Point2D& start) {
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}
	return target;
}