#pragma once

#include <sc2api/sc2_api.h>
#include <iostream>
#include "TryBuild.h"
#include "Movement.h"


#define MAX_SCV_COUNT 24
#define MAX_BARRACKS_COUNT 3
#define MAX_FACTORY_COUNT 2
#define MAX_CC_COUNT 2
#define ARMY_SUPPLY_ATTACK_TRIGGER 50
#define GAME_CHECK_DELAY_FACTOR 20

using namespace sc2;

class Bot : public Agent {
public:

	int spawnCorner;

	virtual void OnGameStart();
	virtual void OnStep() final;
	size_t CountUnitType(UNIT_TYPEID unit_type);
	bool CheckAttackCondition();
	void Attack();
	void TryMicro();
	virtual void OnUnitIdle(const Unit* unit);
	const Unit* FindNearestMineralPath(const Point2D& start);
	const Unit* FindNearestVespeneGeyser(const Point2D& start);

private:
	int delayCounter;
};
