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

#include <Objectively.h>

#include <ObjectivelyMVC/Selector.h>
#include <ObjectivelyMVC/Style.h>

#define _Class _Style

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Style *this = (Style *) self;

	release(this->attributes);
	release(this->selectors);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const Style *this = (Style *) self;

	return $(this->selectors, componentsJoinedByCharacters, ", ");
}

#pragma mark - Style

/**
 * @fn void Style::addAttribute(Style *self, const char *attr, ident value)
 * @memberof Style
 */
static void addAttribute(Style *self, const char *attribute, ident value) {

	assert(attribute);
	assert(value);

	$(self->attributes, setObjectForKeyPath, value, attribute);
}

/**
 * @fn void Style::addAttributes(Style *self, const Dictionary *attributes)
 * @memberof Style
 */
static void addAttributes(Style *self, const Dictionary *attributes) {

	assert(attributes);

	$(self->attributes, addEntriesFromDictionary, attributes);
}

/**
 * @fn void Style::addBoolAttribute(Style *self, const char *attr, _Bool value)
 * @memberof Style
 */
static void addBoolAttribute(Style *self, const char *attr, _Bool value) {
	$(self, addAttribute, attr, $$(Boole, valueof, value));
}

/**
 * @fn void Style::addCharactersAttribute(Style *self, const char *attr, const char *value)
 * @memberof Style
 */
static void addCharactersAttribute(Style *self, const char *attr, const char *value) {

	String *string = $$(String, stringWithCharacters, value);

	$(self, addAttribute, attr, string);

	release(string);
}

/**
 * @fn void Style::addColorAttribute(Style *self, const char *attr, const SDL_Color *value)
 * @memberof Style
 */
static void addColorAttribute(Style *self, const char *attr, const SDL_Color *color) {

	Number *r = $$(Number, numberWithValue, color->r);
	Number *g = $$(Number, numberWithValue, color->g);
	Number *b = $$(Number, numberWithValue, color->b);
	Number *a = $$(Number, numberWithValue, color->a);

	Array *array = $$(Array, arrayWithObjects, r, g, b, a, NULL);

	$(self, addAttribute, attr, array);

	release(array);
	release(r);
	release(g);
	release(b);
	release(a);
}

/**
 * @fn void Style::addDoubleAttribute(Style *self, const char *attr, double value)
 * @memberof Style
 */
static void addDoubleAttribute(Style *self, const char *attr, double value) {

	Number *number = $$(Number, numberWithValue, value);

	$(self, addAttribute, attr, number);

	release(number);
}

/**
 * @fn void Style::addEnumAttribute(Style *self, const char *attr, const EnumName *names, int value)
 * @memberof Style
 */
static void addEnumAttribute(Style *self, const char *attr, const EnumName *names, int value) {

	String *string = nameof(names, value);

	$(self, addAttribute, attr, string);

	release(string);
}

/**
 * @fn void Style::addFloatAttribute(Style *self, const char *attr, float value)
 * @memberof Style
 */
static void addFloatAttribute(Style *self, const char *attr, float value) {
	$(self, addDoubleAttribute, attr, value);
}

/**
 * @fn void Style::addIntegerAttribute(Style *self, const char *attr, int value)
 * @memberof Style
 */
static void addIntegerAttribute(Style *self, const char *attr, int value) {
	$(self, addDoubleAttribute, attr, value);
}

/**
 * @fn void Style::addPointAttribute(Style *self, const char *attr, const SDL_Point *value)
 * @memberof Style
 */
static void addPointAttribute(Style *self, const char *attr, const SDL_Point *value) {

	Number *x = $$(Number, numberWithValue, value->x);
	Number *y = $$(Number, numberWithValue, value->y);

	Array *array = $$(Array, arrayWithObjects, x, y, NULL);

	$(self, addAttribute, attr, array);

	release(array);
	release(x);
	release(y);
}

/**
 * @fn void Style::addRectangleAttribute(Style *self, const char *attr, const SDL_Rect *value)
 * @memberof Style
 */
static void addRectangleAttribute(Style *self, const char *attr, const SDL_Rect *value) {

	Number *x = $$(Number, numberWithValue, value->x);
	Number *y = $$(Number, numberWithValue, value->y);
	Number *w = $$(Number, numberWithValue, value->w);
	Number *h = $$(Number, numberWithValue, value->h);

	Array *array = $$(Array, arrayWithObjects, x, y, w, h, NULL);

	$(self, addAttribute, attr, array);

	release(array);
	release(x);
	release(y);
	release(w);
	release(h);
}

/**
 * @fn void Style::addSizeAttribute(Style *self, const char *attr, const SDL_Size *value)
 * @memberof Style
 */
static void addSizeAttribute(Style *self, const char *attr, const SDL_Size *value) {

	Number *w = $$(Number, numberWithValue, value->w);
	Number *h = $$(Number, numberWithValue, value->h);

	Array *array = $$(Array, arrayWithObjects, w, h, NULL);

	$(self, addAttribute, attr, array);

	release(array);
	release(w);
	release(h);
}

/**
 * @fn ident Style::attributeValue(const Style *self, const char *attr)
 * @memberof Style
 */
static ident attributeValue(const Style *self, const char *attr) {
	return $((Dictionary *) self->attributes, objectForKeyPath, attr);
}

/**
 * @fn Style *Style::initWithRules(Style *self, const char *rules)
 * @memberof Style
 */
static Style *initWithRules(Style *self, const char *rules) {

	self = (Style *) super(Object, self, init);
	if (self) {

		self->selectors = $$(Selector, parse, rules);
		assert(self->selectors->count);

		self->attributes = $$(MutableDictionary, dictionary);
		assert(self->attributes);
	}

	return self;
}

/**
 * @fn void Style::removeAttribute(Style *self, const char *attr)
 * @memberof Style
 */
static void removeAttribute(Style *self, const char *attr) {
	$(self->attributes, removeObjectForKeyPath, attr);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;

	((StyleInterface *) clazz->def->interface)->addAttribute = addAttribute;
	((StyleInterface *) clazz->def->interface)->addAttributes = addAttributes;
	((StyleInterface *) clazz->def->interface)->addBoolAttribute = addBoolAttribute;
	((StyleInterface *) clazz->def->interface)->addCharactersAttribute = addCharactersAttribute;
	((StyleInterface *) clazz->def->interface)->addColorAttribute = addColorAttribute;
	((StyleInterface *) clazz->def->interface)->addDoubleAttribute = addDoubleAttribute;
	((StyleInterface *) clazz->def->interface)->addEnumAttribute = addEnumAttribute;
	((StyleInterface *) clazz->def->interface)->addFloatAttribute = addFloatAttribute;
	((StyleInterface *) clazz->def->interface)->addIntegerAttribute = addIntegerAttribute;
	((StyleInterface *) clazz->def->interface)->addPointAttribute = addPointAttribute;
	((StyleInterface *) clazz->def->interface)->addRectangleAttribute = addRectangleAttribute;
	((StyleInterface *) clazz->def->interface)->addSizeAttribute = addSizeAttribute;
	((StyleInterface *) clazz->def->interface)->attributeValue = attributeValue;
	((StyleInterface *) clazz->def->interface)->initWithRules = initWithRules;
	((StyleInterface *) clazz->def->interface)->removeAttribute = removeAttribute;
}

/**
 * @fn Class *Style::_Style(void)
 * @memberof Style
 */
Class *_Style(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Style";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Style);
		clazz.interfaceOffset = offsetof(Style, interface);
		clazz.interfaceSize = sizeof(StyleInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
