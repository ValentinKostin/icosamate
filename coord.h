#pragma once

#include "icosamate.h"

struct Coord
{
	double x_ = 0;
	double y_ = 0;
	double z_ = 0;

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


struct VertCoord : public Coord
{
	AxisId ax_id_ = 12;
};

typedef size_t FaceTriangleId;

struct VertTriangle
{
	void rotate_start_min(); // ����������� �����������	coords_, ����� � ������ ��� �����������
public:
	const VertCoord* coords_[3] = { 0,0,0 }; // �� ������� �������, ���������� � ������������
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
	std::vector<Coord> subtriangle_coords_;
	void fill_subtriangle_coords();

	FaceTriangleId id_ = 20;
public:
	FaceTriangle(FaceTriangleId id, const VertTriangle& t);

	FaceTriangleId id() const { return id_; }
	const VertCoord& vert_coords(size_t index) const { check(index <= 2); return *coords_[index]; }
	const Coord& vertex_subtriangle_coord(size_t subt_index, size_t pt_index) const { check(subt_index <= 2 && pt_index <= 2);  return subtriangle_coords_[subt_index * 3 + pt_index]; }
	const Coord& center_subtriangle_coord(size_t pt_index) const { check(pt_index <= 2);  return subtriangle_coords_[9 + pt_index]; }
};


// ����� � 0
class IcosomateCoords
{
	double radius_ = 1.0;	// ���������� ����
	std::vector<VertCoord> coords_;	  // ���������� ������
	void fill_coords();
	std::vector<FaceTriangle> face_triangles_;	  // �����
	void fill_face_triangles();
public:
	IcosomateCoords(double radius = 1.0);
	double radius() const { return radius_; }
	const VertCoord& vertex(AxisId axis_id) const { return coords_.at(axis_id); }
	const FaceTriangle& face_triangle(FaceTriangleId id) const { return face_triangles_.at(id); }
};
