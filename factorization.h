#pragma once
#include <vector>
#include <string>

inline std::vector<size_t> factorization(size_t n)
{
	std::vector<size_t> r;
	size_t d = 2;
	while (d<n)
	{
		if (n % d == 0)
		{
			r.push_back(d);
			n /= d;
		}
		else
			++d;
	}
	r.push_back(n);
	return r;
}

inline std::string factorization_str(size_t n)
{
	auto muls = factorization(n);
	std::string r;
	for (auto m : muls)		
		r += std::to_string(m) + "*";
	if (!r.empty())
		r.pop_back();
	return r;
}