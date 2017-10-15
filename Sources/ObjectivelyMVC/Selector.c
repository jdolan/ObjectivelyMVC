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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Selector.h>

#define _Class _Selector

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Selector *this = (Selector *) self;

	free(this->attribute);
	free(this->identifier);
	free(this->rule);
	free(this->type);

	super(Object, self, dealloc);
}

#pragma mark - Selector

/**
 * @fn Selector *Selector::initWithRule(Selector *self, const char *rule)
 * @memberof Selector
 */
static Selector *initWithRule(Selector *self, const char *rule) {

	assert(rule);

	self = (Selector *) super(Object, self, init);
	if (self) {
		self->rule = strdup(rule);
		assert(self->rule);
	}

	return self;
}

/**
 * @fn Array *Selector::parse(const char *rules)
 * @memberof Selector
 */
static Array *parse(const char *rules) {

	MutableArray *selectors = $$(MutableArray, array);
	assert(selectors);

	const char *c = rules;
	while (*c) {
		const size_t size = strcspn(c, ", ");
		if (size) {
			char *s = calloc(1, size + 1);
			strncpy(s, c, size);

			Selector *selector = $(alloc(Selector), initWithRule, s);
			assert(selector);

			$(selectors, addObject, selector);

			release(selector);
			free(s);
		}
		c += size;
		c += strspn(c, ", ");
	}

	return (Array *) selectors;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((SelectorInterface *) clazz->def->interface)->initWithRule = initWithRule;
	((SelectorInterface *) clazz->def->interface)->parse = parse;
}

/**
 * @fn Class *Selector::_Selector(void)
 * @memberof Selector
 */
Class *_Selector(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Selector";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Selector);
		clazz.interfaceOffset = offsetof(Selector, interface);
		clazz.interfaceSize = sizeof(SelectorInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
