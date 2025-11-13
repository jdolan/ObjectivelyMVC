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

#include "TableCellView.h"

#define _Class _TableCellView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TableCellView *this = (TableCellView *) self;

  release(this->text);

  super(Object, self, dealloc);
}

#pragma mark - TableCellView

/**
 * @fn TableCellView *TableCellView::initWithFrame(TableCellView *self, const SDL_Rect *frame)
 * @memberof TableCellView
 */
static TableCellView *initWithFrame(TableCellView *self, const SDL_Rect *frame) {

  self = (TableCellView *) super(View, self, initWithFrame, frame);
  if (self) {
    self->text = $(alloc(Text), initWithText, NULL, NULL);
    assert(self->text);

    $((View *) self, addSubview, (View *) self->text);
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((TableCellViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *TableCellView::_TableCellView(void)
 * @memberof TableCellView
 */
Class *_TableCellView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TableCellView",
      .superclass = _View(),
      .instanceSize = sizeof(TableCellView),
      .interfaceOffset = offsetof(TableCellView, interface),
      .interfaceSize = sizeof(TableCellViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

