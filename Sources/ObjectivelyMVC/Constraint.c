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

#include <Objectively/MutableString.h>
#include <Objectively/Regex.h>

#include <ObjectivelyMVC/Constraint.h>
#include <ObjectivelyMVC/View.h>

/**
 * @return The ConstraintAttribute parsed from `s`.
 */
static ConstraintAttribute constraintAttribute(const char *s) {

	switch (toupper(*s)) {
		case 'W':
			return ConstraintAttributeWidth;
		case 'H':
			return ConstraintAttributeHeight;
		case 'C':
			return ConstraintAttributeCenter;
		case 'T':
			return ConstraintAttributeTop;
		case 'R':
			return ConstraintAttributeRight;
		case 'B':
			return ConstraintAttributeBottom;
		case 'L':
			return ConstraintAttributeLeft;
	}

	return ConstraintAttributeNone;
}

/**
 * @return The ConstraintRelation parsed from `s`.
 */
static ConstraintRelation constraintRelation(const char *s) {

	switch (*s) {
		case '<':
			if (*(s + 1) == '=') {
				return ConstraintRelationLessThanOrEqual;
			}
			return ConstraintRelationLessThan;
		case '>':
			if (*(s + 1) == '=') {
				return ConstraintRelationGreaterThanOrEqual;
			}
			return ConstraintRelationGreaterThan;
		case '=':
			return ConstraintRelationEqual;
	}

	return ConstraintRelationNone;
}

#define _Class _Constraint

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Constraint *this = (Constraint *) self;

	free(this->descriptor);
	free(this->identifier);

	super(Object, self, dealloc);
}

#pragma mark - Constraint

/**
 * @fn void Constraint::apply(const Constraint *self, View *view)
 * @memberof Constraint
 */
static void apply(const Constraint *self, View *view) {

	int value = 0;

	if (self->identifier) {
		const View *source;
		if (strcmp(self->identifier, "superview") == 0) {
			source = view->superview;
		} else {
			source = $(view, ancestorWithIdentifier, self->identifier);
			if (source == NULL) {
				if (view->superview) {
					source = $(view->superview, subviewWithIdentifier, self->identifier);
				}
			}
		}

		assert(source);

		switch (self->source) {
			case ConstraintAttributeNone:
				break;

			case ConstraintAttributeWidth:
				value = source->frame.w;
				break;
			case ConstraintAttributeHeight:
				value = source->frame.h;
				break;

			case ConstraintAttributeTop:
				value = source->frame.y;
				break;
			case ConstraintAttributeMiddle:
				value = source->frame.y + source->frame.h * 0.5;
				break;
			case ConstraintAttributeBottom:
				value = source->frame.y + source->frame.h;
				break;

			case ConstraintAttributeLeft:
				value = source->frame.x;
				break;
			case ConstraintAttributeCenter:
				value = source->frame.x + source->frame.w * 0.5;
				break;
			case ConstraintAttributeRight:
				value = source->frame.x + source->frame.w;
				break;
		}
	}

	value = value * self->multiplier + self->constant;

	switch (self->target) {
		case ConstraintAttributeNone:
			break;

		case ConstraintAttributeWidth:
			$(view, resize, &MakeSize(value, view->frame.h));
			break;
		case ConstraintAttributeHeight:
			$(view, resize, &MakeSize(view->frame.w, value));
			break;

		case ConstraintAttributeTop:
			view->frame.y = value;
			break;
		case ConstraintAttributeMiddle:
			view->frame.y = value - view->frame.h * 0.5;
			break;
		case ConstraintAttributeBottom:
			view->frame.y = value - view->frame.h;
			break;

		case ConstraintAttributeLeft:
			view->frame.x = value;
			break;
		case ConstraintAttributeCenter:
			view->frame.x = value - view->frame.w * 0.5;
			break;
		case ConstraintAttributeRight:
			view->frame.x = value - view->frame.w;
			break;
	}

	view->needsLayout = true;
}

static Regex *_regex;

/**
 * @fn Constraint *Constraint::initWithTarget(Constraint *self, const char *descriptor)
 * @memberof Constraint
 */
static Constraint *initWithDescriptor(Constraint *self, const char *descriptor) {

	self = (Constraint *) super(Object, self, init);
	if (self) {

		self->descriptor = strdup(descriptor);
		assert(self->descriptor);

		self->enabled = true;

		self->multiplier = 1.0f;

		self->priority = DEFAULT_CONSTRAINT_PRIORITY;

		String *string = (String *) mstr(self->descriptor);
		$((MutableString *) string, replaceOccurrencesOfCharacters, " ", "");

		Range *ranges;
		if ($(_regex, matchesString, string, 0, &ranges)) {

			const Range *target = &ranges[1];
			const Range *relation = &ranges[2];
			const Range *identifier = &ranges[3];
			const Range *source = &ranges[4];
			const Range *multiplier = &ranges[5];
			const Range *constant = &ranges[6];
			const Range *priority = &ranges[7];

			assert(target->location != -1);
			assert(relation->location != -1);

			self->target = constraintAttribute(string->chars + target->location);
			assert(self->target);

			self->relation = constraintRelation(string->chars + relation->location);
			assert(self->relation);

			if (identifier->location != -1) {

				self->identifier = calloc(identifier->length + 1, 1);
				assert(self->identifier);

				strncpy(self->identifier, string->chars + identifier->location, identifier->length);

				self->source = constraintAttribute(string->chars + source->location);
				assert(self->source);

				if (multiplier->location != -1) {
					self->multiplier = strtof(string->chars + multiplier->location + 1, NULL);
					assert(self->multiplier);
				}
			}

			if (constant->location != -1) {
				self->constant = strtof(string->chars + constant->location, NULL);
				assert(self->constant);
			}

			if (priority->location != -1) {
				self->priority = (int) strtol(string->chars + priority->location + 1, NULL, 10);
				assert(self->priority);
			}
		} else {
			release(self);
			self = NULL;
		}
		
		release(string);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ConstraintInterface *) clazz->def->interface)->apply = apply;
	((ConstraintInterface *) clazz->def->interface)->initWithDescriptor = initWithDescriptor;

	_regex = rex("^([whtrblc])([<=>]+)([a-z]+)?\\.?([whtrblc])?(\\*[0-9.]*)?([+|-]?[0-9.]+)?(\[[0-9.]+\\])?$", REG_ICASE);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_regex);
}

/**
 * @fn Class *Constraint::_Constraint(void)
 * @memberof Constraint
 */
Class *_Constraint(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Constraint";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Constraint);
		clazz.interfaceOffset = offsetof(Constraint, interface);
		clazz.interfaceSize = sizeof(ConstraintInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
