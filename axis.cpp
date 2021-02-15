#include "def.h"
#include "axis.h"

const std::vector<Axis> Axes::make_axes()
{
	return
	{
		{0, 1, { 7, 11, 10, 9, 8 }}, { 1, 0, {2, 3, 4, 5, 6} }, { 2, 9, {1, 6, 11, 7, 3} }, { 3, 10, {1, 2, 7, 8, 4} },
		{ 4, 11, {1, 3, 8, 9, 5} }, { 5, 7, {1, 4, 9, 10, 6} }, { 6, 8, {1, 5, 10, 11, 2} }, { 7, 5, {2, 11, 0, 8, 3} },
		{ 8, 6, {3, 7, 0, 9, 4} }, { 9, 2, {4, 8, 0, 10, 5} }, { 10, 3, {5, 9, 0, 11, 6} }, { 11, 4, {2, 6, 10, 0, 7} }
	};
}

Axes::Axes() : axes_(make_axes())
{
	check(axes_.size() == count());

	// проверка, что оси заполнены корректно:
	std::vector<size_t> near_axis_count(axes_.size());
	for (const Axis& a : axes_)
	{
		check(a.invariant());
		for (AxisId id : a.near_axes_)
			++near_axis_count[id];
	}
	for (size_t ac : near_axis_count)
		check(ac == Axis::NEAR_AXIS_COUNT);
}

std::vector<AxisId> Axes::near_common_axis(AxisId id_1, AxisId id_2) const
{
	std::vector<AxisId> r;
	for (AxisId t1 : axes_[id_1].near_axes_)
	{
		for (AxisId t2 : axes_[id_2].near_axes_)
		{
			if (t1 == t2)
				r.push_back(t1);
		}
	}
	return r;
}

bool Axes::is_near(AxisId id_1, AxisId id_2) const
{
	for (AxisId t1 : axes_[id_1].near_axes_)
	{
		if (t1 == id_2)
			return true;
	}
	return false;
}

////////////////////////////////////////////
// old numeric notation
char get_x16_natural_char_by_axis(AxisId id)
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

AxisId get_axis_by_x16_natural_char(char a)
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
////////////////////////////////////////////
char get_gelatinbrain_char_by_axis(AxisId id)
{
	switch (id)
	{
	case 1: return 'B';
	case 2: return 'A';
	case 3: return 'F';
	case 4: return 'I';
	case 5: return 'H';
	case 6: return 'C';
	case 7: return 'E';
	case 8: return 'J';
	case 9: return 'L';
	case 10: return 'G';
	case 11: return 'D';
	case 0: return 'K';
	}
	raise("Bad axis id");
	return 0;
}

AxisId get_axis_by_gelatinbrain_char(char a)
{
	switch (a)
	{
	case 'B': return 1;
	case 'A': return 2;
	case 'F': return 3;
	case 'I': return 4;
	case 'H': return 5;
	case 'C': return 6;
	case 'E': return 7;
	case 'J': return 8;
	case 'L': return 9;
	case 'G': return 10;
	case 'D': return 11;
	case 'K': return 0;
	}
	return 12;
}
////////////////////////////////////////////

char Axes::get_char(AxisId id) const
{
	return get_gelatinbrain_char_by_axis(id);
}

AxisId Axes::get_axis(char a) const
{
	AxisId ax_id = get_axis_by_gelatinbrain_char(a);
	if (ax_id==12)
		raise("Bad axis symbol");

	return ax_id;
}

bool Axes::is_axis_char(char a) const
{
	AxisId ax_id = get_axis_by_gelatinbrain_char(a);
	return  ax_id != 12;
}

const Axes& axes()
{
	static Axes	axes;
	return axes;
}