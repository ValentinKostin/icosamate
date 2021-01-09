#pragma once
#include <string>
#include <sstream>
#include <ostream>
#include <map>

#include "factorization.h"
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
inline const IcosamateDifference& with_facorization(const IcosamateDifference& d) { return d; }
inline std::string with_facorization(size_t n)
{
	std::ostringstream oss;
	oss << n << " (" << factorization_str(n) << ")";
	return oss.str();
}

class IcosamateExplorer
{
	std::ostream& log_;

	const IcosamateInSpace ic0_;
	mutable IcosamateInSpace ic_;

	typedef std::map<IcosamateDifference, ActionS> ActMap;
	ActMap actmap_, solving_actmap_;
	typedef std::map<size_t, ActionS> PeriodMap;
	PeriodMap per_map_, solving_per_map_;
	template<class P> void print(const P& actmap, const char* name)
	{
		log_ << name << std::endl;
		for (const auto& p : actmap)
			log_ << with_facorization(p.first) << ": " << to_str(p.second) << std::endl;
	}

	void process_elem(const ActionS& a);

	ActionResult calc_result(const ActionS& aa) const;
	void tree_step(const ActionS& a, bool add_commutators);
	void tree_level(const ActionS& a, size_t max_l, bool add_commutators);

	bool with_solving_ = true;
	bool with_period_ = true;

	void process_actions(const ActionS& a, size_t mul);

public:
	IcosamateExplorer(std::ostream& log);
	void set_with_solving(bool w) { with_solving_ = w; }
	void set_with_period(bool w) { with_period_ = w; }
	void actions(const ActionS& a, size_t mul = 1);
	void tree(size_t n, bool add_commutators);
};

// проверка всевозможных действий для оси и соседних
void explore_near_axis(std::ostream& log);

