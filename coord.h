#pragma once

#include "icosamate.h"

struct Coord
{
	double x_ = 0;
	double y_ = 0;
	double z_ = 0;

	double norm() const { return sqrt(x_ * x_ + y_ * y_ + z_ * z_); }

	inline Coord& operator*=(double t) 
	{
		x_ *= t; 
		y_ *= t;
		z_ *= t;
		return *this;
	}
};

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

typedef std::vector<Coord> CoordS;


struct VertCoord : public Coord
{
	AxisId ax_id_ = 12;
};

typedef size_t FaceTriangleId;

struct VertTriangle
{
	void rotate_start_min(); // циклическое перемещение	coords_, чтобы в начале был минимальный
public:
	const VertCoord* coords_[3] = { 0,0,0 }; // по часовой стрелке, начинаются с минимального
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


// центр в 0
class IcosomateCoords
{
	double radius_ = 1.0;	// описанного шара
	std::vector<VertCoord> coords_;	  // координаты вершин
	void fill_coords();
	std::vector<FaceTriangle> face_triangles_;	  // грани
	void fill_face_triangles();
public:
	IcosomateCoords(double radius = 1.0);
	double radius() const { return radius_; }
	const VertCoord& vertex(AxisId axis_id) const { return coords_.at(axis_id); }
	const FaceTriangle& face_triangle(FaceTriangleId id) const { return face_triangles_.at(id); }
};

// с1 и c2 на сфере, выдаёт промежуточную точку между ними
// если они расположены диаметрально противоположны, то выдаёт 0
Coord sphere_middle_point(const Coord& c1, const Coord& c2, double radius);
CoordS define_arc_on_sphere(const Coord& c1, const Coord& c2, double radius);
