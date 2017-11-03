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

#include <ObjectivelyMVC/Selector.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Selector

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Selector *this = (Selector *) self;

	release(this->sequences);
	free(this->rule);

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Selector *this = (Selector *) self;

	return $((Object *) this->rule, hash);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Selector())) {

		const Selector *this = (Selector *) self;
		const Selector *that = (Selector *) other;

		return $((Object *) this->rule, isEqual, (Object *) that->rule);
	}

	return false;
}

#pragma mark - Selector

/**
 * @fn Selector *Selector::initWithRule(Selector *self, const char *rule)
 * @memberof Selector
 * Control:focused
 */
static Selector *initWithRule(Selector *self, const char *rule) {

	self = (Selector *) super(Object, self, init);
	if (self) {
		self->rule = strdup(rule);
		assert(self->rule);

		self->sequences = $$(SelectorSequence, parse, rule);
		assert(self->sequences->count);
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
		const size_t size = strcspn(c, ",");
		if (size) {
			char *s = calloc(1, size + 1);
			assert(s);

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

/**
 * @brief A context for View selection state.
 */
typedef struct {
	const Selector *selector;
	const SelectorSequence **sequence;
	MutableArray *selection;
} Context;

/**
 * @brief Recursively selects Views by iterating the SelectorSequences in the given Context.
 */
static Array *_select(View *view, const Context *context) {

	const SelectorSequence *sequence = *context->sequence;

	if ($(sequence, matchesView, view)) {

		switch (sequence->combinator) {
			case SequenceCombinatorNone:
				break;

			case SequenceCombinatorDescendent:
				$(view, enumerateDescendants, (ViewEnumerator) _select, &(Context) {
					.selector = context->selector,
					.sequence = context->sequence + 1,
					.selection = context->selection
				});
				break;

			case SequenceCombinatorChild:
				$(view, enumerateSubviews, (ViewEnumerator) _select, &(Context) {
					.selector = context->selector,
					.sequence = context->sequence + 1,
					.selection = context->selection
				});
				break;

			case SequenceCombinatorSibling:
				$(view, enumerateSiblings, (ViewEnumerator) _select, &(Context) {
					.selector = context->selector,
					.sequence = context->sequence + 1,
					.selection = context->selection
				});
				break;

			case SequenceCombinatorAdjacent:
				$(view, enumerateSiblings, (ViewEnumerator) _select, &(Context) {
					.selector = context->selector,
					.sequence = context->sequence + 1,
					.selection = context->selection
				});
				break;

			case SequenceCombinatorTerminal:
				$(context->selection, addObject, view);
				break;
		}
	}

	return (Array *) context->selection;
}

/**
 * @fn Array *Selector::select(const Selector *self, View *view)
 * @memberof Selector
 */
static Array *select(const Selector *self, View *view) {

	assert(view);

	return _select(view, &(Context) {
		.selector = self,
		.sequence = (const SelectorSequence **) self->sequences->elements,
		.selection = $$(MutableArray, array)
	});
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->hash = hash;
	((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;

	((SelectorInterface *) clazz->def->interface)->initWithRule = initWithRule;
	((SelectorInterface *) clazz->def->interface)->parse = parse;
	((SelectorInterface *) clazz->def->interface)->select = select;

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
