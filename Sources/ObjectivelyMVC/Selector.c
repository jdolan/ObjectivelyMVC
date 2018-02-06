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

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableSet.h>

#include <ObjectivelyMVC/Log.h>
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
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const Selector *this = (Selector *) self;

	return str(this->rule);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Selector *this = (Selector *) self;

	return HashForCString(HASH_SEED, this->rule);
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

		return strcmp(this->rule, that->rule) == 0;
	}

	return false;
}

#pragma mark - Selector

/**
 * @return The specificity of the given Selector.
 */
static int specificity(const Selector *selector) {

	int specificity = 0;

	for (size_t i = 0; i < selector->sequences->count; i++) {
		SelectorSequence *sequence = $(selector->sequences, objectAtIndex, i);

		for (size_t j = 0; j < sequence->simpleSelectors->count; j++) {
			SimpleSelector *simpleSelector = $(sequence->simpleSelectors, objectAtIndex, j);

			switch (simpleSelector->type) {
				case SimpleSelectorTypeId:
					specificity += 100;
					break;
				case SimpleSelectorTypeClass:
				case SimpleSelectorTypePseudo:
					specificity += 10;
					break;
				case SimpleSelectorTypeType:
					specificity += 1;
					Class *clazz = classForName(simpleSelector->pattern);
					if (clazz) {
						while (clazz) {
							if (clazz == _View()) {
								break;
							}
							specificity += 1;
							clazz = clazz->superclass;
						}
						if (clazz != _View()) {
							MVC_LogError("Class `%s` in Selector `%s` does not extend View\n",
										 simpleSelector->pattern, selector->rule);
						}
					} else {
						MVC_LogWarn("Class `%s` in Selector `%s` not found\n",
									simpleSelector->pattern, selector->rule);
					}

					break;
				default:
					break;
			}
		}
	}

	return specificity;
}

/**
 * @fn Order Selector::compareTo(const Selector *self, const Selector *other)
 * @memberof Selector
 */
static Order compareTo(const Selector *self, const Selector *other) {

	assert(other);

	if (self->specificity < other->specificity) {
		return OrderAscending;
	} else if (self->specificity > other->specificity) {
		return OrderDescending;
	}

	return OrderSame;
}

/**
 * @fn void Selector::enumerateSelection(const Selector *self, View *view, ViewEnumerator enumerator, ident data)
 * @memberof Selector
 */
static void enumerateSelection(const Selector *self, View *view, ViewEnumerator enumerator, ident data) {

	assert(enumerator);

	Set *selection = $(self, select, view);
	assert(selection);

	Array *array = $(selection, allObjects);
	assert(array);

	for (size_t i = 0; i < array->count; i++) {
		enumerator($(array, objectAtIndex, i), data);
	}

	release(array);
	release(selection);
}

/**
 * @fn Selector *Selector::initWithRule(Selector *self, const char *rule)
 * @memberof Selector
 */
static Selector *initWithRule(Selector *self, const char *rule) {

	self = (Selector *) super(Object, self, init);
	if (self) {

		self->rule = strtrim(rule);
		assert(self->rule);

		self->sequences = $$(SelectorSequence, parse, self->rule);
		assert(self->sequences->count);

		self->specificity = specificity(self);
	}

	return self;
}

/**
 * @brief A context for View matching.
 */
typedef struct {
	const Array *sequences;
	size_t sequence;
	_Bool match;
} Match;

/**
 * @brief ViewEnumerator for matchesView.
 */
