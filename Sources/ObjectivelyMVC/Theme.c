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

#include <Objectively/JSONSerialization.h>

#include <ObjectivelyMVC/View.h>

#define _Class _Theme

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Theme *this = (Theme *) self;

	release(this->selectors);
	release(this->styles);

	super(Object, self, dealloc);
}

#pragma mark - Theme

/**
 * @brief Comparator for Selector sorting.
 */
static Order addStyle_selectorsComparator(const ident a, const ident b) {
	return $((Selector *) a, compareTo, (Selector *) b);
}

/**
 * @fn void Theme::addStyle(Theme *self, Style *style)
 * @memberof Theme
 */
static void addStyle(Theme *self, Style *style) {

	assert(style);

	const Array *selectors = (Array *) style->selectors;
	for (size_t i = 0; i < selectors->count; i++) {

		Selector *selector = $(selectors, objectAtIndex, i);

		$(self->selectors, addObject, selector);
		$(self->styles, setObjectForKey, style, selector);
	}

	$(self->selectors, sort, addStyle_selectorsComparator);
}

/**
 * @brief DictionaryEnumerator for addStylesheet.
 */
static void addStylesheet_enumerator(const Dictionary *dictionary, ident obj, ident key, ident data) {

	const String *rules = cast(String, key);
	const Dictionary *attributes = cast(Dictionary, obj);

	Style *style = $(alloc(Style), initWithRules, rules->chars);
	assert(style);

	$(style, addAttributes, attributes);

	$((Theme *) data, addStyle, style);

	release(style);
}

/**
 * @fn void Theme::addStylesheet(Theme *self, const Dictionary *stylesheet)
 * @memberof Theme
 */
static void addStylesheet(Theme *self, const Dictionary *stylesheet) {

	if (stylesheet) {
		$(stylesheet, enumerateObjectsAndKeys, addStylesheet_enumerator, self);
	}
}

/**
 * @brief ViewEnumerator for apply.
 */
static void apply_enumerateSelection(View *view, ident data) {
	$(view, applyStyle, data);
}

/**
 * @fn void Theme::apply(const Theme *self, View *view)
 * @memberof Theme
 */
static void apply(const Theme *self, View *view) {

	const Array *selectors = (Array *) self->selectors;
	const Dictionary *styles = (Dictionary *) self->styles;

	for (size_t i = 0; i < selectors->count; i++) {

		const Selector *selector = $(selectors, objectAtIndex, i);
		const Style *style = $(styles, objectForKey, (ident) selector);

		$(selector, enumerateSelection, view, apply_enumerateSelection, (ident) style);
	}
}

/**
 * @fn Theme *Theme::init(Theme *self)
 * @memberof Theme
 */
static Theme *init(Theme *self) {

	self = (Theme *) super(Object, self, init);
	if (self) {
		self->selectors = $$(MutableArray, array);
		assert(self->selectors);

		self->styles = $$(MutableDictionary, dictionary);
		assert(self->styles);
	}

	return self;
}

/**
 * @fn void Theme::removeStyle(Theme *self, Style *style)
 * @memberof Theme
 */
static void removeStyle(Theme *self, Style *style) {

	assert(style);

	const Array *selectors = (Array *) style->selectors;
	for (size_t i = 0; i < selectors->count; i++) {

		Selector *selector = $(selectors, objectAtIndex, i);

		$(self->selectors, removeObject, selector);
		$(self->styles, removeObjectForKey, selector);
	}
}

/**
 * @fn Theme *Theme::themeWithContentsOfFile(const char *path)
 * @memberof Theme
 */
static Theme *themeWithContentsOfFile(const char *path) {

	Data *data = $$(Data, dataWithContentsOfFile, path);

	Theme *theme = $$(Theme, themeWithData, data);

	release(data);

	return theme;
}

/**
 * @fn Theme *Theme::themeWithData(const Data *data)
 * @memberof Theme
 */
static Theme *themeWithData(const Data *data) {

	Dictionary *dictionary = $$(JSONSerialization, objectFromData, data, 0);

	Theme *theme = $$(Theme, themeWithDictionary, dictionary);

	release(dictionary);

	return theme;
}

/**
 * @fn Theme *Theme::themeWithDictionary(const Dictionary *dictionary)
 * @memberof Theme
 */
static Theme *themeWithDictionary(const Dictionary *dictionary) {

	Theme *theme = $(alloc(Theme), init);
	assert(theme);

	$(theme, addStylesheet, dictionary);
	return theme;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ThemeInterface *) clazz->def->interface)->addStyle = addStyle;
	((ThemeInterface *) clazz->def->interface)->addStylesheet = addStylesheet;
	((ThemeInterface *) clazz->def->interface)->apply = apply;
	((ThemeInterface *) clazz->def->interface)->init = init;
	((ThemeInterface *) clazz->def->interface)->removeStyle = removeStyle;
	((ThemeInterface *) clazz->def->interface)->themeWithContentsOfFile = themeWithContentsOfFile;
	((ThemeInterface *) clazz->def->interface)->themeWithData = themeWithData;
	((ThemeInterface *) clazz->def->interface)->themeWithDictionary = themeWithDictionary;
}

/**
 * @fn Class *Theme::_Theme(void)
 * @memberof Theme
 */
Class *_Theme(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Theme";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Theme);
		clazz.interfaceOffset = offsetof(Theme, interface);
		clazz.interfaceSize = sizeof(ThemeInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
