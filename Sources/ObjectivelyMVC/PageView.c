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

#include "PageView.h"

#define _Class _PageView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  PageView *this = (PageView *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::addSubview(View *, View *)
 */
static void addSubview(View *self, View *subview) {

  super(View, self, addSubview, subview);

  PageView *this = (PageView *) self;

  subview->hidden = true;

  if (this->currentPage == NULL) {
    $(this, setCurrentPage, subview);
  }
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((PageView *) self, initWithFrame, NULL);
}

/**
 * @see View::removeSubview(View *, View *)
 */
static void removeSubview(View *self, View *subview) {

  PageView *this = (PageView *) self;

  retain(subview);

  super(View, self, removeSubview, subview);

  subview->hidden = false;

  if (subview == this->currentPage) {
    $(this, setCurrentPage, NULL);
  }

  release(subview);
}

/**
 * @see View::visibleSubviews(const View *)
 */
static Array *visibleSubviews(const View *self) {
  return $$(Array, arrayWithArray, (Array *) self->subviews);
}

#pragma mark - PageView

/**
 * @fn PageView *PageView::initWithFrame(PageView *self, const SDL_Rect *frame)
 * @memberof PageView
 */
static PageView *initWithFrame(PageView *self, const SDL_Rect *frame) {

  self = (PageView *) super(View, self, initWithFrame, frame);
  if (self) {
    $((View *) self, addClassName, "container");
  }

  return self;
}

/**
 * @brief ArrayEnumerator for setCurrentPage.
 */
static void setCurrentPage_enumerate(const Array *array, ident obj, ident data) {

  View *subview = obj;

  if (subview == ((PageView *) data)->currentPage) {
    subview->hidden = false;
  } else {
    subview->hidden = true;
  }
}

/**
 * @fn void PageView::setCurrentPage(PageView *self, View *currentPage)
 * @memberof PageView
 */
static void setCurrentPage(PageView *self, View *currentPage) {

  if (currentPage != self->currentPage) {

    const Array *subviews = (Array *) self->view.subviews;

    if (currentPage) {
      self->currentPage = currentPage;
    } else {
      self->currentPage = $(subviews, firstObject);
    }

    $(subviews, enumerateObjects, setCurrentPage_enumerate, self);

    if (self->currentPage) {

      if (self->delegate.didSetCurrentPage) {
        self->delegate.didSetCurrentPage(self);
      }
    }

    self->view.needsLayout = true;
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->addSubview = addSubview;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->removeSubview = removeSubview;
  ((ViewInterface *) clazz->interface)->visibleSubviews = visibleSubviews;

  ((PageViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((PageViewInterface *) clazz->interface)->setCurrentPage = setCurrentPage;
}

/**
 * @fn Class *PageView::_PageView(void)
 * @memberof PageView
 */
Class *_PageView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "PageView",
      .superclass = _View(),
      .instanceSize = sizeof(PageView),
      .interfaceOffset = offsetof(PageView, interface),
      .interfaceSize = sizeof(PageViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
