/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Label_h_
#define _ObjectivelyMVC_Label_h_

#include <Objectively/MVC/View.h>
#include <Objectively/MVC/Font.h>

typedef struct Label Label;
typedef struct LabelInterface LabelInterface;

struct Label {

	View view;
	LabelInterface *interface;

	TTF_Font *font;
	SDL_Color color;
	char *text;

	SDL_Texture *texture;
};

struct LabelInterface {

	ViewInterface viewInterface;

	void (*render)(Label *self);
	void (*getSize)(Label *self, int *width, int *height);
};

extern Class _Label;

#endif
