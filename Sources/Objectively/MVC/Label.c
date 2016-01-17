/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "Label.h"

#define _Class _Label

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Label *this = (Label *) self;

	SDL_DestroyTexture(this->texture);
	this->texture = NULL;

	free(this->text);
	this->text = NULL;

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::init(id, id, va_list *)
 */
static Label *init(Label *self) {

	self = super(Object, self, init);
	if (self) {

		// TODO
	}

	return self;
}

#pragma mark - ViewInterface

/**
 * @see ViewInterface::draw(View *)
 */
static void draw(View *self) {

	Label *this = (Label *) self;

	if (this->texture == NULL) {
		$(this, render);
	}

	super(View, self, draw);

	SDL_RenderCopy(self->renderer, this->texture, NULL, &self->frame);
}

#pragma mark - LabelInterface

/**
 * @see LabelInterface::render(Label *)
 */
static void render(Label *self) {

	if (self->texture) {
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, self->text, self->color);
	if (surface) {

		self->texture = SDL_CreateTextureFromSurface(((View *) self)->renderer, surface);
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

/**
 * @see LabelInterface::getSize(Label *, int *, int *)
 */
static void getSize(Label *self, int *width, int *height) {

	if (self->font && self->text) {
		TTF_SizeUTF8(self->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

#pragma mark - Label class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	((ObjectInterface *) self->interface)->dealloc = dealloc;
	((ObjectInterface *) self->interface)->init = init;

	((ViewInterface *) self->interface)->draw = draw;

	((LabelInterface *) self->interface)->getSize = getSize;
	((LabelInterface *) self->interface)->render = render;
}

Class _Label = {
	.name = "Label",
	.superclass = &_View,
	.instanceSize = sizeof(Label),
	.interfaceOffset = offsetof(Label, interface),
	.interfaceSize = sizeof(LabelInterface),
	.initialize = initialize,
};

#undef __class
