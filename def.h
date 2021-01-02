#pragma once

#include <string>

inline void raise(const char* a) { throw(std::string(a)); }
#define check(a) {if(!(a)) raise(#a);}
