#include "def.h"
#include "action.h"
#include "axis.h"

std::string to_str(const ActionS& acts)
{
	const Axes& aa = axes();
	std::string r;
	for (const Action& a : acts)
	{
		check(a >= 0 && a <= A_12_MOVE_CCW);

		if (a >= A_1_MOVE_CW)
			r.push_back('M');
		AxisId ax_id = a % 12;
		r.push_back(aa.get_char(ax_id));
		if (a >= A_1_MOVE_CCW || a >= A_1_TURN_CCW && a <= A_12_TURN_CCW)
			r.push_back('\'');
	}
	return r;
}

bool is_digit(char a) { return a == '0' || a <= '9' && a >= '1'; }

std::string remove_mults(const std::string& s)
{
	auto i = s.find_first_of('x');
	if (i == std::string::npos)
		return s;

	check(i>1 && s[i - 1] == ')');
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

ActionS from_str(const std::string& ss)
{
	std::string s = remove_mults(ss);

	const Axes& aa = axes();
	ActionS r;
	Action act_delta = 0;
	for (size_t i = 0; i < s.size(); i++)
	{
		char b = s[i];
		if (b == 'M')
		{
			act_delta = 24;
			continue;
		}
		AxisId ax_id = aa.get_axis(b);
		Action ax_delta = ax_id > 0 ? Action(ax_id) : 12;
		Action cw_delta = 0;
		if (i + 1 < s.size() && s[i + 1] == '\'')
		{
			cw_delta = 12;
			++i;
		}
		Action a = act_delta + cw_delta + ax_delta;
		r.push_back(a);
		act_delta = 0;
	}
	return r;
}
