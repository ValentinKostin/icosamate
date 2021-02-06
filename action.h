#pragma once

#include <vector>
#include <string>
#include "turn_alg.h"

typedef int Action;
static const Action A_NO_ACTION = 0;
static const Action A_1_TURN_CW = 1;
static const Action A_12_TURN_CW = 12;
static const Action A_1_TURN_CCW = 13;
static const Action A_12_TURN_CCW = 24;
static const Action A_1_MOVE_CW = 25;
static const Action A_12_MOVE_CW = 36;
static const Action A_1_MOVE_CCW = 37;
static const Action A_12_MOVE_CCW = 48;
typedef std::vector<Action>	ActionS;

TurnAlg to_str(const ActionS& acts);
ActionS from_str(const TurnAlg& s);


