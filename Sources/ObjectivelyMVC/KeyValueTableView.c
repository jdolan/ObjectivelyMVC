/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

#include "KeyValueTableView.h"

#define _Class _KeyValueTableView

#pragma mark - View

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  KeyValueTableView *this = (KeyValueTableView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("-key-width", InletTypeInteger, &this->keyWidth, NULL),
    MakeInlet("-value-width", InletTypeInteger, &this->valueWidth, NULL)
  );

  $(self, bind, inlets, (Dictionary *) style->attributes);

  self->needsLayout = true;
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((KeyValueTableView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  KeyValueTableView *this = (KeyValueTableView *) self;

  // Push the table's fixed columns into every row so all rows share one set of
  // column widths. setColumnWidths only writes when a clamp actually changes, so
  // once the rows match this is a no-op -- it cannot churn the layout. Using the
  // fixed -value-width (not a bounds-derived fill) avoids a re-layout feedback
  // loop with the global `TextView { min-width }` floor.
  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {
    KeyValueView *row = $(subviews, objectAtIndex, i);

    if (row->keyWidth != this->keyWidth || row->valueWidth != this->valueWidth) {
      $(row, setColumnWidths, this->keyWidth, this->valueWidth);
    }
  }

  super(View, self, layoutSubviews);
}

#pragma mark - KeyValueTableView

/**
 * @fn KeyValueTableView *KeyValueTableView::initWithFrame(KeyValueTableView *self, const SDL_Rect *frame)
 * @memberof KeyValueTableView
 */
static KeyValueTableView *initWithFrame(KeyValueTableView *self, const SDL_Rect *frame) {

  self = (KeyValueTableView *) super(StackView, self, initWithFrame, frame);
  if (self) {

    $((View *) self, addClassName, "keyValueTable");

    self->stackView.axis = StackViewAxisVertical;

    // A small gap between rows so the table does not feel cramped.
    self->stackView.spacing = 6;

    // Grow in height with the rows; fill the parent's width so the table claims
    // the box. CSS may override either of these via applyStyle.
    ((View *) self)->autoresizingMask = ViewAutoresizingContain | ViewAutoresizingWidth;

    // Default columns so rows have widths before the stylesheet's -key-width /
    // -value-width are applied; CSS overrides these in applyStyle.
    self->keyWidth = 140;
    self->valueWidth = 280;
  }

  return self;
}

/**
 * @fn KeyValueView *KeyValueTableView::addRow(KeyValueTableView *self, View *key, View *value)
 * @memberof KeyValueTableView
 */
static KeyValueView *addRow(KeyValueTableView *self, View *key, View *value) {

  KeyValueView *row = $(alloc(KeyValueView), initWithFrame, NULL);
  assert(row);

  if (key) {
    $(row, setKey, key);
  }

  if (value) {
    $(row, setValue, value);
  }

  $(row, setColumnWidths, self->keyWidth, self->valueWidth);

  $((View *) self, addSubview, (View *) row);
  release(row);

  ((View *) self)->needsLayout = true;

  return row;
}

/**
 * @fn void KeyValueTableView::addRowView(KeyValueTableView *self, KeyValueView *row)
 * @memberof KeyValueTableView
 */
static void addRowView(KeyValueTableView *self, KeyValueView *row) {

  $(row, setColumnWidths, self->keyWidth, self->valueWidth);

  $((View *) self, addSubview, (View *) row);

  ((View *) self)->needsLayout = true;
}

/**
 * @fn void KeyValueTableView::removeAllRows(KeyValueTableView *self)
 * @memberof KeyValueTableView
 */
static void removeAllRows(KeyValueTableView *self) {

  $((View *) self, removeAllSubviews);
  ((View *) self)->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

  ((KeyValueTableViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((KeyValueTableViewInterface *) clazz->interface)->addRow = addRow;
  ((KeyValueTableViewInterface *) clazz->interface)->addRowView = addRowView;
  ((KeyValueTableViewInterface *) clazz->interface)->removeAllRows = removeAllRows;
}

/**
 * @fn Class *KeyValueTableView::_KeyValueTableView(void)
 * @memberof KeyValueTableView
 */
Class *_KeyValueTableView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "KeyValueTableView",
      .superclass = _StackView(),
      .instanceSize = sizeof(KeyValueTableView),
      .interfaceOffset = offsetof(KeyValueTableView, interface),
      .interfaceSize = sizeof(KeyValueTableViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
