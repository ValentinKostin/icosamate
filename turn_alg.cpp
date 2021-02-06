#include "def.h"
#include "turn_alg.h"
#include "action.h"

bool is_digit(char a) { return a == '0' || a <= '9' && a >= '1'; }

bool is_turn_alg(const std::string& s)
{
   try
   {
	   ActionS acts = from_str(s);
	   return true;
   }
   catch (...){}
   return false;
}

TurnAlg remove_mults(const TurnAlg& s)
{
	auto i = s.find_first_of('x');
	if (i == std::string::npos)
		return s;

	check(i > 1 && s[i - 1] == ')');
	auto j = s.find_last_of('(', i - 1);
	check(j != std::string::npos);
	std::string sub = s.substr(j + 1, i - j - 2);
	std::string beg = s.substr(0, j);

	size_t bm = i + 1;
	check(bm < s.size());
	check(is_digit(s[bm]));
	while (bm < s.size() && is_digit(s[bm]))
		++bm;
	std::string dig_str = s.substr(i + 1, bm - i - 1);
	size_t n = std::stoul(dig_str);

	std::string r = beg;
	for (size_t i = 0; i < n; i++)
		r += sub;
	if (bm + 1 < s.size())
		r += s.substr(bm);

	return remove_mults(r);
}

size_t turn_alg_len(const TurnAlg& s)
{
	ActionS acts = from_str(s);
	return acts.size();
}
