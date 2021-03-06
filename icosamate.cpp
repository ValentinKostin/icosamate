#include  <map>

#include "icosamate.h"

size_t VertexElem::color_index(ColorNum c) const
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

size_t Face::index(VertexId id) const
{
	for (size_t i = 0; i < vert_elems_.size(); i++)
	{
		const VertexElem& ve = *vert_elems_[i];
		if (ve.id_ == id)
			return i;
	}
	return VERTEX_ELEM_COUNT;
}

void Icosamate::fill_faces()
{
	for (ColorNum c = 0; c < COLORS_COUNT; c++)
		faces_.push_back(Face(c));

	for (const VertexElem& ve : vert_elems_)
	{
		for (size_t i=0; i< VertexElem::COLORS_COUNT; ++i)
		{
			ColorNum c = ve.colors_[i];
			Face& f = faces_.at(c);
			f.vert_elems_.push_back(&ve);
			f.vert_elems_colors_inds_.push_back(i);
		}
	}

	for (const Face& f : faces_)
		check(f.invariant());
}

size_t next_index(size_t el_index, size_t n, bool clockwise)
{
	return (el_index + (clockwise ? n : 5-n)) % 5;
}

// грань f имеет ровно одну ссылку из inds, этот индекс в vert_elems_colors_inds_ и выдаётся
size_t NO_UNIQUE_INDEX = Face::VERTEX_ELEM_COUNT;

size_t unique_index(const std::map<VertexId, size_t>& inds, const Face& f)
{
	size_t r = NO_UNIQUE_INDEX;
	for (size_t i=0; i< Face::VERTEX_ELEM_COUNT; ++i)
	{
		const VertexElem* ve = f.vert_elems_[i];
		auto q = inds.find(ve->id_);
		if (q != inds.end())
		{
			if (r != NO_UNIQUE_INDEX)
				return NO_UNIQUE_INDEX;

			r = i;
		}
	}
	return r;
}

struct LayerPicture
{
	Face* face_ptr_ = nullptr;
	size_t vert_elems_ind = NO_UNIQUE_INDEX;
	const VertexElem* vertex_elem_ = nullptr;	 // вершина, которая заменяется при повороте
	VertexElemColorIndex color_index_ = VertexElem::COLORS_COUNT;
};

void Icosamate::half_turn(VertexId vid, VertexId near_vid[5], size_t n)
{
	// ищем те грани, у которых ровно 1 ссылка на элементы из near_vid
	std::map<VertexId, size_t> inds;
	for (size_t i = 0; i < 5; ++i)
		inds.insert({ near_vid[i], i });

	LayerPicture pic[5];
	for (Face& f : faces_)
	{
		size_t ind = unique_index(inds, f);
		if (ind != NO_UNIQUE_INDEX)
		{
			VertexId nvid = f.vert_elems_[ind]->id_;
			size_t f_ind = inds.at(nvid);
			LayerPicture& lp = pic[f_ind];
			lp.face_ptr_ = &f;
			lp.vert_elems_ind = ind;
			lp.vertex_elem_ = f.vert_elems_[lp.vert_elems_ind];
			lp.color_index_ = f.vert_elems_colors_inds_[lp.vert_elems_ind];
		}
	}

	for (size_t i = 0; i < 5; i++)
	{
		LayerPicture& lp = pic[i];
		check(lp.face_ptr_!=nullptr);
		
		size_t repl_index = next_index(i, n, false);
		const LayerPicture& repl_lp = pic[repl_index];

		lp.face_ptr_->vert_elems_[lp.vert_elems_ind] = repl_lp.vertex_elem_;
		lp.face_ptr_->vert_elems_colors_inds_[lp.vert_elems_ind] = repl_lp.color_index_;
	}
}

void Icosamate::turn(VertexId vid, VertexId near_vid[5], VertexId op_vid, VertexId near_op_vid[5], size_t n)
{
	half_turn(vid, near_vid, n);
	half_turn(op_vid, near_op_vid, n);
}

const std::vector<VertexElem> Icosamate::make_vert_elems()
{
	return 
	{
		  {0, {0, 19, 18, 17, 16}}, { 1, {1, 2, 3, 4, 5} }, {2, {1, 6, 7, 8, 2}}, { 3, {2, 8, 9, 10, 3} },
		  {4, {3, 10, 11, 12, 4}}, {	5, {4, 12, 13, 14, 5}}, {6, {1, 5, 14, 15, 6}}, { 7, {7, 16, 17, 9, 8} },
		  {8, {9, 17, 18, 11, 10}}, {	9, {11, 18, 19, 13, 12}}, { 10, {0, 15, 14, 13, 19} }, {11, {0, 16, 7, 6, 15} }
	};
}

