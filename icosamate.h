#pragma once
#include <vector>

#include "def.h"
#include "axis.h"
#include "action.h"

typedef int ColorNum;
typedef size_t VertexId;

class ColorNotFound {};
class BadFaceInit {};

struct VertexElem
{
	VertexId id_;  // ��������� � �������� � ������ ������
	std::vector<ColorNum> colors_; // ����� �� ������� �������
	static const size_t COLORS_COUNT = 5;

	size_t color_index(ColorNum c) const;	// ������ � ������ ������ � ��������� ��������
};

typedef size_t VertexElemColorIndex;

struct Face
{
	std::vector<const VertexElem*> vert_elems_;
	std::vector<VertexElemColorIndex> vert_elems_colors_inds_; // ���������, ��� �������� �����. ��������� ��������
	static const size_t VERTEX_ELEM_COUNT = 3;
	ColorNum center_col_;
	bool invariant() const 
	{
		return vert_elems_.size() == VERTEX_ELEM_COUNT && vert_elems_colors_inds_.size() == VERTEX_ELEM_COUNT;
	}
	bool is_one_color() const;
	Face(ColorNum center_col) : center_col_(center_col) {}
	size_t index(VertexId id) const;
};

class Icosamate
{
protected:
	static const std::vector<VertexElem> make_vert_elems();
	static const std::vector<VertexElem> vert_elems_;
	std::vector<Face> faces_;
	void fill_faces();
public:
	static const size_t COLORS_COUNT = 20;
	static const size_t VERTICES_COUNT = 12;
protected:
	void half_turn(VertexId vid, VertexId near_vid[5], size_t n);

	// �������� ������ ��� � ��������������� ������������ � ����� ������ �������
	void turn(VertexId vid, VertexId near_vid[5], VertexId op_vid, VertexId near_op_vid[5], size_t n); // n - ������� ��� ������������ �� ������� �������

	const Face& face(VertexId v0, VertexId v1, VertexId v2) const;
public:
	Icosamate();
	bool solved() const;
};

struct IcosamateDifference
{
	size_t vert_elems_count_ = 0; //  ����� ����������� ��������� ���������
	size_t vert_elems_diff_orient_ = 0;	 //  ����� ��������� ��������� ���������, �� � ������ ����������
	size_t centers_count_ = 0; // ����� ����������� ����������� ���������

	bool empty() const 
	{
		return vert_elems_count_ == 0 && vert_elems_diff_orient_ == 0 && centers_count_ == 0;
	}

	bool operator<(const IcosamateDifference& d) const
	{
		if (vert_elems_count_ < d.vert_elems_count_)
			return true;
		if (d.vert_elems_count_ < vert_elems_count_)
			return false;
		if (vert_elems_diff_orient_ < d.vert_elems_diff_orient_)
			return true;
		if (d.vert_elems_diff_orient_ < vert_elems_diff_orient_)
			return false;
		return centers_count_ < d.centers_count_;
	}
};

// ������������ ������������ ������������� ����
class IcosamateInSpace : public Icosamate
{
public:
private:
	std::vector<VertexId> vert_elem_by_axis_;
	std::vector<AxisId> axis_by_vert_elem_;
	void move_half(AxisId axis_id, size_t n, bool clockwise); // ������� �������� ��� ������� ������ �� ������� ������� ����� ��������� ���
public:
	static const size_t FACE_COUNT = 20;
public:
	IcosamateInSpace();
	// �������� ������ ��� � ��������������� ������������ � ����� ������ ���������, �� �� � ����� ������ �� ������������ � ������������
	void move(AxisId axis_id, size_t n); // ������� ��������� ��� ������� ������ �� ������� ������� ����� ��������� ���
	void turn(AxisId axis_id, size_t n); // ������� �������� ��������� �� ������� ������� ����� ��������� ���

	static Action turn_action(AxisId axis_id, bool clockwise);
	static Action move_action(AxisId axis_id, bool clockwise);
	static Action inverse(Action a);
	static ActionS inverse(const ActionS& a);
	static ActionS commutator(const ActionS& a, int i); // ������� � ����� i � ���������� �� �����������
	static ActionS commutator(const ActionS& a1, const ActionS& a2);
	static size_t period(const ActionS& a);
	static size_t solving_period(const ActionS& a);
	static bool canonic(const ActionS& a);

	void action(Action a);
	void actions(const ActionS& a);

	static IcosamateDifference difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2); // ��������� �� ��������������, ��������� ��������� ��� ����
	static IcosamateDifference solving_difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2);  // ��������� ��� ����� ���������� � ������������

	const Face& face_by_axis(AxisId ax_id_1, AxisId ax_id_2, AxisId ax_id_3) const;
	ColorNum elem_color(const Face& f, AxisId ax_id) const; // ����� ����� ������������� ��� ������� � ��������� ���

	const VertexElem& vertex_elem_by_axis(AxisId ax_id) const;
	AxisId axis_by_vertex(VertexId vid) const { return axis_by_vert_elem_.at(vid); }
};

