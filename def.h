#pragma once

#include <string>

inline void raise(const char* a) { throw(std::string(a)); }
inline void raise(const std::string& a) { throw a; }
#define check(a) {if(!(a)) raise(#a);}

typedef std::string FnameStr;

std::string get_data_full_path(const char* sh_fname);