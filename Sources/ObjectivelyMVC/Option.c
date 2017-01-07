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
 * @see View::sizeThatFits(View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	const Option *this = (Option *) self;

	SDL_Size size = super(View, self, sizeThatFits);
	SDL_Size titleSize = $((View *) this->title, sizeThatFits);

	titleSize.w += self->padding.left + self->padding.right;
	titleSize.h += self->padding.top + self->padding.bottom;

	size.w = max(size.w, titleSize.w);
	size.h = max(size.h, titleSize.h);

	return size;
}

#pragma mark - Option

/**
 * @fn Option *Option::initWithTitle(Option *self, const char *title, ident value)
 * @memberof Option
 */
static Option *initWithTitle(Option *self, const char *title, ident value) {

	self = (Option *) super(View, self, initWithFrame, NULL);
	if (self) {

		Font *font = $$(Font, defaultFont, FontCategoryPrimaryControl);

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

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;

	((OptionInterface *) clazz->def->interface)->initWithTitle = initWithTitle;
}

/**
 * @fn Class *Option::_Option(void)
 * @memberof Option
 */
Class *_Option(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Option";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(Option);
		clazz.interfaceOffset = offsetof(Option, interface);
		clazz.interfaceSize = sizeof(OptionInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

