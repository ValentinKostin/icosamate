#pragma once

#include <string>
#include <sstream>
#include <ostream>

#include "../icosamate.h"

int ic_scramble(const std::string& turnig_algorithm);
int ic_draw(const IcosamateInSpace& ic, const std::string& turnig_algorithm);

