/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Image_h_
#define _ObjectivelyMVC_Image_h_

#include <Objectively/MVC/View.h>

/**
 * @file
 *
 * @brief Image implementation.
 */

typedef struct Image Image;
typedef struct ImageInterface ImageInterface;

/**
 * @brief The Image type.
 */
struct Image {

	View view;
	ImageInterface *interface;

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
struct ImageInterface {

	ViewInterface viewInterface;

	Image *(*initWithFrameAndTexture)(Image *self, SDL_Rect *frame, SDL_Texture *texture);
};

extern Class _Image;

#endif
