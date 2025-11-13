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

#include <Objectively/MutableString.h>

#include "DebugViewController.h"
#include "WindowController.h"

#include "debug.css.h"
#include "debug.json.h"

#define _Class _DebugViewController

#pragma mark - Selectors

/**
 * @see TableViewDataSource::numberOfRows(const TableView *)
 */
static size_t selectors_numberOfRows(const TableView *tableView) {

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {
    return this->debug->computedStyle->selectors->count;
  } else {
    return 0;
  }
}

/**
 * @see TableViewDataSource::valueForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static ident selectors_valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {
    return $(this->debug->computedStyle->selectors, objectAtIndex, row);
  } else {
    return NULL;
  }
}

/**
 * @see TableViewDelegate::cellForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static TableCellView *selectors_cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  const Selector *selector = tableView->dataSource.valueForColumnAndRow(tableView, column, row);
  assert(selector);

  TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
  $(cell->text, setText, selector->rule);

  return cell;
}

#pragma mark - Computed Style

/**
 * @see TableViewDataSource::numberOfRows(const TableView *)
 */
static size_t computedStyle_numberOfRows(const TableView *tableView) {

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {
    return this->debug->computedStyle->attributes->count;
  } else {
    return 0;
  }
}

/**
 * @see TableViewDataSource::valueForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static ident computedStyle_valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  ident value = NULL;

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {

    Array *array;
    if (strcmp("Attribute", column->identifier) == 0) {
      array = $(this->debug->computedStyle->attributes, allKeys);
    } else {
      array = $(this->debug->computedStyle->attributes, allObjects);
    }

    value = $(array, objectAtIndex, row);
    release(array);
  }

  return value;
}

/**
 * @see TableViewDelegate::cellForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static TableCellView *computedStyle_cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  ident value = tableView->dataSource.valueForColumnAndRow(tableView, column, row);
  assert(value);

  TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
  String *description = $((Object *) value, description);
  $(cell->text, setText, description->chars);
  release(description);

  return cell;
}

#pragma mark - Warnings

/**
 * @see TableViewDataSource::numberOfRows(const TableView *)
 */
static size_t warnings_numberOfRows(const TableView *tableView) {

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {
    return this->debug->warnings->array.count;
  } else {
    return 0;
  }
}

/**
 * @see TableViewDataSource::valueForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static ident warnings_valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  ident value = NULL;

  const DebugViewController *this = tableView->dataSource.self;
  if (this->debug) {
    value = $((Array *) this->debug->warnings, objectAtIndex, row);
  }

  return value;
}

/**
 * @see TableViewDelegate::cellForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static TableCellView *warnings_cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

  Warning *warning = tableView->dataSource.valueForColumnAndRow(tableView, column, row);
  assert(warning);

  TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
  $(cell->text, setText, warning->message->chars);

  return cell;
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

  super(ViewController, self, loadView);

  DebugViewController *this = (DebugViewController *) self;

  Outlet outlets[] = MakeOutlets(
    MakeOutlet("statistics", &this->statistics),
    MakeOutlet("description", &this->description),
    MakeOutlet("path", &this->path),
    MakeOutlet("selectors", &this->selectors),
    MakeOutlet("computedStyle", &this->computedStyle),
    MakeOutlet("warnings", &this->warnings)
  );

  View *view = $$(View, viewWithCharacters, (char *) debug_json, outlets);
  view->stylesheet = $$(Stylesheet, stylesheetWithCharacters, (char *) debug_css);

  $(self, setView, view);
  release(view);

  this->selectors->dataSource.self = self;
  this->selectors->dataSource.numberOfRows = selectors_numberOfRows;
  this->selectors->dataSource.valueForColumnAndRow = selectors_valueForColumnAndRow;
  this->selectors->delegate.self = self;
  this->selectors->delegate.cellForColumnAndRow = selectors_cellForColumnAndRow;

  this->computedStyle->dataSource.self = self;
  this->computedStyle->dataSource.numberOfRows = computedStyle_numberOfRows;
  this->computedStyle->dataSource.valueForColumnAndRow = computedStyle_valueForColumnAndRow;
  this->computedStyle->delegate.self = self;
  this->computedStyle->delegate.cellForColumnAndRow = computedStyle_cellForColumnAndRow;

  this->warnings->dataSource.self = self;
  this->warnings->dataSource.numberOfRows = warnings_numberOfRows;
  this->warnings->dataSource.valueForColumnAndRow = warnings_valueForColumnAndRow;
  this->warnings->delegate.self = self;
  this->warnings->delegate.cellForColumnAndRow = warnings_cellForColumnAndRow;
}

#pragma mark - DebugViewController

/**
 * @brief
 */
