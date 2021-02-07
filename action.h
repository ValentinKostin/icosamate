#pragma once

#include <vector>
#include <string>
#include "turn_alg.h"

typedef int Action;
static const Action A_NO_ACTION = 0;
static const Action A_1_TURN_CW = 1;
static const Action A_2_TURN_CW = 2;
static const Action A_7_TURN_CW = 7;
static const Action A_12_TURN_CW = 12;
static const Action A_1_TURN_CCW = 13;
static const Action A_2_TURN_CCW = 14;
static const Action A_7_TURN_CCW = 19;
static const Action A_12_TURN_CCW = 24;
static const Action A_1_MOVE_CW = 25;
static const Action A_2_MOVE_CW = 26;
static const Action A_7_MOVE_CW = 31;
static const Action A_12_MOVE_CW = 36;
static const Action A_1_MOVE_CCW = 37;
static const Action A_2_MOVE_CCW = 38;
static const Action A_7_MOVE_CCW = 43;
static const Action A_12_MOVE_CCW = 48;
typedef std::vector<Action>	ActionS;



