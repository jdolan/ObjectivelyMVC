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
	
	release(this->text);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {
	
	super(View, self, layoutSubviews);
	
	TextView *this = (TextView *) self;
	
	if (this->text->text == NULL) {
		$(this->text, setText, this->defaultText);
	}
}

/**
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	super(View, self, render, renderer);
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style) {

	self = (TextView *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		
		self->editable = true;
		
		self->text = $(alloc(Label), initWithText, NULL, NULL);
		assert(self->text);
		
		$((View *) self, addSubview, (View *) self->text);
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = BevelTypeInset;
			
			self->control.view.backgroundColor = Colors.DimGray;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_TEXTVIEW_WIDTH;
			}
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->render = render;

	((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
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
