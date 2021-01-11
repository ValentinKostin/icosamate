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
	size_t index(VertexId id) const;
};

//struct Vertex
//{
//	VertexId id_;
//	std::vector < Face* > faces_;  // по часовой стрелке
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

	// вращения вокруг оси и противоположной эквивалентны с точки зрения картины
	void turn(VertexId vid, VertexId near_vid[5], VertexId op_vid, VertexId near_op_vid[5], size_t n); // n - сколько раз поворачивать по часовой стрелке

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
	std::vector<AxisId> near_axes_; // обход по часовой стрелке
	bool invariant() const
	{
		return near_axes_.size() == NEAR_AXIS_COUNT;
	}
};

struct IcosamateDifference
{
	size_t vert_elems_count_ = 0; //  число несовпавших вершинных элементов
	size_t vert_elems_diff_orient_ = 0;	 //  число совпавших вершинных элементов, но в другой ориентации
	size_t centers_count_ = 0; // число несовпавших центральных элементов

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

// фиксированные оси 0-11
class Axes
{
	static const std::vector<Axis> make_axes();
	const std::vector<Axis> axes_; // первые 6 - основные оси, остальные - им противоположные
public:
	Axes();
	const Axis& axis(AxisId ax_id) const { return axes_.at(ax_id); }
	const Axis& operator[](AxisId ax_id) const { return axes_.at(ax_id); }
	const std::vector<Axis>& axes() const { return axes_; }
	size_t count() const { return 12; }
};

const Axes& axes();

// расположение относительно фиксированных осей
class IcosamateInSpace : public Icosamate
{
public:
private:
	std::vector<VertexId> vert_elem_by_axis_;
	std::vector<AxisId> axis_by_vert_elem_;
	void move_half(AxisId axis_id, size_t n, bool clockwise); // поворот половины как единого целого по часовой стрелке вдоль указанной оси
public:
	static const size_t FACE_COUNT = 20;
public:
	IcosamateInSpace();
	// вращения вокруг оси и противоположной эквивалентны с точки зрения элементов, но не с точки зрения их расположения в пространстве
	void move(AxisId axis_id, size_t n); // поворот икосаэдра как единого целого по часовой стрелке вдоль указанной оси
	void turn(AxisId axis_id, size_t n); // поворот половины икосаэдра по часовой стрелке вдоль указанной оси

	static Action turn_action(AxisId axis_id, bool clockwise);
	static Action move_action(AxisId axis_id, bool clockwise);
	static Action inverse(Action a);
	static ActionS inverse(const ActionS& a);
	static ActionS commutator(const ActionS& a, int i); // разбить в месте i и продолжить до коммутатора
	static ActionS commutator(const ActionS& a1, const ActionS& a2);
	static size_t period(const ActionS& a);
	static size_t solving_period(const ActionS& a);
	static bool canonic(const ActionS& a);

	void action(Action a);
	void actions(const ActionS& a);

	static IcosamateDifference difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2); // икосаэдры не поворачиваются, смотрится сравнение как есть
	static IcosamateDifference solving_difference(const IcosamateInSpace& i1, const IcosamateInSpace& i2);  // сравнение без учёта ориентации в пространстве
};

