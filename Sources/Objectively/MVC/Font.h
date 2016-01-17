/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Font_h_
#define _ObjectivelyMVC_Font_h_

#include <SDL2/SDL_TTF.h>

/*
 * @brief Open a font, located by FontConfig. The font should be freed with
 * TTF_FreeFont.
 */
extern DECLSPEC TTF_Font * SDLCALL OpenFont(const char *family, int ptsize, int style);

/*
 * @brief Open the default font.
 */
extern DECLSPEC TTF_Font * SDLCALL DefaultFont(void);

#endif
