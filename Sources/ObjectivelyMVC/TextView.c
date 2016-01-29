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

#include <ObjectivelyMVC/TextView.h>

#define _Class _TextView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TextView *this = (TextView *) self;
	
	release(this->font);

	free(this->text);
	
	SDL_DestroyTexture(this->texture);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	super(View, self, render, renderer);
	
	TextView *this = (TextView *) self;
	
	char *text = this->text ?: this->defaultText;
	if (text) {
		
		if (this->texture == NULL) {
			SDL_Surface *surface = $(this->font, render, text, this->color);
			this->texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
		}
		
		assert(this->texture);
		
		SDL_Rect frame = $(self, renderFrame);
		SDL_RenderCopy(renderer, this->texture, NULL, &frame);
	}
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame)
 *
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame) {

	self = (TextView *) super(Control, self, initWithFrame, frame);
	if (self) {

		self->color = Colors.ForegroundColor;
		
		self->editable = true;
		
		$(self, setFont, NULL);
		$(self, setText, NULL);
		
		self->control.bevel = BevelTypeInset;
		
		if (self->control.view.frame.w == 0) {
			self->control.view.frame.w = DEFAULT_TEXTVIEW_WIDTH;
		}
	}

	return self;
}

/**
 * @fn void TextView::setFont(TextView *self, Font *font)
 *
 * @memberof TextView
 */
static void setFont(TextView *self, Font *font) {
	
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
 * @fn void TextView::setText(TextView *self, const char *text)
 *
 * @memberof TextView
 */
static void setText(TextView *self, const char *text) {
	
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
	
	$((View *) self, sizeToFit);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->render = render;

	((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((TextViewInterface *) clazz->interface)->setFont = setFont;
	((TextViewInterface *) clazz->interface)->setText = setText;
}

Class _TextView = {
	.name = "TextView",
	.superclass = &_Control,
	.instanceSize = sizeof(TextView),
	.interfaceOffset = offsetof(TextView, interface),
	.interfaceSize = sizeof(TextViewInterface),
	.initialize = initialize,
};

#undef _Class
