#define _USE_MATH_DEFINES

#include "coord.h"
#include <cmath>
#include <set>
#include <list>

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

Coord IcosomateCoords::face_center(AxisId a1, AxisId a2, AxisId a3) const
{
	const auto& c0 = vertex(a1);
	const auto& c1 = vertex(a2);
	const auto& c2 = vertex(a3);
	Coord c = (c0 + c1 + c2) * (1.0 / 3.0);
	return c;
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

// с1 и c2 на сфере, выдаЄт промежуточную точку между ними
// если они расположены диаметрально противоположны, то выдаЄт 0
Coord sphere_middle_point(const Coord& c1, const Coord& c2, double radius)
{
	Coord c = c1 + c2;
	double n1 = c.norm();
	if (n1 < 1e-8)
		return Coord();
	c *= (radius / n1);
	return c;
}

void add_sphere_middle_points(std::list<Coord>& cs, double radius)
{
	check(cs.size() > 1);
	for (auto q = cs.begin(), p = q++; q != cs.end(); p = q++)
		cs.insert(q, sphere_middle_point(*p, *q, radius));
}

CoordS define_arc_on_sphere(const Coord& c1, const Coord& c2, double radius)
{
	std::list<Coord> cs = { c1, c2 };
	for (size_t i = 0; i < 6; i++)
		add_sphere_middle_points(cs, radius);
	return CoordS(cs.begin(), cs.end());
}


Coord around_с_middle_point(const Coord& c0, const Coord& c1, const Coord& c2, double radius)
{
	Coord c = (c1 - c0) + (c2 - c0);
	double n1 = c.norm();
	if (n1 < 1e-8)
		return c0;
	c *= (radius / n1);
	return c0 + c;
}

void add_arc_around_с_middle_points(std::list<Coord>& cs, const Coord& c, double radius)
{
	check(cs.size() > 1);
	for (auto q = cs.begin(), p = q++; q != cs.end(); p = q++)
		cs.insert(q, around_с_middle_point(c, *p, *q, radius));
}

CoordS define_arc_around_с(const Coord& c, const Coord& c1, const Coord& c2, double radius)
{
	std::list<Coord> cs = { c1, c2 };
	for (size_t i = 0; i < 6; i++)
		add_arc_around_с_middle_points(cs, c, radius);
	return CoordS(cs.begin(), cs.end());
}

Coord define_arc_around_axis_radius_vector_point(const Coord& ax_c, const Coord& c, double radius)
{
	Coord v = vect_prod(ax_c, c);
	Coord rr = vect_prod(v, ax_c);
	double n1 = c.norm();
	if (n1 < 1e-8)
		return ax_c;
	rr *= (radius / n1);
	return ax_c + rr;
}

// дуга строитс€ в плоскости, перпендикул€рной ax_c вокруг ax_c, 
// c1 и c2 указывают плоскости, в которых лежат начальный и конечный радиус-векторы дуги 
CoordS define_arc_around_axis(const Coord& ax_c, const Coord& c1, const Coord& c2, double radius)
{
	Coord cr1 = define_arc_around_axis_radius_vector_point(ax_c, c1, radius);
	Coord cr2 = define_arc_around_axis_radius_vector_point(ax_c, c2, radius);
	return define_arc_around_с(ax_c, cr1, cr2, radius);
}

// дуга строитс€ в плоскости, перпендикул€рной ax_c вокруг ax_c, 
// c1 указывает плоскость, в которой лежит начальный радиус-вектор дуги
CoordS define_arc_around_axis(const Coord& ax_c, const Coord& c1, double angle, bool clockwise, double radius)
{
	Coord e1 = vect_prod(ax_c, c1);
	e1.normalize();
	Coord e2 = vect_prod(e1, ax_c);
	e2.normalize();
	double sign = clockwise ? 1.0 : -1.0;
	Coord e = e2 * cos(angle) + e1 * sign * sin(angle);
	Coord cr1 = ax_c + e2 * radius;
	Coord cr2 = ax_c + e * radius;
	return define_arc_around_с(ax_c, cr1, cr2, radius);
}

// f: [0,1] -> [0,1], f(0)=0, f(1)=1, f'(0)>0, f'(1)=0
double smooth_fun(double x) { return 2 * x - x * x; }

CoordS define_smooth_arc_different_radius(const Coord& c1, const Coord& c2, double big_radius, double transiton_length, TransitionMode mode)
{
	double small_radius = (c1.norm() + c2.norm()) * 0.5;
	CoordS cs = define_arc_on_sphere(c1, c2, small_radius);
	CoordS cs_big = define_arc_on_sphere(c1, c2, big_radius);
	check(cs.size() == cs_big.size());

	CoordS r;
	r.reserve(cs.size());
	double length = dist(cs);
	double cur_len = 0, rest_len = length;
	for (size_t i = 0; i < cs.size(); i++)
	{
		if (i > 0)
		{
			double d = dist(cs[i - 1], cs[i]);
			cur_len += d;
			rest_len -= d;
		}

		double big_koef = 1.0;
		if ( mode!= TM_END && cur_len< transiton_length)
			big_koef = smooth_fun(cur_len / transiton_length);
		if (mode != TM_BEGIN && rest_len < transiton_length)
			big_koef = smooth_fun(rest_len / transiton_length);

		Coord c = cs[i] * (1.0 - big_koef) + cs_big[i] * big_koef;
		r.push_back(c);
	}

	return r;
}
