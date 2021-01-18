#include <sstream>
#include "explorer.h"
#include "draw/draw.h"

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
	bool has_period = r.period_ > 0;
	bool has_solving = r.solved_period_ > 0;
	oss << "d=" << r.diff_;
	if (has_solving)
		oss << ", sd=" << r.solved_diff_;
	if (has_period)
		oss << ", p=" << with_facorization(r.period_);
	if (has_solving && has_period)
		oss << ", sp=" << with_facorization(r.solved_period_);
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

ActionResult IcosamateExplorer::calc_result(const ActionS& a) const
{
	ic_ = ic0_;
	ic_.actions(a);
	return
	{
		ic_.difference(ic0_, ic_),
		with_solving_ ? ic_.solving_difference(ic0_, ic_) : IcosamateDifference(),
		with_period_ ? ic_.period(a) : 0,
		with_solving_ && with_period_ ? ic_.solving_period(a) : 0
	};
}

void IcosamateExplorer::process_actions(const ActionS& aa, size_t mul)
{
	log_ << mul_str(aa, mul) << ": ";

	auto acts = mul_actions(aa, mul);

	auto r = calc_result(acts);

	log_ << r << std::endl;
}

void IcosamateExplorer::actions(const ActionS& aa, size_t mul)
{
	if (mul > 0)
	{
		process_actions(aa, mul);
		ic_draw(ic_, mul_str(aa, mul));
	}

	size_t period = IcosamateInSpace::period(aa);
	std::vector<size_t> nn = multiplyers(period);
	process_actions(aa, 1);
	for (auto n : nn)
		process_actions(aa, n);
}

template<class M, class R> void add_action_res(M& m, const R& r, const ActionS& a)
{
	auto q = m.find(r);
	if (q == m.end())
		m.insert({ r, a });
	else if (q->second.size() > a.size())
		m[r] = a;
}

void IcosamateExplorer::process_elem(const ActionS& a)
{
	log_ << to_str(a) << ": ";

	auto r = calc_result(a);

	add_action_res(actmap_, r.diff_, a);
	add_action_res(solving_actmap_, r.solved_diff_, a);
	add_action_res(per_map_, r.period_, a);
	add_action_res(solving_per_map_, r.solved_period_, a);

	log_ << r << std::endl;
}

void IcosamateExplorer::tree_step(const ActionS& a, bool add_commutators)
{
	process_elem(a);

	if (!add_commutators) return;

	for (size_t i = 1; i < a.size(); ++i)
	{
		ActionS ca = IcosamateInSpace::commutator(a, int(i));
		if (IcosamateInSpace::canonic(ca))
			process_elem(ca);
	}
}

void IcosamateExplorer::tree_level(const ActionS& aa, size_t max_l, bool add_commutators)
{
	tree_step(aa, add_commutators);

	if (aa.size() >= max_l)
		return;

	ActionS an = aa;
	Action inverse_last_action = IcosamateInSpace::inverse(an.back());
	size_t n = an.size();
	Action doubling_last_action = (n > 1 && an[n - 1] == an[n - 2]) ? an[n - 1] : A_NO_ACTION;  // три одинаковых эквивалентны двум обратным, а если первые единицы, то 111 эквивалентно отражённым 11
	an.push_back(A_NO_ACTION);
	for (Action a = A_1_TURN_CW; a<= A_12_TURN_CCW; ++a)
	{
		if (a == inverse_last_action)
			continue;
		if (a == doubling_last_action)
			continue;

		an.back() = a;
		tree_level(an, max_l, add_commutators);
	}
}

void IcosamateExplorer::tree(size_t n, bool add_commutators)
{
	ActionS acts;
	acts.push_back(A_1_TURN_CW);
	tree_level(acts, n, add_commutators);

	print(actmap_, "Difference");
	print(solving_actmap_, "Solving difference");
	print(per_map_, "Period");
	print(solving_per_map_, "Solving period");
}


void explore_near_axis(std::ostream& log, bool turn1, bool turn2, bool is_1_cw, bool is_2_cw)
{
	for (AxisId ax_id = 0; ax_id < axes().count(); ax_id++)
	{
		Action a1 = turn1 ? IcosamateInSpace::turn_action(ax_id, is_1_cw) : IcosamateInSpace::move_action(ax_id, is_1_cw);
		const Axis& a = axes().axis(ax_id);
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
