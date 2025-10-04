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

#include "HueColorPicker.h"

#define _Class _HueColorPicker

#pragma mark - Actions & delegates

/**
 * @brief SliderDelegate callback for hue modification.
 */
static void didSetHue(Slider *slider, double value) {

  HueColorPicker *this = (HueColorPicker *) slider->delegate.self;

  this->hue = value;

  $((View *) this, updateBindings);

  if (this->delegate.didPickColor) {
    this->delegate.didPickColor(this, this->hue, this->saturation, this->value);
  }
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  HueColorPicker *this = (HueColorPicker *) self;

  release(this->colorView);
  release(this->hueInput);
  release(this->hueSlider);
  release(this->stackView);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  HueColorPicker *this = (HueColorPicker *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("colorView", InletTypeView, &this->colorView, NULL),
    MakeInlet("hue", InletTypeDouble, &this->hue, NULL),
    MakeInlet("saturation", InletTypeDouble, &this->saturation, NULL),
    MakeInlet("value", InletTypeDouble, &this->value, NULL),
    MakeInlet("hueInput", InletTypeView, &this->hueInput, NULL),
    MakeInlet("hueSlider", InletTypeView, &this->hueSlider, NULL),
    MakeInlet("stackView", InletTypeView, &this->stackView, NULL)
  );
  
  $(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((HueColorPicker *) self, initWithFrame, NULL);
}

/**
 * @see View::updateBindings(View *)
 */
static void updateBindings(View *self) {

  super(View, self, updateBindings);

  HueColorPicker *this = (HueColorPicker *) self;

  $(this->hueSlider, setValue, this->hue);

  this->colorView->backgroundColor = $(this, rgbColor);
}

#pragma mark - HueColorPicker

/**
 * @fn HueColorPicker *HueColorPicker::initWithFrame(HueColorPicker *self, const SDL_Rect *frame)
 * @memberof HueColorPicker
 */
static HueColorPicker *initWithFrame(HueColorPicker *self, const SDL_Rect *frame) {

  self = (HueColorPicker *) super(Control, self, initWithFrame, frame);
  if (self) {

    self->stackView = $(alloc(StackView), initWithFrame, NULL);
    assert(self->stackView);

    $((View *) self, addSubview, (View *) self->stackView);

    self->colorView = $(alloc(View), initWithFrame, &MakeRect(0, 0, 0, 24));
    assert(self->colorView);

    $(self->colorView, addClassName, "colorView");
    $((View *) self->stackView, addSubview, self->colorView);

    self->hueSlider = $(alloc(Slider), initWithFrame, NULL);
    assert(self->hueSlider);

    self->hueSlider->delegate.self = self;
    self->hueSlider->delegate.didSetValue = didSetHue;
    self->hueSlider->max = 360.0;
    $(self->hueSlider, setLabelFormat, "%.0lf");

    self->hueInput = $(alloc(Input), initWithFrame, NULL);
    assert(self->hueInput);

    $(self->hueInput, setControl, (Control *) self->hueSlider);
    $(self->hueInput->label->text, setText, "H");

    $((View *) self->stackView, addSubview, (View *) self->hueInput);

    $(self, setColor, 0.0, 1.0, 1.0);
  }

  return self;
}

/**
 * @fn SDL_Color HueColorPicker::rgbColor(const HueColorPicker *self);
 * @memberof HueColorPicker
 */
static SDL_Color rgbColor(const HueColorPicker *self) {
  return MVC_HSVToRGB(self->hue, self->saturation, self->value);
}

/**
 * @fn void HueColorPicker::setColor(HueColorPicker *self, double hue, double saturation, double value)
 * @memberof HueColorPicker
 */
static void setColor(HueColorPicker *self, double hue, double saturation, double value) {

  self->hue = hue;
  self->saturation = saturation;
  self->value = value;

  $((View *) self, updateBindings);
}

/**
 * @fn void HueColorPicker::setRGBColor(HueColorPicker *self, const SDL_Color *color)
 * @memberof HueColorPicker
 */
static void setRGBColor(HueColorPicker *self, const SDL_Color *color) {

  MVC_RGBToHSV(color, &self->hue, &self->saturation, &self->value);

  $((View *) self, updateBindings);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->updateBindings = updateBindings;

  ((HueColorPickerInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((HueColorPickerInterface *) clazz->interface)->rgbColor = rgbColor;
  ((HueColorPickerInterface *) clazz->interface)->setColor = setColor;
  ((HueColorPickerInterface *) clazz->interface)->setRGBColor = setRGBColor;
}

/**
 * @fn Class *HueColorPicker::_HueColorPicker(void)
 * @memberof HueColorPicker
 */
Class *_HueColorPicker(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "HueColorPicker",
      .superclass = _Control(),
      .instanceSize = sizeof(HueColorPicker),
      .interfaceOffset = offsetof(HueColorPicker, interface),
      .interfaceSize = sizeof(HueColorPickerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
