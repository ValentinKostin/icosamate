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
	return change_exe_ext(L".log");
}
///////////////////////////////////////////	 
FnameStr default_ini_path()
{
	return change_exe_ext(L".ini");
}
///////////////////////////////////////////	 

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
		Icosamate ic;

		log << "Icosamate created\n";		
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
