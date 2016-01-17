/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Colors_h_
#define _ObjectivelyMVC_Colors_h_

#include <SDL2/SDL_pixels.h>

/**
 * @brief Color constants.
 */
typedef struct {
	SDL_Color white;
	SDL_Color black;
	SDL_Color gray;
	SDL_Color red;
	SDL_Color green;
	SDL_Color blue;
	SDL_Color clear;
} Colors_t;

extern const Colors_t Colors;

#endif
