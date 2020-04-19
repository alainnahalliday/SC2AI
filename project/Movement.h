#pragma once
#include <sc2api/sc2_api.h>
#include "Bot.h"

using namespace sc2;

class Bot;


enum SPAWN_CORNER {
	TOP_LEFT = 1,
	TOP_RIGHT = 2,
	BOTTOM_LEFT = 3,
	BOTTOM_RIGHT = 4
};

class Movement {
public:
	static bool RallyToRamp(Bot * bot,  const Unit * building);
	static bool Swap(Bot * bot, Unit * addonSource, Unit * addonTarget);
	//1  is top left, 2 is top right, 3 is bottom left, 4 is bottom right.
	static int FindSpawnCorner(Bot * bot);
private:
};