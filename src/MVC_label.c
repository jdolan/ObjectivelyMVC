/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_label.h"

static void MVC_Label_dealloc(Object *object) {

	MVC_Label *self = (MVC_Label *) self;

	Super(MVC_View, Object, dealloc);
}

/*
 * @brief ViewDraw implementation.
 */
static void MVC_Label_draw(MVC_View *view) {

	MVC_Label *self = (MVC_Label *) view;

	__MVC_View.draw(view);
}

/*
 * @brief LabelRender implementation.
 */
static void MVC_Label_render(MVC_Label *self) {

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, self->text, self->color);
	if (surface) {

		glBindTexture(GL_TEXTURE_2D, self->texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	}
}

static void MVC_Label_getSize(MVC_Label *self, int *width, int *height) {

	if (self->font && self->text) {
		TTF_SizeUTF8(self->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

MVC_Label *MVC_Label_init(MVC_Label *self, SDL_Window *window, SDL_GLContext *context,
		TTF_Font *font, SDL_Color color, const char *text) {

	if (MVC_View_init(&self->super, window, context)) {

		Override(Object, dealloc, MVC_Label_dealloc);
		Override(MVC_View, draw, MVC_Label_draw);

		self->render = MVC_Label_render;
		self->getSize = MVC_Label_getSize;

		self->font = font ? font : MVC_DefaultFont();
		self->color = color;
		self->text = g_strdup(text);

		glGenTextures(1, &self->texture);
	} else {
		g_clear_pointer(&self, g_free);
	}

	return self;
}
