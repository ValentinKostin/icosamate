#pragma once
#include <vector>
#include <string>

typedef std::string TurnAlg;
typedef std::vector<TurnAlg> TurnAlgS;

bool is_turn_alg(const std::string& s);
size_t turn_alg_len(const TurnAlg& s);
TurnAlg remove_mults(const TurnAlg& s);
TurnAlg set_mults(const TurnAlg& s);   // "заворачивает" пока только если s - это повторение несколько раз


