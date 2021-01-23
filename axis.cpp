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

char Axes::get_char(AxisId id) const
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

AxisId Axes::get_axis(char a) const
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

const Axes& axes()
{
	static Axes	axes;
	return axes;
}