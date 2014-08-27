/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_color_h
#define _MVC_color_h

#include <SDL2/SDL_pixels.h>

#include "MVC_stdinc.h"

struct _MVC_Colors {
	SDL_Color White;
	SDL_Color Black;
	SDL_Color Gray;
	SDL_Color Clear;
};

extern const struct _MVC_Colors MVC_Colors;

#endif
