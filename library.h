#pragma once
#include <string>
#include "draw/draw_impl.h"

class Library
{
	std::string main_dir_;
public:
	Library(const std::string& main_dir) : main_dir_(main_dir) {}
	void set_main_dir(const std::string& main_dir);
	void save(IcosamateDrawing& icd, std::string subdir="");
};

Library& library();