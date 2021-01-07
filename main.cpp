#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>

#include <windows.h>

#include "version.h"
#include "icosamate.h"

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

int main()
{
	auto time = std::chrono::system_clock::now();
	std::time_t tt_time = std::chrono::system_clock::to_time_t(time);
	char ctimebuf[32] = { 0, };
	ctime_s(ctimebuf, sizeof(ctimebuf), &tt_time);

	std::ofstream log(default_log_path().c_str(), std::ios_base::app);
	log << "================================================================================" << std::endl;
	log << PROGR_NAME << " " << PROGR_VERSION << "    " << ctimebuf; // std::endl уже вписывается автоматом

	try
	{
		IcosamateInSpace ic0;
		log << "Icosamate created\n";
		check(ic0.solved());

		IcosamateInSpace ic = ic0;

		test_turn(ic, ic0);
		test_move(ic, ic0);

		log << "Icosamate test OK\n";
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