const std::vector<VertexElem> Icosamate::vert_elems_ = Icosamate::make_vert_elems();

Icosamate::Icosamate()
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

const Face& Icosamate::face(VertexId v0, VertexId v1, VertexId v2) const
{
	for (const Face& f : faces_)
	{
		if (f.index(v0) == Face::VERTEX_ELEM_COUNT) continue;
		if (f.index(v1) == Face::VERTEX_ELEM_COUNT) continue;
		if (f.index(v2) == Face::VERTEX_ELEM_COUNT) continue;
		return f;
	}
	raise("Cannot find face");
	return faces_[0];
}


IcosamateInSpace::IcosamateInSpace()
{
	for (size_t i = 0; i < vert_elems_.size(); i++)
	{
		vert_elem_by_axis_.push_back(i);
		axis_by_vert_elem_.push_back(i);
	}
}

void IcosamateInSpace::move_half(AxisId axis_id, size_t n, bool clockwise)
{
	const Axis& a = axes()[axis_id];
	AxisId near_axis_id[5];
	for (size_t i = 0; i < 5; i++)
		near_axis_id[i] = a.near_axes_[i];

	VertexId vid = vert_elem_by_axis_[a.id_];
	VertexId near_vid[5];
	for (size_t i = 0; i < 5; i++)
		near_vid[i] = vert_elem_by_axis_[near_axis_id[i]];

	for (size_t i = 0; i < 5; i++)
	{
		VertexId nvid = near_vid[i];
		AxisId aid = near_axis_id[i];
		size_t next_ind = next_index(i, n, !clockwise);
		VertexId new_nvid = near_vid[next_ind];
		axis_by_vert_elem_[new_nvid] = aid;
		vert_elem_by_axis_[aid] = new_nvid;
	}
}

void IcosamateInSpace::move(AxisId axis_id, size_t n)
{
	const Axis& a = axes()[axis_id];
	move_half(a.id_, n, true);
	move_half(a.opposite_id_, n, false);
}

void IcosamateInSpace::turn(AxisId axis_id, size_t n)
{
	const Axis& a = axes()[axis_id];
	AxisId near_axis_id[5];
	for (size_t i = 0; i < 5; i++)
		near_axis_id[i] = a.near_axes_[i];

	VertexId vid = vert_elem_by_axis_[a.id_];
	VertexId near_vid[5];
	for (size_t i = 0; i < 5; i++)
		near_vid[i] = vert_elem_by_axis_[near_axis_id[i]];

	const Axis& a_op = axes()[a.opposite_id_];
	AxisId near_op_axis_id[5];
	for (size_t i = 0; i < 5; i++)
		near_op_axis_id[i] = a_op.near_axes_[i];

	VertexId op_vid = vert_elem_by_axis_[a_op.id_];
	VertexId near_op_vid[5];
	for (size_t i = 0; i < 5; i++)
		near_op_vid[i] = vert_elem_by_axis_[near_op_axis_id[i]];

	Icosamate::turn(vid, near_vid, op_vid, near_op_vid, n);

	for (size_t i = 0; i < 5; i++)
	{
		VertexId nvid = near_vid[i];
		AxisId aid = near_axis_id[i];
		size_t next_ind = next_index(i, n, false);
		VertexId new_nvid = near_vid[next_ind];
		axis_by_vert_elem_[new_nvid] = aid;
		vert_elem_by_axis_[aid] = new_nvid;
	}
}

ActionS IcosamateInSpace::commutator(const ActionS& a1, const ActionS& a2)
{
	ActionS r = a1;
	r.insert(r.end(), a2.begin(), a2.end());
	ActionS a1_inv = inverse(a1);
	r.insert(r.end(), a1_inv.begin(), a1_inv.end());
	ActionS a2_inv = inverse(a2);
	r.insert(r.end(), a2_inv.begin(), a2_inv.end());
	return r;
}

ActionS IcosamateInSpace::commutator(const ActionS& a, int i)
{
	check(i >= 1 && i < a.size());
	ActionS r = a;
	for (int j = i - 1; j >= 0; j--)
		r.push_back(inverse(a[j]));
	for (int j = int(a.size() - 1); j >= i; j--)
		r.push_back(inverse(a[j]));
	return r;
}

