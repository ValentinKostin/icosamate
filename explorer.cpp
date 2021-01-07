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
		AxisId ax_id = (a - 1) % 12 + 1;
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
	auto sp = ic_.solving_period(a);
	log_ << ", p=" << p << ", sp=" << sp;
	log_ << std::endl;
}
