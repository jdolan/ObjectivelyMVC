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

#include "Log.h"
#include "Slider.h"

#define _Class _Slider

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Slider *this = (Slider *) self;

  release(this->bar);
  release(this->handle);
  release(this->label);

  free(this->labelFormat);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  Slider *this = (Slider *) self;

  double value = this->value;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("bar", InletTypeView, &this->bar, NULL),
    MakeInlet("handle", InletTypeView, &this->handle, NULL),
    MakeInlet("label", InletTypeView, &this->label, NULL),
    MakeInlet("labelFormat", InletTypeCharacters, &this->labelFormat, NULL),
    MakeInlet("min", InletTypeDouble, &this->min, NULL),
    MakeInlet("max", InletTypeDouble, &this->max, NULL),
    MakeInlet("step", InletTypeDouble, &this->step, NULL),
    MakeInlet("value", InletTypeDouble, &value, NULL)
  );

  $(self, bind, inlets, dictionary);

  $(this, setValue, value);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Slider *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  super(View, self, layoutSubviews);

  Slider *this = (Slider *) self;

  if (this->max > this->min) {

    if (((View *) this->label)->hidden == false) {
      int minWidth, maxWidth;
      char text[64];

      Text *label = (Text *) this->label;

      snprintf(text, sizeof(text), this->labelFormat, this->min);
      $(label->font, sizeCharacters, text, &minWidth, NULL);

      snprintf(text, sizeof(text), this->labelFormat, this->max);
      $(label->font, sizeCharacters, text, &maxWidth, NULL);

      this->bar->frame.w -= max(minWidth, maxWidth) + label->view.padding.left;
    }

    const double fraction = clamp((this->value - this->min) / (this->max - this->min), 0.0, 1.0);
    const SDL_Rect bounds = $(this->bar, bounds);

    View *handle = (View *) this->handle;
    handle->frame.x = (bounds.w * fraction) - handle->frame.w * 0.5;
  } else {
    MVC_LogWarn("max > min");
  }
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

  super(View, self, render, renderer);

  Slider *this = (Slider *) self;

  const SDL_Rect frame = $(this->bar, renderFrame);

  const SDL_Point points[] = {
    { frame.x, frame.y + frame.h * 0.5 },
    { frame.x + frame.w, frame.y + frame.h * 0.5 }
  };

  $(renderer, drawLine, points);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  Slider *this = (Slider *) self;

  if (event->type == SDL_MOUSEBUTTONDOWN) {
    if ($((View *) this->handle, didReceiveEvent, event)) {
      self->state |= ControlStateHighlighted;
    }
  }

  else if (event->type == SDL_MOUSEBUTTONUP) {
    if (self->state & ControlStateHighlighted) {
      self->state &= ~ControlStateHighlighted;
    }
  }

  else if (event->type == SDL_MOUSEMOTION) {
    if (self->state & ControlStateHighlighted) {
      const SDL_Rect frame = $((View *) this->bar, renderFrame);
      if (frame.w) {

        const double fraction = (event->motion.x - frame.x) / (double) frame.w;
        double value = this->min + (this->max - this->min) * clamp(fraction, 0.0, 1.0);

        if (this->snapToStep && this->step) {
          value = clamp(round(value / this->step) * this->step, this->min, this->max);
        }

        const double delta = fabs(this->value - value);
        if (delta > __DBL_EPSILON__) {
          $(this, setValue, value);

          if (this->delegate.didSetValue) {
            this->delegate.didSetValue(this, this->value);
          }
        }

        return true;
      }
    }
  }

  return super(Control, self, captureEvent, event);
}

#pragma mark - Slider

/**
 * @fn void Slider::formatLabel(Slider *self)
 * @memberof Slider
 */
static void formatLabel(Slider *self) {

  char text[64];
  snprintf(text, sizeof(text), self->labelFormat, self->value);

  $(self->label, setText, text);
}

/**
 * @fn Slider *Slider::init(Slider *self)
 * @memberof Slider
 */
static Slider *initWithFrame(Slider *self, const SDL_Rect *frame) {

  self = (Slider *) super(Control, self, initWithFrame, frame);
  if (self) {

    self->bar = $(alloc(View), initWithFrame, frame);
    assert(self->bar);

    $(self->bar, addClassName, "bar");

    $((View *) self, addSubview, self->bar);

    self->handle = $(alloc(Control), initWithFrame, NULL);
    assert(self->handle);

    $((View *) self->handle, addClassName, "handle");

    $(self->bar, addSubview, (View *) self->handle);

    self->label = $(alloc(Text), initWithText, NULL, NULL);
    assert(self->label);

    $((View *) self->label, addClassName, "label");

    $((View *) self, addSubview, (View *) self->label);

    $(self, setLabelFormat, "%0.1f");
  }

  return self;
}

/**
 * @fn void Slider::setLabelFormat(Slider *self, const char *labelFormat)
 * @memberof Slider
 */
static void setLabelFormat(Slider *self, const char *labelFormat) {

  if (self->labelFormat) {
    free(self->labelFormat);
  }

  self->labelFormat = strdup(labelFormat);
  $(self, formatLabel);
}

/**
 * @fn void Slider::setValue(Slider *self, double value)
 * @memberof Slider
 */
static void setValue(Slider *self, double value) {

  value = clamp(value, self->min, self->max);

  const double delta = fabs(self->value - value);
  if (delta > __DBL_EPSILON__) {
    self->value = value;
    self->control.view.needsLayout = true;

    $(self, formatLabel);
  }
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
  ((ViewInterface *) clazz->interface)->render = render;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((SliderInterface *) clazz->interface)->formatLabel = formatLabel;
  ((SliderInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((SliderInterface *) clazz->interface)->setValue = setValue;
  ((SliderInterface *) clazz->interface)->setLabelFormat = setLabelFormat;
}

/**
 * @fn Class *Slider::_Slider(void)
 * @memberof Slider
 */
Class *_Slider(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Slider",
      .superclass = _Control(),
      .instanceSize = sizeof(Slider),
      .interfaceOffset = offsetof(Slider, interface),
      .interfaceSize = sizeof(SliderInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
