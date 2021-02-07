#include "def.h"
#include "turn_alg.h"
#include "action.h"
#include "axis.h"
#include "icosamate.h"

Action IcosamateInSpace::turn_action(AxisId axis_id, bool clockwise)
{
	return Action((clockwise ? 0 : 12) + (axis_id == 0 ? 12 : axis_id));
}

Action IcosamateInSpace::move_action(AxisId axis_id, bool clockwise)
{
	return turn_action(axis_id, clockwise) + 24;
}

Action IcosamateInSpace::inverse(Action a)
{
	if (a > A_12_MOVE_CCW || a < 0)
		return A_NO_ACTION;

	if (a >= A_1_MOVE_CW)
		return (a >= A_1_MOVE_CCW) ? a - 12 : a + 12;
	else
		return (a >= A_1_TURN_CCW) ? a - 12 : a + 12;
}

ActionS IcosamateInSpace::inverse(const ActionS& a)
{
	ActionS r;
	r.reserve(a.size());
	for (auto q = a.cbegin(); q != a.cend(); ++q)
		r.push_back(inverse(*q));
	return r;
}

TurnAlg IcosamateInSpace::to_str(const ActionS& acts)
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

ActionS IcosamateInSpace::from_str(const TurnAlg& ss)
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
