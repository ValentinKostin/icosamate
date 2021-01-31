#pragma once
#include <vector>

typedef size_t AxisId;

struct Axis
{
	AxisId id_;
	AxisId opposite_id_;
	static const size_t NEAR_AXIS_COUNT = 5;
	std::vector<AxisId> near_axes_; // обход по часовой стрелке
	bool invariant() const
	{
		return near_axes_.size() == NEAR_AXIS_COUNT;
	}
};


// фиксированные оси 0-11
class Axes
{
	static const std::vector<Axis> make_axes();
	const std::vector<Axis> axes_; // первые 6 - основные оси, остальные - им противоположные
public:
	Axes();
	const Axis& axis(AxisId ax_id) const { return axes_.at(ax_id); }
	const Axis& operator[](AxisId ax_id) const { return axes_.at(ax_id); }
	const std::vector<Axis>& axes() const { return axes_; }
	size_t count() const { return 12; }

	std::vector<AxisId> near_common_axis(AxisId id_1, AxisId id_2) const;

	// буквенные обозначения
	char get_char(AxisId id) const;
	AxisId get_axis(char a) const;

	bool is_axis_char(char a) const;
};

const Axes& axes();



