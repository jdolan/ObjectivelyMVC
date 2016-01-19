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

#include <assert.h>

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
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {

	super(View, self, render, renderer);

	Label *this = (Label *) self;
	
	if (this->text) {
		
		if (this->texture == NULL) {
			SDL_Surface *surface = TTF_RenderUTF8_Blended(this->font->font, this->text, this->color);
			this->texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
		}
		
		assert(this->texture);
		
		SDL_Rect frame = $(self, renderFrame);
		SDL_RenderCopy(renderer, this->texture, NULL, &frame);
	}
}

#pragma mark - Label

/**
 * @fn Label *Label::initWithText(Label *self, const char *text, Font *font)
 *
 * @memberof Label
 */
static Label *initWithText(Label *self, const char *text, Font *font) {

	self = (Label *) super(View, self, initWithFrame, NULL);
	if (self) {

		self->color = Colors.foregroundColor;

		$(self, setFont, font);
		$(self, setText, text);
		
		$(self, naturalSize, &self->view.frame.w, &self->view.frame.h);
	}

	return self;
}

/**
 * @fn void Label::setFont(Label *self, Font *font)
 *
 * @memberof Label
 */
static void setFont(Label *self, Font *font) {
	
	if (font == NULL) {
		font = $$(Font, defaultFont);
	}
	
	if (font != self->font) {
		
		release(self->font);
		self->font = retain(font);
		
		if (self->texture) {
			SDL_DestroyTexture(self->texture);
			self->texture = NULL;
		}
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
	
	if (self->texture) {
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}
}

/**
 * @fn void Label::naturalSize(const Label *self, int *width, int *height)
 *
 * @memberof Label
 */
static void naturalSize(const Label *self, int *width, int *height) {
	
	if (self->font && self->text) {
		TTF_SizeUTF8(self->font->font, self->text, width, height);
	} else {
		*width = *height = 0;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	((ObjectInterface *) self->interface)->dealloc = dealloc;

	((ViewInterface *) self->interface)->render = render;

	((LabelInterface *) self->interface)->initWithText = initWithText;
	((LabelInterface *) self->interface)->naturalSize = naturalSize;
	((LabelInterface *) self->interface)->setFont = setFont;
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
