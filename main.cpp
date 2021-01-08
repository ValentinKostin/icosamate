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

typedef std::wstring FnameStr;

/////////////////////////////////////////////
FnameStr get_exe_path()
{
	TCHAR lpFilename[256];
	lpFilename[0] = 0;
	GetModuleFileName(0, lpFilename, 256);
	return lpFilename;
}
///////////////////////////////////////////	 
FnameStr change_exe_ext(const wchar_t* new_ext)
{
	FnameStr log_fname = get_exe_path();
	check(log_fname.size() > 4);
	log_fname.resize(log_fname.size() - 4);
	log_fname += new_ext;
	return log_fname;
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
	return change_exe_ext(L".iclog");
}
///////////////////////////////////////////	 
FnameStr default_ini_path()
{
	return change_exe_ext(L".ini");
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
	for (int i = 2; i < argc; ++i)
	{
		std::string s = argv[i];
		size_t k = s.find('=');
		if (k == std::string::npos) throw std::string("Bad argument ") + s;

		r.insert({ s.substr(0, k), s.substr(k + 1) });
	}

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
	log << PROGR_NAME << " " << PROGR_VERSION << "    " << ctimebuf; // std::endl уже вписывается автоматом

	if (argc < 2) usage();
	std::string command = argv[1];

	try
	{
		Args args = get_args(argc, argv);

		if (command == "test")
			test(log);
		if (command == "explore")
		{
			if (args.count("actions") > 0)
			{
				IcosamateExplorer ex(log);
				size_t n = 1;
				if (args.count("n") > 0)
					n = std::stoull(args.at("n"));
				ex.actions(from_str(args.at("actions")), n);
			}
			else if (args.count("tree") > 0)
			{
				IcosamateExplorer ex(log);

				size_t n = std::stoll(args.at("tree"));
				bool add_cooms = args.count("add_commutators") > 0 && args.at("add_commutators") == "1";

				ex.tree(n, add_cooms);
			}
			else if (args.count("type") > 0)
			{
				if (args.at("type") == "near_axis")
					explore_near_axis(log);
			}
		}
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
