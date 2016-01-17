/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _textfield_h_
#define _textfield_h_

#include "view.h"
#include "font.h"

typedef struct TextField TextField;
typedef struct TextFieldInterface TextFieldInterface;

struct TextField {

	View view;
	TextFieldInterface *interface;

	TTF_Font *font;
	SDL_Color color;
	char *text;

	SDL_Texture *texture;
};

struct TextFieldInterface {

	ViewInterface viewInterface;

	void (*render)(TextField *self);
	void (*getSize)(TextField *self, int *width, int *height);
};

extern Class _TextField;

#endif
