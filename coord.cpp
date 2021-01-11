#define _USE_MATH_DEFINES

#include "coord.h"
#include <cmath>
#include <set>

IcosomateCoords::IcosomateCoords(double radius) : radius_(radius)
{
	fill_coords();
	fill_face_triangles();
}

void IcosomateCoords::fill_coords()
{
	double m_angle = std::atan(0.5);
	double rsin = radius_ * std::sin(m_angle); // R / sqrt(5);
	double rcos = radius_ * std::cos(m_angle); // R * 2 / sqrt(5);

	double s_angle = M_PI * 2 / 5;

	coords_.resize(12);

	coords_[1].ax_id_ = 1;
	coords_[1].x_ = 0;
	coords_[1].y_ = 0;
	coords_[1].z_ = radius_;

	for (AxisId ax_id = 2; ax_id <= 6; ax_id++)
	{
		coords_[ax_id].ax_id_ = ax_id;
		coords_[ax_id].x_ = rcos * sin(s_angle * (ax_id - 1.5));
		coords_[ax_id].y_ = rcos * cos(s_angle * (ax_id - 1.5));
		coords_[ax_id].z_ = rsin;
	}

	for (AxisId ax_id = 7; ax_id <= 10; ax_id++)
	{
		coords_[ax_id].ax_id_ = ax_id;
		coords_[ax_id].x_ = rcos * sin(s_angle * (ax_id-1.0));
		coords_[ax_id].y_ = rcos * cos(s_angle * (ax_id-1.0));
		coords_[ax_id].z_ = -rsin;
	}

	coords_[1].ax_id_ = 0;
	coords_[0].x_ = 0;
	coords_[0].y_ = 0;
	coords_[0].z_ = -radius_;
}

void VertTriangle::rotate_start_min()
{
	size_t min_idx = 3;
	AxisId min_id = 12;
	for (size_t i = 0; i < 3; i++)
	{
		if (coords_[i]->ax_id_ < min_id)
		{
			min_id = coords_[i]->ax_id_;
			min_idx = i;
		}
	}
	VertTriangle t = *this;
	coords_[0] = t.coords_[min_idx % 3];
	coords_[1] = t.coords_[(min_idx + 1) % 3];
	coords_[2] = t.coords_[(min_idx + 2) % 3];
}

void IcosomateCoords::fill_face_triangles()
{
	std::set<VertTriangle> filled_triangles;
	for (AxisId ax_id = 0; ax_id < IcosamateInSpace::AXIS_COUNT; ax_id++)
	{
		const Axis& a = IcosamateInSpace::axis(ax_id);
		for (size_t i=0; i<5; ++i)
		{
			VertTriangle t;
			t.coords_[0] = &coords_[ax_id];
			t.coords_[1] = &coords_[a.near_axes_[i]];
			t.coords_[2] = &coords_[a.near_axes_[(i+1)%5]];
			t.rotate_start_min();

			if (filled_triangles.count(t))
				continue;
			
			FaceTriangleId id = face_triangles_.size();
			face_triangles_.push_back({id, t});
			filled_triangles.insert(t);
		}
	}
}


