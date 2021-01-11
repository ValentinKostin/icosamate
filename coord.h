#pragma once

#include "icosamate.h"

struct Coord
{
	AxisId ax_id_ = 12;
	double x_ = 0;
	double y_ = 0;
	double z_ = 0;
};

typedef size_t FaceTriangleId;

struct VertTriangle
{
public:
	void rotate_start_min(); // циклическое перемещение	coords_, чтобы в начале был минимальный
public:
	const Coord* coords_[3] = { 0,0,0 }; // по часовой стрелке, начинаются с минимального
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

struct FaceTriangle	: public VertTriangle
{
	FaceTriangleId id_ = 20;
	FaceTriangle(FaceTriangleId id, const VertTriangle& t) : VertTriangle(t), id_(id) {}
};


// центр в 0
class IcosomateCoords
{
	double radius_ = 1.0;	// описанного шара
	std::vector<Coord> coords_;	  // координаты вершин
	void fill_coords();
	std::vector<FaceTriangle> face_triangles_;	  // грани
	void fill_face_triangles();
public:
	IcosomateCoords(double radius = 1.0);
	double radius() const { return radius_; }
	const Coord& vertex(AxisId axis_id) const { return coords_.at(axis_id); }
	const FaceTriangle& face_triangle(FaceTriangleId id) const { return face_triangles_.at(id); }
};
