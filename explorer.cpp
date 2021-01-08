#include <sstream>
#include "explorer.h"

char axis_char(AxisId id)
{
	switch (id)
	{
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'A';
	case 11: return 'B';
	case 0: return 'C';
	}
	raise("Bad axis id");
	return 0;
}

std::string to_str(const ActionS& acts)
{
	std::string r;
	for (const Action& a : acts)
	{
		check(a >= 0 && a <= A_12_MOVE_CCW);

		if (a >= A_1_MOVE_CW)
			r.push_back('M');
		AxisId ax_id = a % 12;
		r.push_back(axis_char(ax_id));
		if (a >= A_1_MOVE_CCW || a >= A_1_TURN_CCW && a <= A_12_TURN_CCW)
			r.push_back('\'');
	}
	return r;
}

AxisId get_axis(char a)
{
	switch (a)
	{
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': return 10;
	case 'B': return 11;
	case 'C': return 0;
	}
	raise("Bad axis symbol");
	return 0;
}

ActionS from_str(const std::string& s)
{
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
		AxisId ax_id = get_axis(b);
		Action ax_delta = ax_id > 0 ? Action(ax_id) : 12;
		Action cw_delta = 0;
		if (i+1<s.size() && s[i+1]=='\'')
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

std::ostream& operator<<(std::ostream& oss, const IcosamateDifference& d)
{
	oss << "(" << d.vert_elems_count_ << "," << d.vert_elems_diff_orient_ << "," << d.centers_count_ << ")";
	return oss;
}

std::ostream& operator<<(std::ostream& oss, const ActionResult& r)
{
	oss << "d=" << r.diff_ << ", sd=" << r.solved_diff_;
	oss << ", p=" << r.period_ << ", sp=" << r.solved_period_;
	return oss;
}

IcosamateExplorer::IcosamateExplorer(std::ostream& log)	: log_(log)
{
}

std::string mul_str(const ActionS& a, size_t mul)
{
	std::string r;
	if (mul != 1)
		r += "(";
	r += to_str(a);
	if (mul != 1)
		r += ")x" + std::to_string(mul);
	return r;
}

ActionS mul_actions(const ActionS& a, size_t mul)
{
	check(mul > 0);
	if (mul == 1)
		return a;
	ActionS r;
	r.reserve(a.size() * mul);
	for (size_t i = 0; i < mul; i++)
		r.insert(r.end(), a.begin(), a.end());
	return r;
}

ActionResult IcosamateExplorer::calc_result() const
{
	return
	{
		ic_.difference(ic0_, ic_),
		ic_.solving_difference(ic0_, ic_),
		ic_.period(actions_),
		ic_.solving_period(actions_)
	};
}

void IcosamateExplorer::actions(const ActionS& aa, size_t mul)
{
	log_ << "Actions " << mul_str(aa, mul) << ": ";

	actions_ = mul_actions(aa, mul);
	ic_.actions(actions_);

	auto r = calc_result();

	log_ << r << std::endl;
}

void IcosamateExplorer::tree_step(const ActionS& a)
{

}

void IcosamateExplorer::tree(size_t n, bool add_commutators)
{

}


void explore_near_axis(std::ostream& log, bool turn1, bool turn2, bool is_1_cw, bool is_2_cw)
{
	for (AxisId ax_id = 0; ax_id < IcosamateInSpace::AXIS_COUNT; ax_id++)
	{
		Action a1 = turn1 ? IcosamateInSpace::turn_action(ax_id, is_1_cw) : IcosamateInSpace::move_action(ax_id, is_1_cw);
		const Axis& a = IcosamateInSpace::axis(ax_id);
		for (AxisId near_ax_id : a.near_axes_)
		{
			Action a2 = turn2 ? IcosamateInSpace::turn_action(near_ax_id, is_2_cw) : IcosamateInSpace::move_action(near_ax_id, is_2_cw);

			ActionS aa = { a1, a2 };
			IcosamateExplorer ex(log);
			ex.actions(aa);
		}
	}
}

void explore_near_axis(std::ostream& log, bool turn1, bool turn2)
{
	explore_near_axis(log, turn1, turn2, true, true);
	explore_near_axis(log, turn1, turn2, true, false);
	explore_near_axis(log, turn1, turn2, false, true);
	explore_near_axis(log, turn1, turn2, false, false);
}

void explore_near_axis(std::ostream& log)
{
	explore_near_axis(log, true, true);
	explore_near_axis(log, true, false);
	explore_near_axis(log, false, true);
	explore_near_axis(log, false, false);
}
