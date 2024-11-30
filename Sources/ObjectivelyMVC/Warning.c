/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
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

#include "Warning.h"

#define _Class _Warning

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Warning *this = (Warning *) self;

	release(this->message);

	super(Object, self, dealloc);
}

#pragma mark - Warning

/**
 * @fn Warning *Warning::initWithFormat(Warning *self, WarningLevel level, const char *fmt, ...)
 * @memberof Warning
 */
static Warning *initWithFormat(Warning *self, WarningLevel level, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	self = $(self, initWithVaList, level, fmt, args);

	va_end(args);

	return self;
}

/**
 * @fn Warning *Warning::initWithVaList(Warning *self, WarningLevel level, const char *fmt, va_list args)
 * @memberof Warning
 */
static Warning *initWithVaList(Warning *self, WarningLevel level, const char *fmt, va_list args) {

	self = (Warning *) super(Object, self, init);
	if (self) {
		self->level = level;
		assert(self->level);

		self->message = $(alloc(String), initWithVaList, fmt, args);
		assert(self->message);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((WarningInterface *) clazz->interface)->initWithFormat = initWithFormat;
	((WarningInterface *) clazz->interface)->initWithVaList = initWithVaList;
}

/**
 * @fn Class *Warning::_Warning(void)
 * @memberof Warning
 */
Class *_Warning(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Warning",
			.superclass = _Object(),
			.instanceSize = sizeof(Warning),
			.interfaceOffset = offsetof(Warning, interface),
			.interfaceSize = sizeof(WarningInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
