#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#include "def.h"
#include "library.h"
#include "draw/freeimage/FreeImage.h"
#include "draw/GLWindow.h"
#include "explorer.h"

Library& library()
{
	static Library l (get_library_default_path());
	return l;
}

void Library::set_main_dir(const std::string& main_dir)
{
	main_dir_ = main_dir;
}

void save_picture(IcosamateDrawing& icd, const std::string& fname)
{
	typedef unsigned int PixCoord;
	PixCoord w = icd.width(), h = icd.height();

	std::vector<uint8_t> buf (w * h * 4);
	check(!buf.empty());
	glReadPixels(0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, &buf[0]);

	auto dib = FreeImage_Allocate(w, h, 24);
	check(dib);
	for (PixCoord j = 0; j < h; ++j)
	{
		for (PixCoord i = 0; i < w; ++i)
		{
			RGBQUAD* pcolor = reinterpret_cast<RGBQUAD*>(&buf[(w * j + i)*4]);
			check(FreeImage_SetPixelColor(dib, i, j, pcolor));
		}
	}

	fs::remove(fname);
	check(FreeImage_Save(FIF_PNG, dib, fname.c_str()));

	FreeImage_Unload(dib);
}

void Library::save(IcosamateDrawing& icd, std::string subdir/*=""*/)
{
	if (!fs::exists(main_dir_))
		fs::create_directory(main_dir_);
	std::string subd = main_dir_ + subdir;
	if (!fs::exists(subd))
		fs::create_directory(subd);

	std::string pic_fname = subd + "/" + icd.turnig_algorithm() + ".png";
	save_picture(icd, pic_fname);
}

void save_in_library(const TurnAlgS& turn_algs, Library& l, std::string subdir = "")
{
	IcosamateDrawing& d = icd();

	int w = 1200, h = 1200;
	check(GLWindowCreate("saving icosomate algorithms in library", w, h, false));

	d.opengl_init(w, h);

	for (const TurnAlg& turnig_algorithm : turn_algs)
	{
		ActionS acts = IcosamateInSpace::from_str(turnig_algorithm);
		IcosamateInSpace ic;
		ic.actions(acts);
		d.set_icosomate(ic);
		d.set_turnig_algorithm(turnig_algorithm);
		d.update();
		d.render();
		l.save(d, subdir);
	}

	GLWindowDestroy();
}

void save_in_library(const FnameStr& file_name, size_t max_alg_len, const std::string& string_diff, Library& l, std::string subdir)
{
	IcosamateDifference d;
	if (!string_diff.empty())
		d = diff_with_any_from_str(string_diff);
	TurnAlgS algs = read_turn_algs_from_file(file_name, max_alg_len, string_diff.empty() ? nullptr : &d);
	save_in_library(algs, l, subdir);
}
