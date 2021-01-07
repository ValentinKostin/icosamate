#pragma once
#include <string>
#include <ostream>

#include "icosamate.h"

std::string to_str(const ActionS& acts);
ActionS from_str(const std::string& s);

std::ostream& operator<<(std::ostream& oss, const IcosamateDifference& d);

class IcosamateExplorer
{
	std::ostream& log_;

	const IcosamateInSpace ic0_;
	IcosamateInSpace ic_;

public:
	IcosamateExplorer(std::ostream& log);
	void actions(const ActionS& a);
};


