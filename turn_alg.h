#pragma once
#include <vector>
#include <string>

typedef std::string TurnAlg;
typedef std::vector<TurnAlg> TurnAlgS;

bool is_turn_alg(const std::string& s);
TurnAlg remove_mults(const TurnAlg& s);
size_t turn_alg_len(const TurnAlg& s);

