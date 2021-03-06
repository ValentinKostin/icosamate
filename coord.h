#pragma once

#include "icosamate.h"

struct Coord
{
	double x_ = 0;
	double y_ = 0;
	double z_ = 0;

	double norm() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_); }
	double norm_sq() const { return x_ * x_ + y_ * y_ + z_ * z_; }

	inline Coord& operator*=(double t)
	{
		x_ *= t;
		y_ *= t;
		z_ *= t;
		return *this;
	}
	inline Coord& operator+=(const Coord& c)
	{
		x_ += c.x_;
		y_ += c.y_;
		z_ += c.z_;
		return *this;
	}

	Coord& normalize() 
	{
		*this *= 1.0 / norm(); 
		return *this;
	}
};

inline Coord operator-(const Coord& c1)
{
	return { -c1.x_, -c1.y_, -c1.z_ };
}
inline Coord operator+(const Coord& c1, const Coord& c2)
{
	return { c1.x_ + c2.x_, c1.y_ + c2.y_, c1.z_ + c2.z_ };
}
inline Coord operator-(const Coord& c1, const Coord& c2)
{
	return { c1.x_ - c2.x_, c1.y_ - c2.y_, c1.z_ - c2.z_ };
}
inline Coord operator*(const Coord& c1, double a)
{
	return { c1.x_ * a, c1.y_ * a, c1.z_ * a };
}
inline Coord operator/(const Coord& c1, double a)
{
	return { c1.x_ / a, c1.y_ / a, c1.z_ / a };
}

inline Coord vect_prod(const Coord& c1, const Coord& c2) { return { c1.y_ * c2.z_ - c1.z_ * c2.y_, c1.z_ * c2.x_ - c1.x_ * c2.z_, c1.x_ * c2.y_ - c1.y_ * c2.x_, }; }
inline double dist(const Coord& c1, const Coord& c2) { return (c2-c1).norm(); }

typedef std::vector<Coord> CoordS;
inline double dist(const CoordS& cs) 
{
	double r = 0;
	for (size_t i = 0; i + 1 < cs.size(); i++)
		r += dist(cs[i], cs[i+1]);
	return r;
}


struct VertCoord : public Coord
{
	AxisId ax_id_ = 12;
};

typedef size_t FaceTriangleId;

struct VertTriangle
{
	void rotate_start_min(); // ??????????? ???????????	coords_, ????? ? ?????? ??? ???????????
public:
	const VertCoord* coords_[3] = { 0,0,0 }; // ?? ??????? ???????, ?????????? ? ????????????
};

inline bool operator<(const VertTriangle& t1, const VertTriangle& t2)
{
	if (t1.coords_[0]->ax_id_ < t2.coords_[0]->ax_id_)
		return false;
	if (t2.coords_[0]->ax_id_ < t1.coords_[0]->ax_id_)
		return true;
	if (t1.coords_[1]->ax_id_ < t2.coords_[1]->ax_id_)
		return false;
	if (t2.coords_[1]->ax_id_ < t1.coords_[1]->ax_id_)
		return true;
	return t1.coords_[2]->ax_id_ < t2.coords_[2]->ax_id_;
}

class FaceTriangle	: protected VertTriangle
{
	FaceTriangleId id_ = 20;

	std::vector<Coord> subtriangle_coords_;
	void fill_subtriangle_coords();

	std::vector<Coord> sticker_coords_;
	void fill_sticker_coords();

public:
	FaceTriangle(FaceTriangleId id, const VertTriangle& t);

	FaceTriangleId id() const { return id_; }
	const VertCoord& vert_coords(size_t index) const { check(index <= 2); return *coords_[index]; }
	const Coord& vertex_subtriangle_coord(size_t subt_index, size_t pt_index) const { check(subt_index <= 2 && pt_index <= 2);  return subtriangle_coords_[subt_index * 3 + pt_index]; }
	const Coord& center_subtriangle_coord(size_t pt_index) const { check(pt_index <= 2);  return subtriangle_coords_[9 + pt_index]; }

	const Coord& subtriangle_coord(size_t subt_index, size_t pt_index) const;
	const Coord& sticker_coord(size_t subt_index, size_t pt_index) const;
};


// ????? ? 0
class IcosomateCoords
{
	double radius_ = 1.0;	// ?????????? ????
	std::vector<VertCoord> coords_;	  // ?????????? ??????
	void fill_coords();
	std::vector<FaceTriangle> face_triangles_;	  // ?????
	void fill_face_triangles();
public:
	IcosomateCoords(double radius = 1.0);
	double radius() const { return radius_; }
	const VertCoord& vertex(AxisId axis_id) const { return coords_.at(axis_id); }
	const FaceTriangle& face_triangle(FaceTriangleId id) const { return face_triangles_.at(id); }
	Coord face_center(AxisId a1, AxisId a2, AxisId a3) const;
};

// ?1 ? c2 ?? ?????, ?????? ????????????? ????? ????? ????
// ???? ??? ??????????? ???????????? ??????????????, ?? ?????? 0
Coord sphere_middle_point(const Coord& c1, const Coord& c2, double radius);
CoordS define_arc_on_sphere(const Coord& c1, const Coord& c2, double radius);

// ???? ???????? ? ?????????, ???????????????? ax_c ?????? ax_c, 
// c1 ? c2 ????????? ?????????, ? ??????? ????? ????????? ? ???????? ??????-??????? ???? 
CoordS define_arc_around_axis(const Coord& ax_c, const Coord& c1, const Coord& c2, double radius);

// ???? ???????? ? ?????????, ???????????????? ax_c ?????? ax_c, 
// c1 ????????? ?????????, ? ??????? ????? ????????? ??????-?????? ????
CoordS define_arc_around_axis(const Coord& ax_c, const Coord& c1, double angle, bool clockwise, double radius);

enum TransitionMode
{
	TM_BOTH = 0,
	TM_BEGIN,
	TM_END
};
CoordS define_smooth_arc_different_radius(const Coord& c1, const Coord& c2, double big_radius, double transiton_length, TransitionMode mode = TM_BOTH);