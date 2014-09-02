/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_label.h"

static void MVC_Label_dealloc(MVC_Label *self) {

	Super(Object, self, dealloc);
}

/*
 * @brief ViewDraw implementation.
 */
static void MVC_Label_draw(MVC_Label *self) {

	if (self->texture == NULL) {
		self->render(self);
	}

	Super(MVC_View, self, draw);

	SDL_RenderCopy(self->super.renderer, self->texture, NULL, &self->super.frame);
}

/*
 * @brief LabelRender implementation.
 */
static void MVC_Label_render(MVC_Label *self) {

	if (self->texture) {
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, self->text, self->color);
	if (surface) {

		self->texture = SDL_CreateTextureFromSurface(self->super.renderer, surface);
		if (self->texture) {
			SDL_LogVerbose(0, "%s: Created texture for \"%s\"", __func__, self->text);
		} else {
			SDL_LogError(0, "%s: Failed to create texture for \"%s\"", __func__, self->text);
		}

		SDL_FreeSurface(surface);
	} else {
		SDL_LogError(0, "%s: Failed to render \"%s\"", __func__, self->text);
	}
}

static void MVC_Label_getSize(MVC_Label *self, int *width, int *height) {

	if (self->font && self->text) {
		TTF_SizeUTF8(self->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

Implementation(MVC_Label, SDL_Rect *frame, TTF_Font *font, SDL_Color color, const char *text)

	if (MVC_View_init(&self->super, frame)) {

		Override(Object, self, dealloc, MVC_Label_dealloc);

		Override(MVC_View, self, draw, MVC_Label_draw);

		self->render = MVC_Label_render;
		self->getSize = MVC_Label_getSize;

		self->font = font ? font : MVC_DefaultFont();
		self->color = color;
		self->text = g_strdup(text);
	}

	return self;

End
