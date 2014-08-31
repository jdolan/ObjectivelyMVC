/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_font_h
#define _MVC_font_h

#include <SDL2/SDL_TTF.h>

#include "MVC_stdinc.h"

/*
 * @brief Open a font, located by FontConfig. The font should be freed with
 * TTF_FreeFont.
 */
extern DECLSPEC TTF_Font * SDLCALL MVC_OpenFont(const char *family, int ptsize, int style);

/*
 * @brief Open the default font.
 */
extern DECLSPEC TTF_Font * SDLCALL MVC_DefaultFont(void);

#endif
