#include <map>

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

AxisId IcosamateInSpace::action_axis(Action a)
{
	return a % 12;
}

Action IcosamateInSpace::change_axis(Action a, AxisId axis_id)
{
	if (a > A_12_MOVE_CCW || a <= 0)
		return A_NO_ACTION;

	int d = a % 12 == 0 ? 12 : a % 12;
	int r = a - d;
	r += int(axis_id == 0 ? 12 : axis_id);
	return r;
}

Action IcosamateInSpace::inverse(Action a)
{
	if (a > A_12_MOVE_CCW || a <= 0)
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

void add_reflections_1_to_2(std::map<AxisId, AxisId>& rmap, AxisId id_1, AxisId id_2)
{
	int k = 0;
	const Axes& aa = axes();
	const Axis& a1 = aa.axis(id_1);
	for (;k<5; ++k)
	{
		if (a1.near_axes_[k] == id_2)
			break;
	}
	check(k < 5);

	for (int i=0; i < 5; ++i)
	{
		AxisId ax_id = a1.near_axes_.at(i);
		AxisId ref_ax_id = a1.near_axes_.at((2*k+5-i)%5);
		rmap.insert({ ax_id, ref_ax_id });
	}
}

void add_reflections(std::map<AxisId, AxisId>& rmap, AxisId id_1, AxisId id_2)
{
	add_reflections_1_to_2(rmap, id_1, id_2);
	add_reflections_1_to_2(rmap, id_2, id_1);
}

// axis_id_1, axis_id_2 - две соседние оси, отражение происходит относительно задаваемой ими плоскости
ActionS IcosamateInSpace::reflect(const ActionS& acts, AxisId axis_id_1, AxisId axis_id_2)
{
	const Axes& aa = axes();
	check(aa.is_near(axis_id_1, axis_id_2));
	std::map<AxisId, AxisId> rmap;
	add_reflections(rmap, axis_id_1, axis_id_2);
	add_reflections(rmap, aa.axis(axis_id_1).opposite_id_, aa.axis(axis_id_2).opposite_id_);

	ActionS r;
	r.reserve(acts.size());
	for (const Action& a : acts)
	{
		AxisId ax_id = action_axis(a);
		AxisId refl_ax_id = rmap.at(ax_id);
		Action ra = change_axis(a, refl_ax_id);
		r.push_back(inverse(ra));
	}
	return r;
}

// поворот "всего алгоритма" вокруг axis_id
ActionS IcosamateInSpace::rotate(const ActionS& acts, AxisId axis_id, bool clockwise)
{
	std::map<AxisId, AxisId> rmap;
	const Axes& aa = axes();
	const Axis& a = aa.axis(axis_id);
	rmap.insert({ axis_id, axis_id });
	for (size_t i = 0; i < 5; i++)
		rmap.insert({ a.near_axes_[i], a.near_axes_[(i + (clockwise ? 1 : 4)) % 5] });
	const Axis& a_op = aa.axis(a.opposite_id_);
	rmap.insert({ a.opposite_id_, a.opposite_id_ });
	for (size_t i = 0; i < 5; i++)
		rmap.insert({ a_op.near_axes_[i], a_op.near_axes_[(i + (clockwise ? 4 : 1)) % 5] });

	ActionS r;
	r.reserve(acts.size());
	for (const Action& a : acts)
	{
		AxisId ax_id = action_axis(a);
		AxisId refl_ax_id = rmap.at(ax_id);
		Action ra = change_axis(a, refl_ax_id);
		r.push_back(ra);
	}
	return r;
}

ActionS IcosamateInSpace::center_symmetry(const ActionS& acts)
{
	std::map<AxisId, AxisId> rmap;
	const Axes& aa = axes();
	for (AxisId axis_id = 0; axis_id < aa.count(); axis_id++)
		rmap.insert({ axis_id, aa.axis(axis_id).opposite_id_ });

	ActionS r;
	r.reserve(acts.size());
	for (const Action& a : acts)
	{
		AxisId ax_id = action_axis(a);
		AxisId refl_ax_id = rmap.at(ax_id);
		Action ra = change_axis(a, refl_ax_id);
		r.push_back(inverse(ra));
	}
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
