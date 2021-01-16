#define _USE_MATH_DEFINES

#include "coord.h"
#include <cmath>
#include <set>

FaceTriangle::FaceTriangle(FaceTriangleId id, const VertTriangle& t) : VertTriangle(t), id_(id)
{
	fill_subtriangle_coords();
	fill_sticker_coords();
}

const Coord& FaceTriangle::subtriangle_coord(size_t subt_index, size_t pt_index) const
{
	check(subt_index <= 3);
	return subt_index==3 ? center_subtriangle_coord(pt_index) : vertex_subtriangle_coord(subt_index, pt_index);
}

const Coord& FaceTriangle::sticker_coord(size_t subt_index, size_t pt_index) const
{
	check(subt_index <= 3);
	check(pt_index <= 2);
	return sticker_coords_[subt_index*3 + pt_index];
}

void FaceTriangle::fill_subtriangle_coords()
{
	Coord p01 = (*coords_[0] + *coords_[1]) * 0.5;
	Coord p12 = (*coords_[1] + *coords_[2]) * 0.5;
	Coord p20 = (*coords_[2] + *coords_[0]) * 0.5;
	subtriangle_coords_ = { *coords_[0], p01, p20, *coords_[1], p12, p01, *coords_[2], p20, p12, p12, p20, p01 };
}

void FaceTriangle::fill_sticker_coords()
{
	sticker_coords_.reserve(subtriangle_coords_.size());
	for (size_t subt_index = 0; subt_index < 4; ++subt_index)
	{
		const Coord* c = &subtriangle_coords_[3 * subt_index];
		Coord center = (c[0] + c[1] + c[2]) / 3.0;

		const double STICKER_SIZE_PART = 0.9;
		for (size_t pt_index = 0; pt_index < 3; ++pt_index)
			sticker_coords_.push_back(center * (1.0 - STICKER_SIZE_PART) + c[pt_index] * STICKER_SIZE_PART);
	}
}

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

	for (AxisId ax_id = 7; ax_id <= 11; ax_id++)
	{
		coords_[ax_id].ax_id_ = ax_id;
		coords_[ax_id].x_ = rcos * sin(s_angle * (ax_id - 1.0));
		coords_[ax_id].y_ = rcos * cos(s_angle * (ax_id - 1.0));
		coords_[ax_id].z_ = -rsin;
	}

	coords_[0].ax_id_ = 0;
	coords_[0].x_ = 0;
	coords_[0].y_ = 0;
	coords_[0].z_ = -radius_;

	const double eps = 1e-8;
	for (auto& c: coords_)
	{
		if (fabs(c.x_) < eps)
			c.x_ = 0;
		if (fabs(c.y_) < eps)
			c.y_ = 0;
	}
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
	for (AxisId ax_id = 0; ax_id < axes().count(); ax_id++)
	{
		const Axis& a = axes().axis(ax_id);
		for (size_t i=0; i<5; ++i)
		{
			VertTriangle t;
			t.coords_[0] = &coords_[ax_id];
			t.coords_[1] = &coords_[a.near_axes_[i]];
			t.coords_[2] = &coords_[a.near_axes_[(i+1)%5]];
			t.rotate_start_min();

			if (filled_triangles.count(t)>0)
				continue;
			
			FaceTriangleId id = face_triangles_.size();
			face_triangles_.push_back({id, t});
			filled_triangles.insert(t);
		}
	}
	check(face_triangles_.size() == IcosamateInSpace::FACE_COUNT);
}


