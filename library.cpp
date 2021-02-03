#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#include "def.h"
#include "library.h"
#include "draw/freeimage/FreeImage.h"

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
