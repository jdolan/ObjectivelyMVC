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

#include <ObjectivelyMVC/Option.h>
#include <ObjectivelyMVC/Select.h>

#define _Class _Option

#pragma mark - Object

static void dealloc(Object *self) {
	
	Option *this = (Option *) self;
	
	release(this->title);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::sizeThatFits(View *, int *, int *)
 */
static void sizeThatFits(const View *self, int *w, int *h) {
	
	super(View, self, sizeThatFits, w, h);
	
	const Option *this = (Option *) self;
	
	int tw, th;
	$((View *) this->title, sizeThatFits, &tw, &th);
	
	tw += self->padding.left + self->padding.right;
	th += self->padding.top + self->padding.bottom;
	
	if (tw > *w) {
		*w = tw;
	}
	
	if (th > *h) {
		*h = th;
	}
}

#pragma mark - Option

/**
 * @fn Option *Option::initWithTitle(Option *self, const char *title, Font *font, ident value)
 *
 * @memberof Option
 */
static Option *initWithTitle(Option *self, const char *title, Font *font, ident value) {
	
	self = (Option *) super(View, self, initWithFrame, NULL);
	if (self) {
		
		self->title = $(alloc(Text), initWithText, title, font);
		assert(self->title);
		
		self->value = value;
		
		self->title->view.alignment = ViewAlignmentMiddleLeft;
		$((View *) self, addSubview, (View *) self->title);
		
		self->view.autoresizingMask = ViewAutoresizingWidth;
		
		self->view.frame.h = DEFAULT_OPTION_HEIGHT;
		
		self->view.padding.top = DEFAULT_OPTION_PADDING;
		self->view.padding.right = DEFAULT_OPTION_PADDING;
		self->view.padding.bottom = DEFAULT_OPTION_PADDING;
		self->view.padding.left = DEFAULT_OPTION_PADDING;
		
		$((View *) self, sizeToFit);
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	
	((OptionInterface *) clazz->interface)->initWithTitle = initWithTitle;
}

Class _Option = {
	.name = "Option",
	.superclass = &_View,
	.instanceSize = sizeof(Option),
	.interfaceOffset = offsetof(Option, interface),
	.interfaceSize = sizeof(OptionInterface),
	.initialize = initialize,
};

#undef _Class

