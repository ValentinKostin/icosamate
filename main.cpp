#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <map>

#include <windows.h>

#include "version.h"
#include "icosamate.h"
#include "explorer.h"
#include "library.h"
#include "draw/draw.h"

/////////////////////////////////////////////
FnameStr get_exe_path()
{
	TCHAR lpFilename[256];
	lpFilename[0] = 0;
	GetModuleFileName(0, lpFilename, 256);
	return lpFilename;
}
///////////////////////////////////////////	 
FnameStr change_exe_ext(const char* new_ext)
{
	FnameStr log_fname = get_exe_path();
	check(log_fname.size() > 4);
	log_fname.resize(log_fname.size() - 4);
	log_fname += new_ext;
	return log_fname;
}
///////////////////////////////////////////	 
std::string get_data_full_path(const char* sh_fname)
{
	FnameStr r = get_exe_path();
	auto q = r.find_last_of("/\\");
	r.resize(q);
	r += "/data/";
	r += sh_fname;
	return r;
}
///////////////////////////////////////////	 
std::string get_library_default_path()
{
	FnameStr r = get_exe_path();
	auto q = r.find_last_of("/\\");
	r.resize(q);
	r += "/icl/";
	return r;
}
///////////////////////////////////////////	 
void usage()
{
	std::ostringstream oss;
	oss << PROGR_NAME << " " << PROGR_VERSION << std::endl;
	oss << "Usage:" << std::endl;

	oss << PROGR_NAME".exe" << std::endl;
	throw oss.str();
}
///////////////////////////////////////////	 
FnameStr default_log_path()
{
	return change_exe_ext(".iclog");
}
///////////////////////////////////////////	 
FnameStr default_ini_path()
{
	return change_exe_ext(".ini");
}
///////////////////////////////////////////	

void test_turn(IcosamateInSpace& ic, const IcosamateInSpace& ic0, AxisId axis_id)
{
	ic.turn(axis_id, 1);
	check(!ic.solved());
	IcosamateDifference diff = IcosamateInSpace::difference(ic0, ic);
	check(diff.vert_elems_count_ == 5 && diff.vert_elems_diff_orient_ == 1 && diff.centers_count_ == 10);
	IcosamateDifference sdiff = IcosamateInSpace::solving_difference(ic0, ic);
	check(sdiff.vert_elems_count_ == 5 && sdiff.vert_elems_diff_orient_ == 1 && sdiff.centers_count_ == 10);

	ic.turn(axis_id, 1);
	check(!ic.solved());

	ic.turn(axis_id, 3);
	check(ic.solved());
	IcosamateDifference diff2 = IcosamateInSpace::difference(ic0, ic);
	check(diff2.empty());
	IcosamateDifference sdiff2 = IcosamateInSpace::solving_difference(ic0, ic);
	check(sdiff2.empty());
}

void test_turn(IcosamateInSpace& ic, const IcosamateInSpace& ic0)
{
	for (AxisId axis_id = 0; axis_id < 12; ++axis_id)
		test_turn(ic, ic0, axis_id);
}

void test_move(IcosamateInSpace& ic, const IcosamateInSpace& ic0, AxisId axis_id)
{
	ic.move(axis_id, 1);
	check(ic.solved());
	IcosamateDifference diff = IcosamateInSpace::difference(ic0, ic);
	check(diff.vert_elems_count_ == 10 && diff.vert_elems_diff_orient_ == 2 && diff.centers_count_ == 20);
	IcosamateDifference sdiff = IcosamateInSpace::solving_difference(ic0, ic);
	check(sdiff.empty());

	ic.move(axis_id, 1);
	check(ic.solved());

	ic.move(axis_id, 3);
	check(ic.solved());
	IcosamateDifference diff2 = IcosamateInSpace::difference(ic0, ic);
	check(diff2.empty());
}

void test_move(IcosamateInSpace& ic, const IcosamateInSpace& ic0)
{
	for (AxisId axis_id = 0; axis_id < 12; ++axis_id)
		test_move(ic, ic0, axis_id);
}

