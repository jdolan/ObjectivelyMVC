/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include "Option.h"

#define _Class _Option

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Option *this = (Option *) self;

	release(this->title);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	View *this = (View *) self;

	String *classNames = $((Object *) this->classNames, description);
	String *description = str("%s@%p \"%s\" %s [%d, %d, %d, %d]",
							  this->identifier ?: classnameof(self),
							  self,
							  ((Option *) self)->title->text,
							  classNames->chars,
							  this->frame.x, this->frame.y, this->frame.w, this->frame.h);

	release(classNames);
	return description;
}

#pragma mark - View

/**
 * @see View::acceptsFirstResponder(const View *)
 */
static _Bool acceptsFirstResponder(const View *self) {
	return true;
}

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const Option *this = (Option *) self;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("selected", simpleSelector->pattern) == 0) {
				return this->isSelected;
			}
			break;
		default:
			break;
	}

	return super(View, self, matchesSelector, simpleSelector);
}

#pragma mark - Option

/**
 * @fn Option *Option::initWithTitle(Option *self, const char *title, ident value)
 * @memberof Option
 */
static Option *initWithTitle(Option *self, const char *title, ident value) {

	self = (Option *) super(View, self, initWithFrame, NULL);
	if (self) {

		self->title = $(alloc(Text), initWithText, title, NULL);
		assert(self->title);

		self->value = value;

		$((View *) self, addSubview, (View *) self->title);
	}

	return self;
}

/**
 * @fn void Option::setSelected(Option *self, _Bool isSelected)
 * @memberof Option
 */
static void setSelected(Option *self, _Bool isSelected) {

	if (self->isSelected != isSelected) {
		self->isSelected = isSelected;

		self->view.needsLayout = true;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->description = description;

	((ViewInterface *) clazz->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;

	((OptionInterface *) clazz->interface)->initWithTitle = initWithTitle;
	((OptionInterface *) clazz->interface)->setSelected = setSelected;

}

/**
 * @fn Class *Option::_Option(void)
 * @memberof Option
 */
Class *_Option(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Option",
			.superclass = _View(),
			.instanceSize = sizeof(Option),
			.interfaceOffset = offsetof(Option, interface),
			.interfaceSize = sizeof(OptionInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

