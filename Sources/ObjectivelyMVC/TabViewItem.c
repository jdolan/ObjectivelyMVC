/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include "TabViewItem.h"

#define _Class _TabViewItem

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TabViewItem *this = (TabViewItem *) self;

  free(this->identifier);

  release(this->label);
  release(this->view);

  super(Object, self, dealloc);
}

#pragma mark - TabViewItem

/**
 * @fn TabViewItem *TabViewItem::initWithIdentifier(TabViewItem *self, const char *identifier)
 * @memberof TabViewItem
 */
static TabViewItem *initWithIdentifier(TabViewItem *self, const char *identifier) {

  self = (TabViewItem *) super(Object, self, init);
  if (self) {
    self->identifier = strdup(identifier);
    assert(self->identifier);

    self->label = $(alloc(Label), initWithText, self->identifier, NULL);
    assert(self->label);

    self->view = $(alloc(View), initWithFrame, NULL);
    assert(self->view);

    $(self, setState, TabStateDefault);
  }

  return self;
}

/**
 * @fn TabViewItem *TabViewItem::initWithView(TabViewItem *self, View *view)
 * @memberof TabViewItem
 */
static TabViewItem *initWithView(TabViewItem *self, View *view) {

  self = $(self, initWithIdentifier, view->identifier ?: classnameof(self));
  if (self) {
    release(self->view);
    self->view = retain(view);
  }

  return self;
}

/**
 * @fn TabViewItem::setState(TabViewItem *self, int state)
 * @memberof TabViewItem
 */
static void setState(TabViewItem *self, int state) {

  self->state = state;

  if (self->state & TabStateSelected) {
    $((View *) self->label, addClassName, "selected");
  } else {
    $((View *) self->label, removeClassName, "selected");
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((TabViewItemInterface *) clazz->interface)->initWithIdentifier = initWithIdentifier;
  ((TabViewItemInterface *) clazz->interface)->initWithView = initWithView;
  ((TabViewItemInterface *) clazz->interface)->setState = setState;
}

/**
 * @fn Class *TabViewItem::_TabViewItem(void)
 * @memberof TabViewItem
 */
Class *_TabViewItem(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TabViewItem",
      .superclass = _Object(),
      .instanceSize = sizeof(TabViewItem),
      .interfaceOffset = offsetof(TabViewItem, interface),
      .interfaceSize = sizeof(TabViewItemInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
