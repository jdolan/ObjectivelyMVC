/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <fontconfig/fontconfig.h>

#include "MVC_font.h"

TTF_Font *OpenFont(const char *family, int ptsize, int style) {

	FcConfig *config = FcConfigGetCurrent();
	if (config) {

		FcPattern *pattern = FcPatternCreate();

		FcPatternAddString(pattern, FC_FAMILY, family);
		FcPatternAddDouble(pattern, FC_SIZE, (double) ptsize);

		if (style & TTF_STYLE_BOLD) {
			if (style & TTF_STYLE_ITALIC) {
				FcPatternAddString(pattern, FC_STYLE, "Bold Italic");
			} else {
				FcPatternAddString(pattern, FC_STYLE, "Bold");
			}
		} else if (style & TTF_STYLE_ITALIC) {
			FcPatternAddString(pattern, FC_STYLE, "Italic");
		} else {
			FcPatternAddString(pattern, FC_STYLE, "Regular");
		}

		FcPatternAddString(pattern, FC_FONTFORMAT, "TrueType");

		FcObjectSet *objects = FcObjectSetCreate();

		FcObjectSetAdd(objects, FC_FILE);
		FcObjectSetAdd(objects, FC_INDEX);

		FcFontSet *fonts = FcFontList(config, pattern, objects);
		FcPattern **f = fonts->fonts;
		for (int i = 0; i < fonts->nfont; i++, f++) {

			unsigned char *path;
			if (FcPatternGetString(*f, FC_FILE, 0, &path) == FcResultMatch) {

				int index;
				if (FcPatternGetInteger(*f, FC_INDEX, 0, &index) == FcResultMatch) {
					SDL_LogDebug(0, "Matched font %s (%d)", path, index);

					TTF_Font *font = TTF_OpenFont(path, ptsize);
					if (font) {
						TTF_SetFontStyle(font, style);
						return font;
					}
				}
			}
		}
	}

	return NULL;
}

TTF_Font *DefaultFont(void) {
	return OpenFont("Sans", 12, 0);
}
