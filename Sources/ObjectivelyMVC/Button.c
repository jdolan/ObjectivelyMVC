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

#include "Button.h"

#define _Class _Button

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Button *this = (Button *) self;

  release(this->image);
  release(this->title);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  Button *this = (Button *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("title", InletTypeView, &this->title, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Button *) self, initWithFrame, NULL);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  Button *this = (Button *) self;

  View *view = (View *) self;

  if (event->type == SDL_MOUSEBUTTONDOWN) {
    self->state |= ControlStateHighlighted;
    return true;
  }

  if (event->type == SDL_MOUSEBUTTONUP) {
    self->state &= ~ControlStateHighlighted;
    if (event->button.clicks) {
      if (this->delegate.didClick) {
        this->delegate.didClick(this);
      }
      $(view, emitViewEvent, ViewEventClick, NULL);
      return true;
    }
  }

  if (event->type == SDL_MOUSEMOTION) {
    if (self->state & ControlStateHighlighted) {
      return true;
    }
  }

  return super(Control, self, captureEvent, event);
}

#pragma mark - Button

/**
 * @fn Button *Button::initWithFrame(Button *self, const SDL_Rect *frame)
 * @memberof Button
 */
static Button *initWithFrame(Button *self, const SDL_Rect *frame) {

  self = (Button *) super(Control, self, initWithFrame, frame);
  if (self) {

    self->image = $(alloc(ImageView), initWithFrame, frame);
    assert(self->image);

    $((View *) self, addSubview, (View *) self->image);

    self->title = $(alloc(Text), initWithText, NULL, NULL);
    assert(self->title);

    $((View *) self, addSubview, (View *) self->title);
  }

  return self;
}

/**
 * @fn Button *Button::initWithImage(Button *self, Image *image)
 * @memberof Button
 */
static Button *initWithImage(Button *self, Image *image) {

  self = $(self, initWithFrame, NULL);
  if (self) {
    $(self->image, setImage, image);
  }
  return self;
}

/**
 * @fn Button *Button::initWithFrame(Button *self, const char *title)
 * @memberof Button
 */
static Button *initWithTitle(Button *self, const char *title) {

  self = $(self, initWithFrame, NULL);
  if (self) {
    $(self->title, setText, title);
  }
  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->init = init;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((ButtonInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((ButtonInterface *) clazz->interface)->initWithImage = initWithImage;
  ((ButtonInterface *) clazz->interface)->initWithTitle = initWithTitle;
}

/**
 * @fn Class *Button::_Button(void)
 * @memberof Button
 */
Class *_Button(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Button",
      .superclass = _Control(),
      .instanceSize = sizeof(Button),
      .interfaceOffset = offsetof(Button, interface),
      .interfaceSize = sizeof(ButtonInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