void IcosamateInSpace::action(Action a)
{
	if (a > A_12_MOVE_CCW || a < 0)
		return;

	AxisId ax_id = a % 12;
	if (a >= A_1_MOVE_CW)
		move(ax_id, a >= A_1_MOVE_CCW ? 4 : 1);
	else
		turn(ax_id, a >= A_1_TURN_CCW ? 4 : 1);
}

void IcosamateInSpace::actions(const ActionS& aaa)
{
	for (const auto& a : aaa)
		action(a);
}

size_t IcosamateInSpace::period(const ActionS& a)
{
	size_t r = 0;
	IcosamateInSpace ic0;
	IcosamateInSpace ic = ic0;
	do
	{
		ic.actions(a);
		++r;
	} while (!difference(ic0, ic).empty());
	return r;
}

size_t IcosamateInSpace::solving_period(const ActionS& a)
{
	size_t r = 0;
	IcosamateInSpace ic;
	do
	{
		ic.actions(a);
		++r;
	} while (!ic.solved());
	return r;
}

bool IcosamateInSpace::canonic(const ActionS& a)
{
	for (size_t i = 1; i < a.size(); ++i)
		if (inverse(a[i - 1]) == a[i])
			return false;

	return true;
}

IcosamateDifference IcosamateInSpace::difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2)
{
	IcosamateDifference r;
	for (const Axis& a : axes().axes())
	{
		VertexId v1_0 = i1.vert_elem_by_axis_[a.id_];
		VertexId v2_0 = i2.vert_elem_by_axis_[a.id_];
		if (v1_0 != v2_0)
			++r.vert_elems_count_;
		else
		{
			VertexId v1_1 = i1.vert_elem_by_axis_[a.near_axes_[0]];
			VertexId v2_1 = i2.vert_elem_by_axis_[a.near_axes_[0]];
			VertexId v1_2 = i1.vert_elem_by_axis_[a.near_axes_[1]];
			VertexId v2_2 = i2.vert_elem_by_axis_[a.near_axes_[1]];
			const Face& f1 = i1.face(v1_0, v1_1, v1_2);
			const Face& f2 = i2.face(v2_0, v2_1, v2_2);
			if ( f1.vert_elems_colors_inds_[ f1.index(v1_0) ] != f2.vert_elems_colors_inds_[f2.index(v2_0)] )
				++r.vert_elems_diff_orient_;
		}
	}

	for (const Face& f1 : i1.faces_)
	{
		VertexId v2_0 = i2.vert_elem_by_axis_[i1.axis_by_vert_elem_[f1.vert_elems_[0]->id_]];
		VertexId v2_1 = i2.vert_elem_by_axis_[i1.axis_by_vert_elem_[f1.vert_elems_[1]->id_]];
		VertexId v2_2 = i2.vert_elem_by_axis_[i1.axis_by_vert_elem_[f1.vert_elems_[2]->id_]];
		const Face& f2 = i2.face(v2_0, v2_1, v2_2);
		if (f1.center_col_!=f2.center_col_)
			++r.centers_count_;
	}

	return r;
}

IcosamateDifference IcosamateInSpace::solving_difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2)
{
	IcosamateDifference r = difference(i1, i2);
	IcosamateInSpace ic = i2;
	for (const Axis& a : axes().axes())
	{
		for (size_t i=0; i<Axis::NEAR_AXIS_COUNT; ++i)
		{
			ic.move(a.id_, 1);
			IcosamateDifference d = difference(i1, ic);
			if (d < r)
				r = d;
		}
	}
	return r;
}

const Face& IcosamateInSpace::face_by_axis(AxisId ax_id_1, AxisId ax_id_2, AxisId ax_id_3) const
{
	VertexId v1 = vert_elem_by_axis_[ax_id_1];
	VertexId v2 = vert_elem_by_axis_[ax_id_2];
	VertexId v3 = vert_elem_by_axis_[ax_id_3];
	return face(v1, v2, v3);
}

ColorNum IcosamateInSpace::elem_color(const Face& f, AxisId ax_id) const
{
	VertexId v1 = vert_elem_by_axis_[ax_id];
	size_t index = f.index(v1);
	const VertexElem& ve = *f.vert_elems_[index];
	VertexElemColorIndex cind = f.vert_elems_colors_inds_[index];
	return ve.colors_[cind];
}

const VertexElem& IcosamateInSpace::vertex_elem_by_axis(AxisId ax_id) const
{
	VertexId v1 = vert_elem_by_axis_.at(ax_id);
	return vert_elems_.at(v1);
}
