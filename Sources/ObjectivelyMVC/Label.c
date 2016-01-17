/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <ObjectivelyMVC/Label.h>

#define _Class _Label

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Label *this = (Label *) self;

	SDL_DestroyTexture(this->texture);

	free(this->text);

	release(this->font);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::draw(View *)
 */
static void draw(View *self) {

	super(View, self, draw);

	Label *this = (Label *) self;

	SDL_RenderCopy(self->renderer, this->texture, NULL, &self->frame);
}

#pragma mark - Label

/**
 * @fn void Label::getSize(const Label *self, int *width, int *height)
 *
 * @memberof Label
 */
static void getSize(const Label *self, int *width, int *height) {

	if (self->font && self->text) {
		TTF_SizeUTF8(self->font->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

/**
 * @fn Label *Label::initWithText(Label *self, const char *text, Font *font)
 *
 * @memberof Label
 */
static Label *initWithText(Label *self, const char *text, Font *font) {

	self = (Label *) super(View, self, initWithFrame, NULL);
	if (self) {

		if (font) {
			self->font = font;
		} else {
			self->font = $$(Font, defaultFont);
		}

		retain(self->font);

		$(self, setText, text);
		$(self, getSize, &self->view.frame.w, &self->view.frame.h);
	}

	return self;
}

/**
 * @fn void Label::render(Label *self)
 *
 * @memberof Label
 */
static void render(Label *self) {

	if (self->texture) {
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font->font, self->text, self->color);
	if (surface) {

		self->texture = SDL_CreateTextureFromSurface(self->view.renderer, surface);
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
 * @fn void Label::setText(Label *self, const char *text)
 *
 * @memberof Label
 */
static void setText(Label *self, const char *text) {

	free(self->text);

	if (text) {
		self->text = strdup(text);
	} else {
		self->text = NULL;
	}

	$(self, render);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	((ObjectInterface *) self->interface)->dealloc = dealloc;

	((ViewInterface *) self->interface)->draw = draw;

	((LabelInterface *) self->interface)->getSize = getSize;
	((LabelInterface *) self->interface)->initWithText = initWithText;
	((LabelInterface *) self->interface)->render = render;
	((LabelInterface *) self->interface)->setText = setText;
}

Class _Label = {
	.name = "Label",
	.superclass = &_View,
	.instanceSize = sizeof(Label),
	.interfaceOffset = offsetof(Label, interface),
	.interfaceSize = sizeof(LabelInterface),
	.initialize = initialize,
};

#undef _Class
