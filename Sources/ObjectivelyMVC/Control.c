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

#include "Control.h"

const EnumName ControlBevelNames[] = MakeEnumNames(
  MakeEnumAlias(ControlBevelNone, none),
  MakeEnumAlias(ControlBevelInset, inset),
  MakeEnumAlias(ControlBevelOutset, outset)
);

const EnumName ControlSelectionNames[] = MakeEnumNames(
  MakeEnumAlias(ControlSelectionNone, none),
  MakeEnumAlias(ControlSelectionSingle, single),
  MakeEnumAlias(ControlSelectionMultiple, multiple)
);

const EnumName ControlStateNames[] = MakeEnumNames(
  MakeEnumAlias(ControlStateDefault, default),
  MakeEnumAlias(ControlStateHighlighted, highlighted),
  MakeEnumAlias(ControlStateSelected, selected),
  MakeEnumAlias(ControlStateFocused, focused)
);

#define _Class _Control

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Control *this = (Control *) self;

  this->state = ControlStateDefault;

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::acceptsFirstResponder(const View *)
 */
static bool acceptsFirstResponder(const View *self) {
  return true;
}

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  Control *this = (Control *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("bevel", InletTypeEnum, &this->bevel, (ident) (ControlBevelNames)),
    MakeInlet("selection", InletTypeEnum, &this->selection, (ident) ControlSelectionNames)
  );

  $(self, bind, inlets, (Dictionary *) style->attributes);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Control *) self, initWithFrame, NULL);
}

/**
 * @fn View::becomeFirstResponder(View *)
 */
static void becomeFirstResponder(View *self) {

  Control *this = (Control *) self;

  if (!$(this, isFocused)) {

    this->state |= ControlStateFocused;

    $(this, stateDidChange);
  }

  super(View, self, becomeFirstResponder);
}

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

  assert(simpleSelector);

  const Control *this = (Control *) self;

  switch (simpleSelector->type) {
    case SimpleSelectorTypePseudo:
      if (strcmp("highlighted", simpleSelector->pattern) == 0) {
        return $(this, isHighlighted);
      } else if (strcmp("disabled", simpleSelector->pattern) == 0) {
        return $(this, isDisabled);
      } else if (strcmp("selected", simpleSelector->pattern) == 0) {
        return $(this, isSelected);
      } else if (strcmp("focused", simpleSelector->pattern) == 0) {
        return $(this, isFocused);
      } else if (strcmp("single", simpleSelector->pattern) == 0) {
        return this->selection == ControlSelectionSingle;
      } else if (strcmp("multiple", simpleSelector->pattern) == 0) {
        return this->selection == ControlSelectionMultiple;
      }
      break;
    default:
      break;
  }

  return super(View, self, matchesSelector, simpleSelector);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

  super(View, self, render, renderer);

  Control *this = (Control *) self;

  const SDL_Rect frame = $(self, renderFrame);

  if (this->bevel == ControlBevelInset) {

    $(renderer, setDrawColor, &Colors.Silver);

    SDL_Point points[3];

    points[0].x = frame.x;
    points[0].y = frame.y + frame.h;

    points[1].x = frame.x + frame.w;
    points[1].y = frame.y + frame.h;

    points[2].x = frame.x + frame.w;
    points[2].y = frame.y;

    $(renderer, drawLines, points, lengthof(points));

    $(renderer, setDrawColor, &Colors.Black);

    points[0].x = frame.x;
    points[0].y = frame.y + frame.h;

    points[1].x = frame.x;
    points[1].y = frame.y;

    points[2].x = frame.x + frame.w;
    points[2].y = frame.y;

    $(renderer, drawLines, points, lengthof(points));

  } else if (this->bevel == ControlBevelOutset) {

    $(renderer, setDrawColor, &Colors.Black);

    SDL_Point points[3];

    points[0].x = frame.x;
    points[0].y = frame.y + frame.h;

    points[1].x = frame.x + frame.w;
    points[1].y = frame.y + frame.h;

    points[2].x = frame.x + frame.w;
    points[2].y = frame.y;

    $(renderer, drawLines, points, lengthof(points));

    $(renderer, setDrawColor, &Colors.Silver);

    points[0].x = frame.x;
    points[0].y = frame.y + frame.h;

    points[1].x = frame.x;
    points[1].y = frame.y;

    points[2].x = frame.x + frame.w;
    points[2].y = frame.y;

    $(renderer, drawLines, points, lengthof(points));
  }

  if (this->state & ControlStateFocused) {

    $(renderer, setDrawColor, &Colors.Charcoal);

    $(renderer, drawRect, &frame);
  }

  $(renderer, setDrawColor, &Colors.White);
}

