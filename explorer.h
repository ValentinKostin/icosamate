#pragma once
#include <string>
#include <ostream>

#include "icosamate.h"

std::string to_str(const ActionS& acts);
ActionS from_str(const std::string& s);


class IcosamateExplorer
{
	std::ostream& log_;
public:
	IcosamateExplorer(std::ostream& log);
	void actions(const ActionS& a);
};


