#pragma once
#include <string>
#include <ostream>
#include <map>

#include "icosamate.h"

std::string to_str(const ActionS& acts);
ActionS from_str(const std::string& s);

std::ostream& operator<<(std::ostream& oss, const IcosamateDifference& d);

struct ActionResult
{
	IcosamateDifference diff_;
	IcosamateDifference solved_diff_;
	size_t period_ = 0;
	size_t solved_period_ = 0;
};

std::ostream& operator<<(std::ostream& oss, const ActionResult& r);

class IcosamateExplorer
{
	std::ostream& log_;

	const IcosamateInSpace ic0_;
	IcosamateInSpace ic_;
	ActionS actions_;

	typedef std::multimap<IcosamateDifference, ActionS> ActMap;
	ActMap actmap_, solving_actmap_;
	void process_elem(const ActionS& a);

	ActionResult calc_result() const;
	void tree_step(const ActionS& a);

public:
	IcosamateExplorer(std::ostream& log);
	void actions(const ActionS& a, size_t mul=1);
	void tree(size_t n, bool add_commutators = true);
};

// проверка всевозможных действий для оси и соседних
void explore_near_axis(std::ostream& log);

