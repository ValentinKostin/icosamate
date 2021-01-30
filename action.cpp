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

ActionS from_str(const std::string& s)
{
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
