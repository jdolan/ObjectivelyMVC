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
#include <stdlib.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/SimpleSelector.h>

#define _Class _SimpleSelector

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	SimpleSelector *this = (SimpleSelector *) self;

	free(this->pattern);

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	SimpleSelector *this = (SimpleSelector *) self;

	return HashForCString(HASH_SEED, this->pattern);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _SimpleSelector())) {

		const SimpleSelector *this = (SimpleSelector *) self;
		const SimpleSelector *that = (SimpleSelector *) other;

		return strcmp(this->pattern, that->pattern) == 0;
	}

	return false;
}

#pragma mark - SimpleSelector

/**
 * @fn SimpleSelector *SimpleSelector::initWithPattern(SimpleSelector *self, const char *pattern)
 * @memberof SimpleSelector
 */
static SimpleSelector *initWithPattern(SimpleSelector *self, const char *pattern) {

	self = (SimpleSelector *) super(Object, self, init);
	if (self) {
		self->pattern = strdup(pattern);
		assert(self->pattern);
	}

	return self;
}

/**
 * @return The SimpleSelectorType for the given character.
 */
static SimpleSelectorType simpleSelectorType(const char c) {
	switch (c) {
		case '\0':
			return SimpleSelectorTypeNone;
		case '*':
			return SimpleSelectorTypeUniversal;
		case '#':
			return SimpleSelectorTypeId;
		case '.':
			return SimpleSelectorTypeClass;
		case ':':
			return SimpleSelectorTypePseudo;
		default:
			return SimpleSelectorTypeType;
	}
}

/**
 * @fn Array *SimpleSelector::parse(const char *sequence)
 * @memberof SimpleSelector
 */
static Array *parse(const char *sequence) {

	MutableArray *simpleSelectors = $$(MutableArray, array);
	assert(simpleSelectors);

	const char *c = sequence;
	const char *delim = sequence;
	while (*c) {
		const size_t size = strcspn(c, "*.#:");
		if (size || *c == '*') {
			char *s = calloc(1, size + 1);
			assert(s);

			strncpy(s, c, size);

			SimpleSelector *simpleSelector = $(alloc(SimpleSelector), initWithPattern, s);
			assert(simpleSelector);

			simpleSelector->type = simpleSelectorType(*delim);
			assert(simpleSelector->type);

			delim = c + size;

			$(simpleSelectors, addObject, simpleSelector);

			release(simpleSelector);
			free(s);
		}

		c += size;
		c += strspn(c, "*.#:");
	}

	return (Array *) simpleSelectors;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->hash = hash;
	((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;

	((SimpleSelectorInterface *) clazz->def->interface)->initWithPattern = initWithPattern;
	((SimpleSelectorInterface *) clazz->def->interface)->parse = parse;
}

/**
 * @fn Class *SimpleSelector::_SimpleSelector(void)
 * @memberof SimpleSelector
 */
Class *_SimpleSelector(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "SimpleSelector";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(SimpleSelector);
		clazz.interfaceOffset = offsetof(SimpleSelector, interface);
		clazz.interfaceSize = sizeof(SimpleSelectorInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