static _Bool _matchesView(const View *view, Match *match) {

	const SelectorSequence *sequence = $(match->sequences, objectAtIndex, match->sequence);

	if ($(sequence, matchesView, view)) {

		switch (sequence->left) {
			case SequenceCombinatorNone:
				match->match = true;
				break;

			case SequenceCombinatorDescendent:
				match->sequence--;
				$(view, enumerateAncestors, (ViewEnumerator) _matchesView, match);
				break;

			case SequenceCombinatorChild:
				match->sequence--;
				$(view, enumerateSuperview, (ViewEnumerator) _matchesView, match);
				break;

			case SequenceCombinatorSibling:
				match->sequence--;
				$(view, enumerateSiblings, (ViewEnumerator) _matchesView, match);
				break;

			case SequenceCombinatorAdjacent:
				match->sequence--;
				$(view, enumerateAdjacent, (ViewEnumerator) _matchesView, match);
				break;

			case SequenceCombinatorTerminal:
				break;
		}
	}

	return match->match;
}

/**
 * @fn _Bool Selector::matchesView(const Selector *self, View *view)
 * @memberof Selector
 */
static _Bool matchesView(const Selector *self, const View *view) {

	assert(view);

	return _matchesView(view, &(Match) {
		.sequences = self->sequences,
		.sequence = self->sequences->count - 1
	});
}

/**
 * @fn Array *Selector::parse(const char *rules)
 * @memberof Selector
 */
static Array *parse(const char *rules) {

	MutableArray *selectors = $$(MutableArray, arrayWithCapacity, 4);
	assert(selectors);

	if (rules) {

		const char *c = rules;
		while (*c) {
			const size_t size = strcspn(c, ",");
			if (size) {
				char *rule = calloc(1, size + 1);
				assert(rule);

				strncpy(rule, c, size);

				Selector *selector = $(alloc(Selector), initWithRule, rule);
				assert(selector);

				$(selectors, addObject, selector);

				release(selector);
				free(rule);
			}
			c += size;
			c += strspn(c, ", \t\n");
		}
	}

	return (Array *) selectors;
}

/**
 * @brief A context for View selection.
 */
typedef struct {
	const Array *sequences;
	size_t sequence;
	MutableSet *selection;
} Selection;

/**
 * @brief Recursively selects Views by iterating the SelectorSequences in the given Selection.
 */
static Set *__select(View *view, Selection *selection) {

	const SelectorSequence *sequence = $(selection->sequences, objectAtIndex, selection->sequence);
	
	if ($(sequence, matchesView, view)) {

		switch (sequence->right) {
			case SequenceCombinatorNone:
				break;

			case SequenceCombinatorDescendent:
				selection->sequence++;
				$(view, enumerateDescendants, (ViewEnumerator) __select, selection);
				break;

			case SequenceCombinatorChild:
				selection->sequence++;
				$(view, enumerateSubviews, (ViewEnumerator) __select, selection);
				break;

			case SequenceCombinatorSibling:
				selection->sequence++;
				$(view, enumerateSiblings, (ViewEnumerator) __select, selection);
				break;

			case SequenceCombinatorAdjacent:
				selection->sequence++;
				$(view, enumerateAdjacent, (ViewEnumerator) __select, selection);
				break;

			case SequenceCombinatorTerminal:
				$(selection->selection, addObject, view);
				break;
		}
	}

	$(view, enumerateSubviews, (ViewEnumerator) __select, selection);

	return (Set *) selection->selection;
}


/**
 * @fn Array *Selector::select(const Selector *self, View *view)
 * @memberof Selector
 */
static Set *_select(const Selector *self, View *view) {

	assert(view);

	return __select(view, &(Selection) {
		.sequences = self->sequences,
		.selection = $$(MutableSet, set)
	});
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;
	((ObjectInterface *) clazz->def->interface)->hash = hash;
	((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;

	((SelectorInterface *) clazz->def->interface)->enumerateSelection = enumerateSelection;
	((SelectorInterface *) clazz->def->interface)->compareTo = compareTo;
	((SelectorInterface *) clazz->def->interface)->initWithRule = initWithRule;
	((SelectorInterface *) clazz->def->interface)->matchesView = matchesView;
	((SelectorInterface *) clazz->def->interface)->parse = parse;
	((SelectorInterface *) clazz->def->interface)->select = _select;
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
