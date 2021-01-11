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
	VertexId id_;  // ��������� � �������� � ������ ������
	std::vector<Color> colors_; // ����� �� ������� �������
	static const size_t COLORS_COUNT = 5;

	size_t color_index(Color c) const;
};

typedef size_t VertexElemColorIndex;

struct Face
{
	std::vector<const VertexElem*> vert_elems_;
	std::vector<VertexElemColorIndex> vert_elems_colors_inds_; // ���������, ��� �������� �����. ��������� ��������
	static const size_t VERTEX_ELEM_COUNT = 3;
	Color center_col_;
	bool invariant() const 
	{
		return vert_elems_.size() == VERTEX_ELEM_COUNT && vert_elems_colors_inds_.size() == VERTEX_ELEM_COUNT;
	}
	bool is_one_color() const;
	Face(Color center_col) : center_col_(center_col) {}
	size_t index(VertexId id) const;
};

//struct Vertex
//{
//	VertexId id_;
//	std::vector < Face* > faces_;  // �� ������� �������
//	static const size_t FACE_COUNT = 5;
//	bool invariant() const
//	{
//		return faces_.size() == FACE_COUNT;
//	}
//	Vertex(VertexId id) : id_(id) {}
//};

class Icosamate
{
protected:
	static const std::vector<VertexElem> make_vert_elems();
	static const std::vector<VertexElem> vert_elems_;
	std::vector<Face> faces_;
//	std::vector<Vertex> vertices_;
	void fill_faces();
	static const size_t COLORS_COUNT = 20;
//	void fill_vertices();
	static const size_t VERTICES_COUNT = 12;
	void half_turn(VertexId vid, VertexId near_vid[5], size_t n);

	// �������� ������ ��� � ��������������� ������������ � ����� ������ �������
	void turn(VertexId vid, VertexId near_vid[5], VertexId op_vid, VertexId near_op_vid[5], size_t n); // n - ������� ��� ������������ �� ������� �������

	const Face& face(VertexId v0, VertexId v1, VertexId v2) const;
public:
	Icosamate();
	bool solved() const;
};

struct Axis
{
	AxisId id_;
	AxisId opposite_id_;
	static const size_t NEAR_AXIS_COUNT = 5;
	std::vector<AxisId> near_axes_; // ����� �� ������� �������
	bool invariant() const
	{
		return near_axes_.size() == NEAR_AXIS_COUNT;
	}
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

typedef int Action;
static const Action A_NO_ACTION = 0;
static const Action A_1_TURN_CW = 1;
static const Action A_12_TURN_CW = 12;
static const Action A_1_TURN_CCW = 13;
static const Action A_12_TURN_CCW = 24;
static const Action A_1_MOVE_CW = 25;
static const Action A_12_MOVE_CW = 36;
static const Action A_1_MOVE_CCW = 37;
static const Action A_12_MOVE_CCW = 48;
typedef std::vector<Action>	ActionS;

// ������������� ��� 0-11
class Axes
{
	static const std::vector<Axis> make_axes();
	const std::vector<Axis> axes_; // ������ 6 - �������� ���, ��������� - �� ���������������
public:
	Axes();
	const Axis& axis(AxisId ax_id) const { return axes_.at(ax_id); }
	const Axis& operator[](AxisId ax_id) const { return axes_.at(ax_id); }
	const std::vector<Axis>& axes() const { return axes_; }
	size_t count() const { return 12; }
};

const Axes& axes();

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
};

