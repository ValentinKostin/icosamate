#include "icosamate.h"

size_t VertexElem::color_index(Color c) const
{
	for (size_t i = 0; i < COLORS_COUNT; i++)
	{
		if (colors_[i] == c)
			return i;
	}
	raise("Color not found");
	return -1;
}

bool Face::is_one_color() const
{
	for (size_t i = 0; i < vert_elems_.size(); i++)
	{
		const VertexElem& ve = *vert_elems_[i];
		if (ve.colors_[vert_elems_colors_inds_[i] % VertexElem::COLORS_COUNT] != center_col_)
			return false;
	}
	return true;
}

void Icosamate::fill_faces()
{
	for (Color c = 0; c < COLORS_COUNT; c++)
		faces_.push_back(Face(c));

	for (const VertexElem& ve : vert_elems_)
	{
		for (size_t i=0; i< VertexElem::COLORS_COUNT; ++i)
		{
			Color c = ve.colors_[i];
			Face& f = faces_.at(c);
			f.vert_elems_.push_back(&ve);
			f.vert_elems_colors_inds_.push_back(i);
		}
	}

	for (const Face& f : faces_)
		check(f.invariant());
}

Icosamate::Icosamate() :
	vert_elems_({
		{0, {0, 19, 18, 17, 16}}, { 1, {1, 2, 3, 4, 5} }, {2, {1, 6, 7, 8, 2}}, { 3, {2, 8, 9, 10, 3} },
		{4, {3, 10, 11, 12, 4}}, {	5, {4, 12, 13, 14, 5}}, {6, {1, 5, 14, 15, 6}}, { 7, {7, 16, 17, 9, 8} },
		{8, {9, 17, 18, 11, 10}}, {	9, {11, 18, 19, 13, 12}}, { 10, {0, 15, 14, 13, 19} }, {11, {0, 16, 7, 6, 15} }
		})
{
  fill_faces();

  if (!solved())
	  raise("Bad icosamate init");
}

bool Icosamate::solved() const
{
	for (const auto& f : faces_)
	{
		if (!f.is_one_color())
			return false;
	}
	return true;
}
