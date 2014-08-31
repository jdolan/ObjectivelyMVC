/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_label_h
#define _MVC_label_h

#include "MVC_view.h"
#include "MVC_font.h"

Interface(MVC_Label, MVC_View)

	TTF_Font *font;
	SDL_Color color;
	const char *text;

	SDL_Texture *texture;

	void (*render)(MVC_Label *self);
	void (*getSize)(MVC_Label *self, int *width, int *height);

End

Constructor(MVC_Label, SDL_Rect *frame, TTF_Font *font, SDL_Color color, const char *text);

#endif
