/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include "ObjectivelyMVC.h"

#include "stylesheet.css.h"

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

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  Stylesheet *this = (Stylesheet *) self;

  int hash = HASH_SEED;

  hash = HashForObject(hash, this->selectors);
  hash = HashForObject(hash, this->styles);

  return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Stylesheet())) {

    const Stylesheet *this = (Stylesheet *) self;
    const Stylesheet *that = (Stylesheet *) other;

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
    _defaultStylesheet = $$(Stylesheet, stylesheetWithCharacters, (char *) stylesheet_css);
    assert(_defaultStylesheet);
  });

  return _defaultStylesheet;
}

static ident selectorsReducer(const ident obj, ident accumulator, ident data) {

  $((MutableArray *) accumulator, addObjectsFromArray, ((Style *) obj)->selectors);

  return accumulator;
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

    self->styles = $$(Style, parse, chars);
    assert(self->styles);

    MutableArray *selectors = $$(MutableArray, array);
    assert(selectors);

    $(self->styles, reduce, selectorsReducer, selectors, NULL);

    self->selectors = $((Array *) selectors, sortedArray, selectorsComparator);
    assert(self->selectors);

    release(selectors);
  }

  return self;
}

/**
 * @fn Stylesheet *Stylesheet::initWithData(Stylesheet *self, const Data *data)
 * @memberof Stylesheet
 */
static Stylesheet *initWithData(Stylesheet *self, const Data *data) {

  assert(data);

  String *string = $$(String, stringWithData, data, STRING_ENCODING_UTF8);
  assert(string);

  self = $(self, initWithString, string);

  release(string);
  return self;
}

/**
 * @fn Stylesheet *Stylesheet::initWithResource(Stylsheet *self, const Resource *resource)
 * @memberof Stylesheet
 */
static Stylesheet *initWithResource(Stylesheet *self, const Resource *resource) {

  assert(resource);

  return $(self, initWithData, resource->data);
}

/**
 * @fn Stylesheet *Stylesheet::initWithResourceName(Stylsheet *self, const char *name)
 * @memberof Stylesheet
 */
static Stylesheet *initWithResourceName(Stylesheet *self, const char *name) {

  assert(name);

  Resource *resource = $$(Resource, resourceWithName, name);
  assert(resource);

  self = $(self, initWithResource, resource);

  release(resource);
  return self;
}

/**
 * @fn Stylesheet *Stylesheet::initWithString(Stylesheet *self, const String *string)
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
 * @fn Stylesheet *Stylesheet::stylesheetWithCharacters(const Data *data)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithData(const Data *data) {
  return $(alloc(Stylesheet), initWithData, data);
}

/**
 * @fn Stylesheet *Stylesheet::stylesheetWithResource(const Resource *resource)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithResource(const Resource *resource) {
  return $(alloc(Stylesheet), initWithResource, resource);
}

/**
 * @fn Stylesheet *Stylesheet::stylesheetWithResourceName(const char *name)
 * @memberof Stylesheet
 */
static Stylesheet *stylesheetWithResourceName(const char *name) {
  return $(alloc(Stylesheet), initWithResourceName, name);
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

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((StylesheetInterface *) clazz->interface)->defaultStylesheet = defaultStylesheet;
  ((StylesheetInterface *) clazz->interface)->initWithCharacters = initWithCharacters;
  ((StylesheetInterface *) clazz->interface)->initWithData = initWithData;
  ((StylesheetInterface *) clazz->interface)->initWithResource = initWithResource;
  ((StylesheetInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
  ((StylesheetInterface *) clazz->interface)->initWithString = initWithString;
  ((StylesheetInterface *) clazz->interface)->stylesheetWithCharacters = stylesheetWithCharacters;
  ((StylesheetInterface *) clazz->interface)->stylesheetWithData = stylesheetWithData;
  ((StylesheetInterface *) clazz->interface)->stylesheetWithResource = stylesheetWithResource;
  ((StylesheetInterface *) clazz->interface)->stylesheetWithResourceName = stylesheetWithResourceName;
  ((StylesheetInterface *) clazz->interface)->stylesheetWithString = stylesheetWithString;
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
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Stylesheet",
      .superclass = _Object(),
      .instanceSize = sizeof(Stylesheet),
      .interfaceOffset = offsetof(Stylesheet, interface),
      .interfaceSize = sizeof(StylesheetInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