void test(std::ostream& log)
{
	IcosamateInSpace ic0;
	log << "Icosamate created\n";
	check(ic0.solved());

	IcosamateInSpace ic = ic0;

	test_turn(ic, ic0);
	test_move(ic, ic0);

	log << "Icosamate test OK\n";
}

typedef std::map<std::string, std::string> Args;

Args get_args(int argc, char* argv[])
{
	Args r;
	for (int i = 1; i < argc; ++i)
	{
		std::string s = argv[i];
		size_t k = s.find('=');
		if (k == std::string::npos) throw std::string("Bad argument ") + s;

		r.insert({ s.substr(0, k), s.substr(k + 1) });
	}

	return r;
}

template<class T> T define_if_exist(const Args& args, const char* arg_name);
template<> std::string define_if_exist<std::string>(const Args& args, const char* arg_name)
{
	std::string r;
	if (args.count(arg_name) > 0)
		r = args.at(arg_name);
	return r;
}
template<> size_t define_if_exist<size_t>(const Args& args, const char* arg_name)
{
	size_t r = 0;
	if (args.count(arg_name) > 0)
		r = std::stoull(args.at(arg_name));
	return r;
}

int main(int argc, char* argv[])
{
	auto time = std::chrono::system_clock::now();
	std::time_t tt_time = std::chrono::system_clock::to_time_t(time);
	char ctimebuf[32] = { 0, };
	ctime_s(ctimebuf, sizeof(ctimebuf), &tt_time);

	std::ofstream log(default_log_path().c_str(), std::ios_base::app);
	log << "================================================================================" << std::endl;
	log << PROGR_NAME << " " << PROGR_VERSION << "    " << ctimebuf; // std::endl ??? ??????????? ?????????


	try
	{
		if (argc <= 1)
		{
			ic_scramble("");
			return 0;
		}
		
		if (argc == 2)
		{
			auto s = std::string(argv[1]);
			if (s.find('=') == std::string::npos && s.size() > 3 && s.substr(s.size() - 4) == ".png")
			{
				open_library_file(s);
				return 0;
			}
		}

		Args args = get_args(argc, argv);

		if (args.count("test") > 0)
			test(log);
		else if (args.count("scramble_info") > 0)
		{
			IcosamateExplorer ex(log);
			ex.scramble_info(args.at("scramble_info"));
		}
		else if (args.count("tree") > 0)
		{
			IcosamateExplorer ex(log);

			bool with_solving = args.count("with_solving") == 0 || args.at("with_solving") != "0";
			ex.set_with_solving(with_solving);
			bool with_period = args.count("with_period") == 0 || args.at("with_period") != "0";
			ex.set_with_period(with_period);
			bool with_mults = args.count("with_mults") == 0 || args.at("with_mults") != "0";
			ex.set_with_mults(with_mults);

			size_t n = std::stoll(args.at("tree"));
			bool add_cooms = args.count("add_commutators") > 0 && args.at("add_commutators") == "1";

			ex.tree(n, add_cooms);
		}
		else if (args.count("special_info") > 0)
		{
			if (args.at("special_info") == "near_axis")
				explore_near_axis(log);
		}
		else if (args.count("scramble")>0)
		{
			std::string turning_algorithm = define_if_exist<std::string>(args, "scramble");
			ic_scramble(turning_algorithm);
		}
		else if (args.count("fill_library_from_file")>0)
		{
			std::string fname = args.at("fill_library_from_file");
			size_t max_alg_len = define_if_exist<size_t>(args, "max_alg_len");
			std::string string_diff = define_if_exist<std::string>(args, "string_diff");
			std::string subdir = define_if_exist<std::string>(args, "subdir");
			save_in_library(fname, max_alg_len, string_diff, library(), subdir);
		}
		else
			raise("Bad command line arguments");
	}
	catch (const std::string& err)
	{
		log << "ERROR: " << err << std::endl;
		return -1;
	}
	catch (const std::exception& ex)
	{
		log << "ERROR: " << ex.what() << std::endl;
		return -2;
	}
	catch (...)
	{
		log << "FATAL ERROR" << std::endl;
		return -5;
	}
	return 0;
}
