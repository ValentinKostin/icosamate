#define _USE_MATH_DEFINES

#include "coord.h"
#include <cmath>

IcosomateCoords::IcosomateCoords(double radius) : radius_(radius)
{
	fill_coords();
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

void IcosomateCoords::fill_face_triangles()
{

}


