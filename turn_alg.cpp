#include "def.h"
#include "turn_alg.h"
#include "action.h"
#include "icosamate.h"

bool is_digit(char a) { return a == '0' || a <= '9' && a >= '1'; }

bool is_turn_alg(const std::string& s)
{
   try
   {
	   ActionS acts = IcosamateInSpace::from_str(s);
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
	ActionS acts = IcosamateInSpace::from_str(s);
	return acts.size();
}

TurnAlg simple_inverse(const TurnAlg& s)
{
	typedef IcosamateInSpace I;
	ActionS acts = I::from_str(s);
	ActionS inv_acts;
	for (Action a : acts)
		inv_acts.push_back(I::inverse(a));
	std::reverse(inv_acts.begin(), inv_acts.end());
	return I::to_str(inv_acts);
}

TurnAlg mult_inverse(const TurnAlg& s)
{
	check(s[0] == '(');
	auto i = s.find_first_of('x');
	check(i != std::string::npos);
	check(i > 1 && s[i - 1] == ')');
	for (size_t k = i + 1; k < s.size(); ++k)
		check(is_digit(s[k]));

	TurnAlg r; 
	r += s[0] + inverse(s.substr(1, i - 2)) + s.substr(i - 1);
	return r;
}

TurnAlg inverse(const TurnAlg& s)
{
	struct MultAlg
	{
		size_t beg_;
		size_t size_;
	};

	std::vector<MultAlg> mult_algs;
	for (size_t k = 0; k < s.size();)
	{
		auto i = s.find_first_of('x', k);
		if (i == std::string::npos)
			break;

		check(i > 1 && s[i - 1] == ')');
		auto j = s.find_last_of('(', i - 1);
		check(j != std::string::npos);

		size_t bm = i + 1;
		check(bm < s.size());
		check(is_digit(s[bm]));
		while (bm < s.size() && is_digit(s[bm]))
			++bm;
		mult_algs.push_back({ j, i - j + 1 });
	}
	size_t ma_n = mult_algs.size();

	TurnAlg r;
	for (size_t i=0; i< mult_algs.size(); ++i)
	{
		size_t b = i == 0 ? 0 : mult_algs[i - 1].beg_ + mult_algs[i - 1].size_;
		TurnAlg sa = s.substr(b, mult_algs[i].beg_-b);
		if (!sa.empty())
		{
			sa = simple_inverse(sa);
			std::reverse(sa.begin(), sa.end());
			r.insert(r.end(), sa.begin(), sa.end());
		}
		TurnAlg ma = s.substr(mult_algs[i].beg_, mult_algs[i].size_);
		std::reverse(ma.begin(), ma.end());
		r.insert(r.end(), ma.begin(), ma.end());
	}
	size_t b = mult_algs.empty() ? 0 : mult_algs[ma_n - 1].beg_ + mult_algs[ma_n - 1].size_;
	if (b < s.size())
	{
		TurnAlg sa = s.substr(b);
		sa = simple_inverse(sa);
		std::reverse(sa.begin(), sa.end());
		r.insert(r.end(), sa.begin(), sa.end());
	}

	std::reverse(r.begin(), r.end());
	return r;
}

bool is_repetition(const TurnAlg& s, const TurnAlg& subs)
{
	size_t n = s.size();
	size_t k = subs.size();
	if (n % k != 0)
		return false;
	size_t m = n / k;
	for (size_t i = 0; i < m; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			if (s[i * k + j] != subs[j])
				return false;
		}
	}
	return true;
}

TurnAlg set_mults(const TurnAlg& s)
{
	size_t n = s.size();
	for (size_t k = 2; k <= n/2 && k<=24; k++) // слишком длинные не "заворачиваем"
	{
		if (n % k != 0)
			continue;
		if (is_repetition(s, s.substr(0, k)))
		{
			std::string r = "(";
			r += s.substr(0, k) + ")x" + std::to_string(n / k);
			return r;
		}
	}
	return s;
}


