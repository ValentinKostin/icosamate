#pragma once
#include <vector>

#include "def.h"

typedef int Color;
typedef size_t VertexId;
typedef size_t AxisId;

class ColorNotFound {};
class BadFaceInit {};

struct VertexElem
{
	VertexId id_;  // совпадает с индексом в наборе вершин
	std::vector<Color> colors_; // обход по часовой стрелке
	static const size_t COLORS_COUNT = 5;

	size_t color_index(Color c) const;
};

typedef size_t VertexElemColorIndex;

struct Face
{
	std::vector<const VertexElem*> vert_elems_;
	std::vector<VertexElemColorIndex> vert_elems_colors_inds_; // описывают, как повёрнуты соотв. вершинные элементы
	static const size_t VERTEX_ELEM_COUNT = 3;
	Color center_col_;
	bool invariant() const 
	{
		return vert_elems_.size() == VERTEX_ELEM_COUNT && vert_elems_colors_inds_.size() == VERTEX_ELEM_COUNT;
	}
	bool is_one_color() const;
	Face(Color center_col) : center_col_(center_col) {}
};

struct Vertex
{
	VertexId id_;
	std::vector < Face* > faces_;  // по часовой стрелке
	static const size_t FACE_COUNT = 5;
	bool invariant() const
	{
		return faces_.size() == FACE_COUNT;
	}
	Vertex(VertexId id) : id_(id) {}
};

class Icosamate
{
protected:
	const std::vector<VertexElem> vert_elems_;
	std::vector<Face> faces_;
	std::vector<Vertex> vertices_;
	void fill_faces();
	static const size_t COLORS_COUNT = 20;
	void fill_vertices();
	static const size_t VERTICES_COUNT = 12;
public:
	Icosamate();
	bool solved() const;
};

struct Axis
{
	AxisId id_;
	static const size_t NEAR_AXIS_COUNT = 5;
	std::vector<AxisId> near_axes_; // обход по часовой стрелке
	bool invariant() const
	{
		return near_axes_.size() == NEAR_AXIS_COUNT;
	}
};

// расположение относительно фиксированных осей 0-11
class IcosamateInSpace : protected Icosamate
{
	const std::vector<Axis> axes_;
	static const size_t AXIS_COUNT = 12;
	std::vector<VertexId> vert_elem_by_axis_;
	std::vector<AxisId> axis_by_vert_elem_;
public:
	IcosamateInSpace();
	void move(AxisId axis_id); // поворот как единого целого по часовой стрелке вдоль указанной оси
	void turn(AxisId axis_id); // поворот половины икосаэдра по часовой стрелке вдоль указанной оси  
};
