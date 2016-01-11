/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_textfield_h_
#define _MVC_textfield_h_

#include "MVC_view.h"
#include "MVC_font.h"

typedef struct MVC_TextField MVC_TextField;
typedef struct MVC_TextFieldInterface MVC_TextFieldInterface;

struct MVC_TextField {

	MVC_View view;
	MVC_TextFieldInterface *interface;

	TTF_Font *font;
	SDL_Color color;
	const char *text;

	SDL_Texture *texture;
};

struct MVC_TextFieldInterface {

	MVC_ViewInterface viewInterface;

	void (*render)(MVC_TextField *self);
	void (*getSize)(MVC_TextField *self, int *width, int *height);
};

extern Class __MVC_TextField;

#endif
