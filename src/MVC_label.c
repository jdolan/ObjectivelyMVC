/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_label.h"

/*
 * @brief ViewDraw implementation.
 */
static void MVC_Label_draw(MVC_View *view) {

	MVC_Label *self = (MVC_Label *) view;

	// TODO: Draw it

}

/*
 * @brief LabelRender implementation.
 */
static void MVC_Label_render(MVC_Label *self) {

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, self->text, self->color);
	if (surface) {

	}
}

/*
 * @brief LabelSetText implementation.
 */
static void MVC_Label_setText(MVC_Label *self, const char *text) {

	if (self->text) {
		g_free(self->text);
	}

	if (text) {
		self->text = g_strdup(text);
	} else {
		self->text = NULL;
	}

	self->view.display.needsDisplay = 1;
}

static void MVC_Label_getSize(MVC_Label *self, int *width, int *height) {

	if (self->font && self->text) {
		TTF_SizeUTF8(self->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

MVC_Label *MVC_Label_init(MVC_Label *self, TTF_Font *font, SDL_Color color, const char *text) {

	if (MVC_View_init(&self->super)) {

		self->super.draw = MVC_Label_draw;

		self->render = MVC_Label_render;
		self->getSize = MVC_Label_getSize;

		self->font = font ? font : MVC_DefaultFont();
		self->color = color;
		self->text = g_strdup(text);

	} else {
		g_clear_pointer(&self, g_free);
	}

	return self;
}
