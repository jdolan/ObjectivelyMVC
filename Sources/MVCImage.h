/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_image_h_
#define _MVC_image_h_

#include "MVC_view.h"

/**
 * @file
 *
 * @brief Image implementation.
 */

typedef struct MVC_Image MVC_Image;
typedef struct MVC_ImageInterface MVC_ImageInterface;

/**
 * @brief The Image type.
 */
struct MVC_Image {

	MVC_View view;
	MVC_ImageInterface *interface;

	SDL_Texture *texture;
	GLfloat alpha;

	struct {
		GLenum src;
		GLenum dst;
	} blend;
};

/**
 * @brief The Image interface.
 */
struct MVC_ImageInterface {

	MVC_ViewInterface viewInterface;

	MVC_Image *(*initWithFrameAndTexture)(MVC_Image *self, SDL_Rect *frame, SDL_Texture *texture);
};

extern Class _MVC_Image;

#endif
