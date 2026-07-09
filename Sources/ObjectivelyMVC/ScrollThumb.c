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

#include "ScrollThumb.h"

#define _Class _ScrollThumb

#pragma mark - View

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((ScrollThumb *) self, initWithFrame, NULL);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  ScrollThumb *this = (ScrollThumb *) self;

  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if ($((View *) self, didReceiveEvent, event)) {
      self->state |= ControlStateHighlighted;
      return true;
    }
  } else if (event->type == SDL_EVENT_MOUSE_MOTION) {
    if (self->state & ControlStateHighlighted) {
      if (this->delegate.didDrag) {
        this->delegate.didDrag(this, (int) event->motion.yrel);
      }
      return true;
    }
  } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    if (self->state & ControlStateHighlighted) {
      self->state &= ~ControlStateHighlighted;
      return true;
    }
  }

  return super(Control, self, captureEvent, event);
}

#pragma mark - ScrollThumb

/**
 * @fn ScrollThumb *ScrollThumb::initWithFrame(ScrollThumb *self, const SDL_Rect *frame)
 * @memberof ScrollThumb
 */
static ScrollThumb *initWithFrame(ScrollThumb *self, const SDL_Rect *frame) {

  self = (ScrollThumb *) super(Control, self, initWithFrame, frame);
  if (self) {
    $((View *) self, addClassName, "thumb");
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ViewInterface *) clazz->interface)->init = init;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((ScrollThumbInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *ScrollThumb::_ScrollThumb(void)
 * @memberof ScrollThumb
 */
Class *_ScrollThumb(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "ScrollThumb",
      .superclass = _Control(),
      .instanceSize = sizeof(ScrollThumb),
      .interfaceOffset = offsetof(ScrollThumb, interface),
      .interfaceSize = sizeof(ScrollThumbInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
