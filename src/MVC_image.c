/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_image.h"

Implementation(MVC_Image, SDL_Rect *frame, SDL_Texture *texture)

	if (MVC_View_init(&self->super, frame)) {

		self->texture = texture;
		self->alpha = 1.0;

		self->blend.src = GL_SRC_ALPHA;
		self->blend.dst = GL_ONE_MINUS_SRC_ALPHA;

	}

	return self;

End
