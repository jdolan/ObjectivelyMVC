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

#include "Box.h"

#define _Class _Box

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Box *this = (Box *) self;

  release(this->contentView);
  release(this->label);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionar(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  Box *this = (Box *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("contentView", InletTypeView, &this->contentView, NULL),
    MakeInlet("label", InletTypeView, &this->label, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Box *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  super(View, self, layoutSubviews);

  View *label = (View *) ((Box *) self)->label;
  if (label->hidden == false) {

    const SDL_Size size = $(label, sizeThatContains);
    label->frame.y = -size.h * 0.5;
  }
}

#pragma mark - Box

/**
 * @fn Box *Box::initWithFrame(Box *self, const SDL_Rect *frame)
 * @memberof Box
 */
static Box *initWithFrame(Box *self, const SDL_Rect *frame) {

  self = (Box *) super(View, self, initWithFrame, frame);
  if (self) {

    self->contentView = $(alloc(StackView), initWithFrame, NULL);
    assert(self->contentView);

    $((View *) self->contentView, addClassName, "contentView");
    $((View *) self->contentView, addClassName, "container");

    $((View *) self, addSubview, (View *) self->contentView);

    self->label = $(alloc(Label), initWithText, NULL, NULL);
    assert(self->label);

    View *label = (View *) self->label;

    label->alignment = ViewAlignmentInternal;

    $((View *) self, addSubview, (View *) self->label);
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
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

  ((BoxInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *Box::_Box(void)
 * @memberof Box
 */
Class *_Box(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Box",
      .superclass = _View(),
      .instanceSize = sizeof(Box),
      .interfaceOffset = offsetof(Box, interface),
      .interfaceSize = sizeof(BoxInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
