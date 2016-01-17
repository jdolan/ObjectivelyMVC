/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_ImageView_h_
#define _ObjectivelyMVC_ImageView_h_

#include <Objectively/MVC/View.h>

/**
 * @file
 *
 * @brief ImageView implementation.
 */

typedef struct ImageView ImageView;
typedef struct ImageViewInterface ImageViewInterface;

/**
 * @brief The ImageView type.
 */
struct ImageView {

	View view;
	ImageViewInterface *interface;

	SDL_Texture *texture;
	GLfloat alpha;

	struct {
		GLenum src;
		GLenum dst;
	} blend;
};

/**
 * @brief The ImageView interface.
 */
struct ImageViewInterface {

	ViewInterface viewInterface;

	ImageView *(*initWithFrameAndTexture)(ImageView *self, SDL_Rect *frame, SDL_Texture *texture);
};

extern Class _ImageView;

#endif
