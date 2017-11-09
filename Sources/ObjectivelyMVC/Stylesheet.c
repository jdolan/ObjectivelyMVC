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

#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/Stylesheet.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Stylesheet

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Stylesheet *this = (Stylesheet *) self;

	release(this->selectors);
	release(this->styles);

	super(Object, self, dealloc);
}

#pragma mark - Stylesheet

/**
 * @brief ViewEnumerator for apply.
 */
static void apply_enumerateSelection(View *view, ident data) {

	if (MVC_LogEnabled(SDL_LOG_PRIORITY_VERBOSE)) {

		String *this = $((Object *) view, description);
		String *that = $((Object *) data, description);

		MVC_LogVerbose("%s -> %s\n", that->chars, this->chars);

		release(this);
		release(that);
	}

	$(view, applyStyle, data);
}

/**
 * @fn void Stylesheet::apply(const Stylesheet *self, View *view)
 * @memberof Stylesheet
 */
static void apply(const Stylesheet *self, View *view) {

	assert(view);

	for (size_t i = 0; i < self->selectors->count; i++) {

		const Selector *selector = $(self->selectors, objectAtIndex, i);
		const Style *style = $(self->styles, objectForKey, (ident) selector);

		$(selector, enumerateSelection, view, apply_enumerateSelection, (ident) style);
	}
}

/**
 * @brief Comparator for Selector sorting.
 */
static Order selectorsComparator(const ident a, const ident b) {
	return $((Selector *) a, compareTo, (Selector *) b);
}

/**
 * @fn Stylesheet *Stylesheet::initWithCharacters(Stylesheet *self, const char *chars)
 * @memberof Stylesheet
 */
static Stylesheet *initWithCharacters(Stylesheet *self, const char *chars) {

	self = (Stylesheet *) super(Object, self, init);
	if (self) {

		self->selectors = (Array *) $$(MutableArray, array);
		assert(self->selectors);

		self->styles = (Dictionary *) $$(MutableDictionary, dictionary);
		assert(self->styles);

		Array *styles = $$(Style, parse, chars);
		assert(styles);

		for (size_t i = 0; i < styles->count; i++) {
			Style *style = $(styles, objectAtIndex, i);

			for (size_t j = 0; j < style->selectors->count; j++) {
				Selector *selector = $(style->selectors, objectAtIndex, j);

				$((MutableArray *) self->selectors, addObject, selector);
				$((MutableDictionary *) self->styles, setObjectForKey, style, selector);
			}

			$((MutableArray *) self->selectors, sort, selectorsComparator);
		}

		release(styles);
	}

	return self;
}

/**
 * @fn Stylesheet *Stylesheet::initWithContentsOfFile(Stylesheet *self, const char *path, StringEncoding encoding)
 * @memberof Stylesheet
 */
static Stylesheet *initWithContentsOfFile(Stylesheet *self, const char *path, StringEncoding encoding) {

	String *string = $$(String, stringWithContentsOfFile, path, encoding);
	if (string) {
		self = $(self, initWithCharacters, string->chars);
	} else {
		self = $(self, initWithCharacters, NULL);
	}
	release(string);

	return self;
}

/**
 * @fn Stylesheet *Stylesheet::stylesheetWithCharacters(const char *chars)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithCharacters(const char *chars) {
	return $(alloc(Stylesheet), initWithCharacters, chars);
}

/**
 * @fn Stylesheet *Stylesheet::stylesheetWithContentsOfFile(const char *path)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithContentsOfFile(const char *path, StringEncoding encoding) {
	return $(alloc(Stylesheet), initWithContentsOfFile, path, encoding);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((StylesheetInterface *) clazz->def->interface)->apply = apply;
	((StylesheetInterface *) clazz->def->interface)->initWithCharacters = initWithCharacters;
	((StylesheetInterface *) clazz->def->interface)->initWithContentsOfFile = initWithContentsOfFile;
	((StylesheetInterface *) clazz->def->interface)->stylesheetWithCharacters = stylesheetWithCharacters;
	((StylesheetInterface *) clazz->def->interface)->stylesheetWithContentsOfFile = stylesheetWithContentsOfFile;

}

/**
 * @fn Class *Stylesheet::_Stylesheet(void)
 * @memberof Stylesheet
 */
Class *_Stylesheet(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Stylesheet";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Stylesheet);
		clazz.interfaceOffset = offsetof(Stylesheet, interface);
		clazz.interfaceSize = sizeof(StylesheetInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
