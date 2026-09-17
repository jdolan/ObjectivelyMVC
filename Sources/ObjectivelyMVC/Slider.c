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
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Objectively/Number.h>

#include "Colors.h"
#include "Log.h"
#include "Slider.h"

#define _Class _Slider

/**
 * @return The index in `values` nearest to the given value.
 */
static size_t indexOfValue(const Slider *self, double value) {

  size_t index = 0;
  double nearest = DBL_MAX;

  for (size_t i = 0; i < self->values->count; i++) {
    const double delta = fabs(VectorValue(self->values, double, i) - value);
    if (delta < nearest) {
      nearest = delta;
      index = i;
    }
  }

  return index;
}

/**
 * @return The handle position, `0.0` to `1.0`, of this Slider's current value.
 */
static double fractionOfValue(const Slider *self) {

  if (self->values) {
    if (self->values->count < 2) {
      return 0.0;
    }
    return indexOfValue(self, self->value) / (double) (self->values->count - 1);
  }

  return clamp((self->value - self->min) / (self->max - self->min), 0.0, 1.0);
}

/**
 * @return The value at the given handle position, snapped.
 */
static double valueAtFraction(const Slider *self, double fraction) {

  fraction = clamp(fraction, 0.0, 1.0);

  if (self->values) {
    return VectorValue(self->values, double, (size_t) round(fraction * (self->values->count - 1)));
  }

  const double value = self->min + (self->max - self->min) * fraction;

  if (self->snapToStep && self->step) {
    return clamp(round(value / self->step) * self->step, self->min, self->max);
  }

  return value;
}

/**
 * @return The value `steps` increments away from this Slider's current value.
 */
static double valueAfterSteps(const Slider *self, int steps) {

  if (self->values) {
    const ssize_t index = clamp((ssize_t) indexOfValue(self, self->value) + steps,
                                (ssize_t) 0, (ssize_t) self->values->count - 1);
    return VectorValue(self->values, double, index);
  }

  const double step = self->step ?: (self->max - self->min) / 20.0;
  return self->value + step * steps;
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Slider *this = (Slider *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  release(this->bar);
  release(this->handle);
  release(this->label);
  release(this->values);

  free(this->labelFormat);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @brief InletBinding for a table of non-linear values.
 */
static void bindValues(const Inlet *inlet, ident obj) {

  const Array *array = cast(Array, obj);
  assert(array->count);

  Vector *values = $(alloc(Vector), initWithSize, sizeof(double));
  assert(values);

  for (size_t i = 0; i < array->count; i++) {
    double value = cast(Number, $(array, objectAtIndex, i))->value;
    $(values, add, &value);
  }

  if (values->count < 2) {
    MVC_LogWarn("Inlet %s wanted 2 or more values but received %zd\n", inlet->name, values->count);
  }

  release(*(Vector **) inlet->dest);
  *(Vector **) inlet->dest = values;
}

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
    MakeInlet("snapToStep", InletTypeBool, &this->snapToStep, NULL),
    MakeInlet("step", InletTypeDouble, &this->step, NULL),
    MakeInlet("value", InletTypeDouble, &value, NULL),
    MakeInlet("values", InletTypeApplicationDefined, &this->values, bindValues)
  );

  $(self, bind, inlets, dictionary);

  $(this, setValue, value);

  // setValue only reformats on a change, and the labelFormat inlet does not format at all,
  // so a Slider whose bound value matches its initial one would keep the default format

  $(this, formatLabel);
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

  if (this->values || this->max > this->min) {

    if (((View *) this->label)->visibility != ViewVisibilityHidden) {
      int labelWidth = 0;
      char text[64];

      Text *label = (Text *) this->label;

      // the widest entry is not necessarily an extremity, e.g. "0.25" over "3"

      if (this->values) {
        for (size_t i = 0; i < this->values->count; i++) {
          int width;

          snprintf(text, sizeof(text), this->labelFormat, VectorValue(this->values, double, i));
          $(label->font, sizeCharacters, text, &width, NULL);

          labelWidth = max(labelWidth, width);
        }
      } else {
        int minWidth, maxWidth;

        snprintf(text, sizeof(text), this->labelFormat, this->min);
        $(label->font, sizeCharacters, text, &minWidth, NULL);

        snprintf(text, sizeof(text), this->labelFormat, this->max);
        $(label->font, sizeCharacters, text, &maxWidth, NULL);

        labelWidth = max(minWidth, maxWidth);
      }

      this->bar->frame.w -= labelWidth + label->view.padding.left;
    }

    const double fraction = fractionOfValue(this);
    const SDL_Rect bounds = $(this->bar, bounds);

    View *handle = (View *) this->handle;
    handle->frame.x = (bounds.w * fraction) - handle->frame.w * 0.5;
  } else {
    if (this->max < this->min) {
      MVC_LogWarn("max must be greater than min");
    }
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

  $(renderer, drawLine, points, &Colors.White);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  Slider *this = (Slider *) self;

  const SDL_Rect frame = $((View *) this->bar, renderFrame);

  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {

    // The handle captures mouse motion events allowing the user to drag it
    // However, the user may also click left or right of the handle to decrement
    // or increment the value

    if ($((View *) this->handle, didReceiveEvent, event)) {
      self->state |= ControlStateHighlighted;
    } else {
      const int x = event->button.x - frame.x;

      $(this, setValue, valueAfterSteps(this, x > this->handle->frame.x ? 1 : -1));

      if (this->delegate.didSetValue) {
        this->delegate.didSetValue(this, this->value);
      }
    }
    return true;
  }

  else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    if (self->state & ControlStateHighlighted) {
      self->state &= ~ControlStateHighlighted;
    }
    return true;
  }

  else if (event->type == SDL_EVENT_MOUSE_MOTION) {
    if (self->state & ControlStateHighlighted) {
      if (frame.w) {

        const double fraction = (double) (event->motion.x - frame.x) / (double) frame.w;
        const double value = valueAtFraction(this, fraction);

        const double delta = fabs(this->value - value);
        if (delta > __DBL_EPSILON__) {
          $(this, setValue, value);

          if (this->delegate.didSetValue) {
            this->delegate.didSetValue(this, this->value);
          }
        }

      }
    }
    return true;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {

    int steps = 0;
    switch (event->key.key) {
      case SDLK_LEFT:
        steps = -1;
        break;
      case SDLK_RIGHT:
        steps = 1;
        break;
    }

    if (steps) {
      $(this, setValue, valueAfterSteps(this, steps));

      if (this->delegate.didSetValue) {
        this->delegate.didSetValue(this, this->value);
      }
      return true;
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

    self->handle = $(alloc(View), initWithFrame, NULL);
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

  if (self->values) {
    value = VectorValue(self->values, double, indexOfValue(self, value));
  } else {
    value = clamp(value, self->min, self->max);
  }

  const double delta = fabs(self->value - value);
  if (delta > __DBL_EPSILON__) {
    self->value = value;
    $((View *) self, setNeedsLayout);

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
      .interfaceSize = sizeof(SliderInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