/**
 * @see View::resignFirstResponder(View *)
 */
static void resignFirstResponder(View *self) {

  Control *this = (Control *) self;

  if ($(this, isFocused)) {

    this->state &= ~ControlStateFocused;

    $(this, stateDidChange);
  }

  super(View, self, resignFirstResponder);
}

/**
 * @see View::respondToEvent(View *, SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

  Control *this = (Control *) self;

  const ControlState state = this->state;
  if ($(this, captureEvent, event)) {
    if (this->state != state) {
      $(this, stateDidChange);
    }
  } else {
    super(View, self, respondToEvent, event);
  }
}

#pragma mark - Control

/**
 * @fn bool Control::captureEvent(Control *self, const SDL_Event *event)
 * @memberof Control
 */
static bool captureEvent(Control *self, const SDL_Event *event) {
  return false;
}

/**
 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame)
 * @memberof Control
 */
static Control *initWithFrame(Control *self, const SDL_Rect *frame) {

  self = (Control *) super(View, self, initWithFrame, frame);
  if (self) {

  }

  return self;
}

/**
 * @fn bool Control::isDisabled(const Control *self)
 * @memberof Control
 */
static bool isDisabled(const Control *self) {
  return (self->state & ControlStateDisabled) == ControlStateDisabled;
}

/**
 * @fn bool Control::isFocused(const Control *self)
 * @memberof Control
 */
static bool isFocused(const Control *self) {
  return (self->state & ControlStateFocused) == ControlStateFocused;
}

/**
 * @fn bool Control::isHighlighted(const Control *self)
 * @memberof Control
 */
static bool isHighlighted(const Control *self) {
  return (self->state & ControlStateHighlighted) == ControlStateHighlighted;
}

/**
 * @fn bool Control::isSelected(const Control *self)
 * @memberof Control
 */
static bool isSelected(const Control *self) {
  return (self->state & ControlStateSelected) == ControlStateSelected;
}

/**
 * @fn void Control::stateDidChange(Control *self)
 * @memberof Control
 */
static void stateDidChange(Control *self) {

  View *this = (View *) self;

  if (self->state & ControlStateFocused) {
    $(this, emitViewEvent, ViewEventFocus, NULL);
  } else {
    $(this, emitViewEvent, ViewEventBlur, NULL);
  }

  $(this, invalidateStyle);

  this->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->acceptsFirstResponder = acceptsFirstResponder;
  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->becomeFirstResponder = becomeFirstResponder;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;
  ((ViewInterface *) clazz->interface)->render = render;
  ((ViewInterface *) clazz->interface)->resignFirstResponder = resignFirstResponder;
  ((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
  ((ControlInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((ControlInterface *) clazz->interface)->isDisabled = isDisabled;
  ((ControlInterface *) clazz->interface)->isFocused = isFocused;
  ((ControlInterface *) clazz->interface)->isHighlighted = isHighlighted;
  ((ControlInterface *) clazz->interface)->isSelected = isSelected;
  ((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;
}

/**
 * @fn Class *Control::_Control(void)
 * @memberof Control
 */
Class *_Control(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Control",
      .superclass = _View(),
      .instanceSize = sizeof(Control),
      .interfaceOffset = offsetof(Control, interface),
      .interfaceSize = sizeof(ControlInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
