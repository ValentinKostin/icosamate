#include "../def.h"
#include "text.h"
#include "ft2build.h"
#include FT_FREETYPE_H

TextDrawing::TextDrawing()
{
	FT_Library ft;
	check(FT_Init_FreeType(&ft) == 0);
}

TextDrawing& text_drawing()
{
	static TextDrawing td;
	return td;
}
