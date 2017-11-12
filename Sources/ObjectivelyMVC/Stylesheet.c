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

		assert(style);

		if ($(style, attributeValue, "debug")) {
			SDL_TriggerBreakpoint();
		}

		$(selector, enumerateSelection, view, apply_enumerateSelection, (ident) style);
	}
}

static Stylesheet *_defaultStylesheet;

/**
 * @fn Stylesheet *Stylesheet::defaultStylesheet(void)
 * @memberof Stylesheet
 */
static Stylesheet *defaultStylesheet(void) {
	static Once once;

	do_once(&once, {
		_defaultStylesheet = $$(Stylesheet, stylesheetWithCharacters, "\
				Box { \
					borderColor: #dededeaa; \
					borderWidth: 1; \
					padding: 10 10 10 10; \
				} \
				Box > Label { \
					backgroundColor: #88888844; \
					padding: 0 8 0 8; \
					x: 20; \
				} \
				Box .content { \
					spacing: 4; \
				} \
				Button { \
					backgroundColor: #88888822; \
					padding: 8 8 8 8; \
				} \
				Checkbox > Control { \
					backgroundColor: #22222266; \
					frame: 0 0 18 18; \
					padding: 4 4 4 4; \
				} \
				CollectionItemView { \
					backgroundColor: #22222266; \
					borderColor: #999999aa; \
				} \
				CollectionItemView > .selectionOverlay { \
					backgroundColor: #ffffff22; \
					hidden: true; \
				} \
				CollectionItemView:selected > .selectionOverlay { \
					hidden: false; \
				} \
				CollectionView { \
					backgroundColor: #22222266; \
					itemSize: 64 64; \
					itemSpacing: 10 10; \
				} \
				CollectionView .content { \
					padding: 10 10 10 10; \
				} \
				Panel { \
					backgroundColor: #444444dd; \
					borderColor: #dedede; \
					borderWidth: 1; \
					padding: 12 12 12 12; \
				} \
				Slider { \
					height: 32; \
					padding: 8 8 8 8; \
				} \
		");

		assert(_defaultStylesheet);
	});

	return _defaultStylesheet;
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
		self = $(self, initWithString, string);
	} else {
		self = release(self);
	}
	release(string);

	return self;
}

/**
 * @fn Stylesheet *Stylesheet::initWithString(Stylsheet *self, const String *string)
 * @memberof Stylesheet
 */
static Stylesheet *initWithString(Stylesheet *self, const String *string) {

	assert(string);

	return $(self, initWithCharacters, string->chars);
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

/**
 * @fn Stylesheet *Stylesheet::stylesheetWithString(const String *string)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithString(const String *string) {
	return $(alloc(Stylesheet), initWithString, string);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((StylesheetInterface *) clazz->def->interface)->apply = apply;
	((StylesheetInterface *) clazz->def->interface)->defaultStylesheet = defaultStylesheet;
	((StylesheetInterface *) clazz->def->interface)->initWithCharacters = initWithCharacters;
	((StylesheetInterface *) clazz->def->interface)->initWithContentsOfFile = initWithContentsOfFile;
	((StylesheetInterface *) clazz->def->interface)->initWithString = initWithString;
	((StylesheetInterface *) clazz->def->interface)->stylesheetWithCharacters = stylesheetWithCharacters;
	((StylesheetInterface *) clazz->def->interface)->stylesheetWithContentsOfFile = stylesheetWithContentsOfFile;
	((StylesheetInterface *) clazz->def->interface)->stylesheetWithString = stylesheetWithString;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_defaultStylesheet);
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
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
