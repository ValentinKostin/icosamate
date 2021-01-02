#pragma once
#include <vector>

#include "def.h"

typedef int Color;
typedef int VertexId;

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

class Icosamate
{
	const std::vector<VertexElem> vert_elems_;
	std::vector<Face> faces_;
	void fill_faces();
	static const size_t COLORS_COUNT = 20;
public:
	Icosamate();
	bool solved() const;
};
