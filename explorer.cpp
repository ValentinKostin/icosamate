#include <sstream>
#include "explorer.h"

std::string to_str(const ActionS& acts)
{
	std::ostringstream oss;
	for (const Action& a : acts)
	{
		check(a >= 0 && a <= A_6_MOVE_CCW);

		if (a >= A_1_MOVE_CW)
			oss << 'M';
		AxisId ax_id = (a - 1) % 6 + 1;
		oss << ax_id;
		if (a >= A_1_MOVE_CCW || a >= A_1_TURN_CCW && a <= A_6_TURN_CCW)
			oss << '\'';
	}
	return oss.str();
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
			act_delta = 12;
			continue;
		}
		AxisId ax_id = get_axis(b);
		Action cw_delta = 0;
		if (i+1<s.size() && s[i+1]=='\'')
		{
			cw_delta = 6;
			++i;
		}
		Action a = act_delta + cw_delta + Action(ax_id);
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

IcosamateExplorer::IcosamateExplorer(std::ostream& log)	: log_(log)
{
}

void IcosamateExplorer::actions(const ActionS& a)
{
	log_ << "Actions " << to_str(a) << ": ";

	ic_.actions(a);

	auto d = ic_.difference(ic0_, ic_);
	auto sd = ic_.solving_difference(ic0_, ic_);

	log_ << "d=" << d << ", sd=" << sd;

	auto p = ic_.period(a);
	log_ << ", p=" << p;
	log_ << std::endl;
}