static void debugEnumerate(View *view, ident data) {

  DebugViewController *this = data;

  if (view == this->debug) {
    const SDL_Rect frame = $(view, renderFrame);

    $(this->renderer, setClippingFrame, NULL);
    $(this->renderer, setDrawColor, &MakeColor(0x22, 0x66, 0x99, 0x88));
    $(this->renderer, drawRectFilled, &frame);
    $(this->renderer, setDrawColor, &Colors.White);
  }

  if ($(view, isVisible)) {
    this->visibleViews++;

    // FIXME: This is a bit gaff. But hasOverflow produces false positives if called at the
    // FIXME: end of layoutSubviews, because containers have not yet been resized to fill their
    // FIXME: parent (e.g. autoresizing-mask: contain | width).
    // FIXME: Ideally, warnings are accumulated during layoutSubviews and applyTheme, instead
    // FIXME: of attempting to collect them here.

    $(view, clearWarnings, 0xff);
    $(view, hasOverflow);

    const Array *warnings = (Array *) view->warnings;
    if (warnings->count) {
      const SDL_Rect frame = $(view, renderFrame);

      $(this->renderer, setClippingFrame, NULL);
      $(this->renderer, setDrawColor, &Colors.DarkGoldenRod);
      $(this->renderer, drawRect, &frame);
      $(this->renderer, setDrawColor, &Colors.White);
    }
  }
}

/**
 * @fn void DebugViewController::debug(DebugViewController *self, const View *view, Renderer *renderer)
 * @memberof DebugViewController
 */
static void debug(DebugViewController *self, const View *view, Renderer *renderer) {

  if (view != self->debug) {
    self->debug = view;

    if (self->debug) {
      self->root = self->debug;
      while (self->root->superview) {
        self->root = self->root->superview;
      }

      String *path = $(self->debug, path);
      $(self->path, setText, path->chars);
      release(path);

      String *description = $((Object *) view, description);
      $(self->description, setText, description->chars);
      release(description);
    } else {
      $(self->path, setText, NULL);
      $(self->description, setText, NULL);
    }

    $(self->selectors, reloadData);
    $((View *) self->selectors, sizeToFit);

    $(self->computedStyle, reloadData);
    $((View *) self->computedStyle, sizeToFit);

    $(self->warnings, reloadData);
    $((View *) self->warnings, sizeToFit);
  }

  self->renderer = renderer;
  assert(self->renderer);

  self->visibleViews = 0;

  if (self->root) {
    $((View *) self->root, enumerate, debugEnumerate, self);
  }

  self->frames++;
  if (SDL_GetTicks() - self->timestamp >= 1000) {

    $(self->statistics, setTextWithFormat, "%d views, %dfps", self->visibleViews, self->frames);

    self->timestamp = SDL_GetTicks();
    self->frames = 0;
  }
}

/**
 * @fn DebugViewController *DebugViewController::init(DebugViewController *self)
 * @memberof DebugViewController
 */
static DebugViewController *init(DebugViewController *self) {
  return (DebugViewController *) super(ViewController, self, init);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ViewControllerInterface *) clazz->interface)->loadView = loadView;

  ((DebugViewControllerInterface *) clazz->interface)->debug = debug;
  ((DebugViewControllerInterface *) clazz->interface)->init = init;
}

/**
 * @fn Class *DebugViewController::_DebugViewController(void)
 * @memberof DebugViewController
 */
Class *_DebugViewController(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "DebugViewController",
      .superclass = _ViewController(),
      .instanceSize = sizeof(DebugViewController),
      .interfaceOffset = offsetof(DebugViewController, interface),
      .interfaceSize = sizeof(DebugViewControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
