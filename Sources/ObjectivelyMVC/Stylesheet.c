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

#include <ObjectivelyMVC.h>

#include <stylesheet.css.h>

#define _Class _Stylesheet

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Stylesheet *this = (Stylesheet *) self;

	free(this->identifier);

	release(this->selectors);
	release(this->styles);

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Stylesheet *this = (Stylesheet *) self;

	int hash = HASH_SEED;

	hash = HashForCString(hash, this->identifier);
	hash = HashForObject(hash, this->selectors);
	hash = HashForObject(hash, this->styles);

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Stylesheet())) {

		const Stylesheet *this = (Stylesheet *) self;
		const Stylesheet *that = (Stylesheet *) other;

		if (this->identifier && that->identifier) {
			if (strcmp(this->identifier, that->identifier)) {
				return false;
			}
		} else if (this->identifier || that->identifier) {
			return false;
		}

		if ($((Object *) this->selectors, isEqual, (Object *) that->selectors)) {
			return $((Object *) this->styles, isEqual, (Object *) that->styles);
		}
	}

	return false;
}

#pragma mark - Stylesheet

static Stylesheet *_defaultStylesheet;

/**
 * @fn Stylesheet *Stylesheet::defaultStylesheet(void)
 * @memberof Stylesheet
 */
static Stylesheet *defaultStylesheet(void) {
	static Once once;

	do_once(&once, {
		_initialize(_Box());
		_initialize(_Button());
		_initialize(_Checkbox());
		_initialize(_CollectionView());
		_initialize(_CollectionItemView());
		_initialize(_Control());
		_initialize(_HSVColorPicker());
		_initialize(_HueColorPicker());
		_initialize(_ImageView());
		_initialize(_Input());
		_initialize(_Label());
		_initialize(_Option());
		_initialize(_PageView());
		_initialize(_Panel());
		_initialize(_ProgressBar());
		_initialize(_RGBColorPicker());
		_initialize(_ScrollView());
		_initialize(_Select());
		_initialize(_Slider());
		_initialize(_StackView());
		_initialize(_TableCellView());
		_initialize(_TableHeaderView());
		_initialize(_TableHeaderCellView());
		_initialize(_TableRowView());
		_initialize(_TableView());
		_initialize(_TabView());
		_initialize(_Text());
		_initialize(_TextView());

		_defaultStylesheet = $$(Stylesheet, stylesheetWithCharacters, (char *) stylesheet_css);
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

		self->selectors = $$(MutableArray, array);
		assert(self->selectors);

		self->styles = $$(MutableDictionary, dictionary);
		assert(self->styles);

		Array *styles = $$(Style, parse, chars);
		assert(styles);

		for (size_t i = 0; i < styles->count; i++) {
			Style *style = $(styles, objectAtIndex, i);

			for (size_t j = 0; j < style->selectors->count; j++) {
				Selector *selector = $(style->selectors, objectAtIndex, j);

				$(self->selectors, addObject, selector);
				$(self->styles, setObjectForKey, style, selector);
			}

			$(self->selectors, sort, selectorsComparator);
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
		release(string);
	} else {
		self = release(self);
	}

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
	((ObjectInterface *) clazz->def->interface)->hash = hash;
	((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;

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
