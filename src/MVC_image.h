/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_image_h
#define _MVC_image_h

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include "MVC_view.h"

Interface(MVC_Image, MVC_View)

	SDL_Texture *texture;
	GLfloat alpha;

	struct {
		GLenum src;
		GLenum dst;
	} blend;

End

Constructor(MVC_Image, SDL_Texture *texture);

#endif
