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
#include <Objectively/String.h>

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

#pragma mark - Option

/**
 * @fn Option *Option::initWithTitle(Option *self, const char *title, ident value)
 *
 * @memberof Option
 */
static Option *initWithTitle(Option *self, const char *title, ident value) {
	
	self = (Option *) super(Object, self, init);
	if (self) {
		
		assert(title);
		assert(value);
		
		self->title = str(title);
		assert(self->title);
		
		self->value = value;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((OptionInterface *) clazz->interface)->initWithTitle = initWithTitle;	
}

Class _Option = {
	.name = "Option",
	.superclass = &_Object,
	.instanceSize = sizeof(Option),
	.interfaceOffset = offsetof(Option, interface),
	.interfaceSize = sizeof(OptionInterface),
	.initialize = initialize,
};

#undef _Class

