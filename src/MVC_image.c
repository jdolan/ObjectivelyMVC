/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_image.h"

MVC_Image *MVC_Image_init(MVC_Image *self, SDL_Texture *texture) {

	if (MVC_View_init(&self->super)) {

		self->texture = texture;
		self->alpha = 1.0;

		self->blend.src = GL_SRC_ALPHA;
		self->blend.dst = GL_ONE_MINUS_SRC_ALPHA;

	} else {
		g_clear_pointer(&self, g_free);
	}

	return self;
}
