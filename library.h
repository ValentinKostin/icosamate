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

void save_in_library(const FnameStr& file_name, size_t max_alg_len, const std::string& string_diff, Library& l, std::string subdir = "");
