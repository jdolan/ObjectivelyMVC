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
#include <string.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/SelectorSequence.h>
#include <ObjectivelyMVC/View.h>

#define _Class _SelectorSequence

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	SelectorSequence *this = (SelectorSequence *) self;

	free(this->sequence);
	release(this->simpleSelectors);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const SelectorSequence *this = (SelectorSequence *) self;

	return str(this->sequence);
}

#pragma mark - SelectorSequence

/**
 * @fn SelectorSequence *SelectorSequence::initWithSequence(SelectorSequence *self, const char *sequence)
 * @memberof SelectorSequence
 */
static SelectorSequence *initWithSequence(SelectorSequence *self, const char *sequence) {

	self = (SelectorSequence *) super(Object, self, init);
	if (self) {
		self->sequence = strdup(sequence);
		assert(self->sequence);

		self->simpleSelectors = $$(SimpleSelector, parse, sequence);
		assert(self->simpleSelectors->count);
	}

	return self;
}

/**
 * @fn _Bool SelectorSequence::matchesView(const SelectorSequence *self, const View *view)
 * @memberof SelectorSequence
 */
static _Bool matchesView(const SelectorSequence *self, const View *view) {

	for (size_t i = 0; i < self->simpleSelectors->count; i++) {
		const SimpleSelector *simpleSelector = $(self->simpleSelectors, objectAtIndex, i);
		if ($(view, matchesSelector, simpleSelector) == false) {
			return false;
		}
	}

	return true;
}

/**
 * @return The SequenceCombinator for the given character.
 */
static SequenceCombinator sequenceCombinator(const char c) {
	switch (c) {
		case '\0':
			return SequenceCombinatorTerminal;
		case '+':
			return SequenceCombinatorAdjacent;
		case '~':
			return SequenceCombinatorSibling;
		default:
			return SequenceCombinatorDescendent;
	}

	return SequenceCombinatorNone;
}

/**
 * @fn Array *SelectorSequence::parse(const char *rule)
 * @memberof SelectorSequence
 */
static Array *parse(const char *rule) {

	MutableArray *selectorSequences = $$(MutableArray, array);
	assert(selectorSequences);

	const char *c = rule;
	while (*c) {
		const size_t size = strcspn(c, " \n\t>+~");
		if (size) {
			char *s = calloc(1, size + 1);
			assert(s);

			strncpy(s, c, size);

			SelectorSequence *selectorSequence = $(alloc(SelectorSequence), initWithSequence, s);
			assert(selectorSequence);

			selectorSequence->combinator = sequenceCombinator(*(c + size));
			assert(selectorSequence->combinator);

			$(selectorSequences, addObject, selectorSequence);

			release(selectorSequence);
			free(s);
		}

		c += size;
		c += strspn(c, " \n\t>+~");
	}

	return (Array *) selectorSequences;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;

	((SelectorSequenceInterface *) clazz->def->interface)->initWithSequence = initWithSequence;
	((SelectorSequenceInterface *) clazz->def->interface)->matchesView = matchesView;
	((SelectorSequenceInterface *) clazz->def->interface)->parse = parse;
}

/**
 * @fn Class *SelectorSequence::_SelectorSequence(void)
 * @memberof SelectorSequence
 */
Class *_SelectorSequence(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "SelectorSequence";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(SelectorSequence);
		clazz.interfaceOffset = offsetof(SelectorSequence, interface);
		clazz.interfaceSize = sizeof(SelectorSequenceInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
