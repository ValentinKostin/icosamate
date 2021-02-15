#pragma once
#include <vector>
#include <string>

typedef std::string TurnAlg;
typedef std::vector<TurnAlg> TurnAlgS;

bool is_turn_alg(const std::string& s);
size_t turn_alg_len(const TurnAlg& s);
TurnAlg inverse(const TurnAlg& s);
TurnAlg remove_mults(const TurnAlg& s);
TurnAlg set_mults(const TurnAlg& s);   // "заворачивает" пока только если s - это повторение несколько раз
TurnAlg reflect(const TurnAlg& s, char ax_1, char ax_2); // ax_1, ax_2 - две соседние оси, отражение происходит относительно задаваемой ими плоскости


