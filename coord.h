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
protected:
	void rotate_start_min(); // циклическое перемещение	coords_, чтобы в начале был минимальный
public:
	const Coord* coords_[3]; // по часовой стрелке, начинаются с минимального
};

inline bool operator<(const VertTriangle& t1, const VertTriangle& t2)
{
	// ZAGL 
	return false;
}

struct FaceTriangle	: public VertTriangle
{
	FaceTriangleId id_;
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
	const Coord& coord(AxisId axis_id) const { return coords_.at(axis_id); }
	double radius() const { return radius_; }
};
