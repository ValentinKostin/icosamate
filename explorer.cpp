#include <sstream>
#include "explorer.h"
#include "draw/draw.h"

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

ActionResult IcosamateExplorer::calc_result(const ActionS& a, bool from_0) const
{
	if (from_0)
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

template<class M, class R> void add_action_res(M& m, const R& r, const ActionS& a)
{
	auto q = m.find(r);
	if (q == m.end())
		m.insert({ r, a });
	else if (q->second.size() > a.size())
		m[r] = a;
}

void IcosamateExplorer::process_actions(const ActionS& aa, size_t mul, size_t total_mul, bool update_maps)
{
	log_ << mul_str(aa, total_mul) << ": ";

	auto acts = mul_actions(aa, mul);

	bool from_0 = mul == total_mul;
	auto r = calc_result(acts, from_0);

	if (update_maps)
	{
		auto a = mul_actions(aa, total_mul);
		add_action_res(actmap_, r.diff_, a);
		add_action_res(solving_actmap_, r.solved_diff_, a);
		add_action_res(per_map_, r.period_, a);
		add_action_res(solving_per_map_, r.solved_period_, a);
	}

	log_ << r << std::endl;
}

void IcosamateExplorer::actions(const ActionS& aa, size_t mul)
{
	if (mul > 0)
	{
		process_actions(aa, mul, mul);
		ic_draw(ic_, mul_str(aa, mul));
	}

	process_actions(aa, 1, 1);
	if (!with_mults_)
		return;

	size_t period = IcosamateInSpace::period(aa);
	std::vector<size_t> nn = multiplyers(period);
	size_t prev_n = 1;
	for (auto n : nn)
	{
		process_actions(aa, n-prev_n, n);
		prev_n = n;
	}
}

void IcosamateExplorer::process_elem(const ActionS& a)
{
	process_actions(a, 1, 1, true);

	if (!with_mults_)
		return;

	size_t period = IcosamateInSpace::period(a);
	std::vector<size_t> nn = multiplyers(period);
	size_t prev_n = 1;
	for (auto n : nn)
	{
		process_actions(a, n - prev_n, n, true);
		prev_n = n;
	}
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
	Action doubling_last_action = (n > 1 && an[n - 1] == an[n - 2]) ? an[n - 1] : A_NO_ACTION;  // ��� ���������� ������������ ���� ��������, � ���� ������ �������, �� 111 ������������ ��������� 11
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
