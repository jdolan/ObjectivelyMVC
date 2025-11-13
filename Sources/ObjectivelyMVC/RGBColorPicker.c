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
#include <math.h>

#include "RGBColorPicker.h"

#define _Class _RGBColorPicker

#pragma mark - Delegates

/**
 * @brief SliderDelegate callback for color component modification.
 */
static void didSetComponent(Slider *slider, double value) {

  RGBColorPicker *this = (RGBColorPicker *) slider->delegate.self;

  const int c = round(value);

  if (slider == this->redSlider) {
    this->color.r = c;
  } else if (slider == this->greenSlider) {
    this->color.g = c;
  } else if (slider == this->blueSlider) {
    this->color.b = c;
  } else if (slider == this->alphaSlider) {
    this->color.a = c;
  } else {
    assert(false);
  }

  $((View *) this, updateBindings);

  if (this->delegate.didPickColor) {
    this->delegate.didPickColor(this, &this->color);
  }
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  RGBColorPicker *this = (RGBColorPicker *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  release(this->colorView);
  release(this->redSlider);
  release(this->redInput);
  release(this->greenSlider);
  release(this->greenInput);
  release(this->blueSlider);
  release(this->blueInput);
  release(this->alphaSlider);
  release(this->alphaInput);
  release(this->stackView);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  RGBColorPicker *this = (RGBColorPicker *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("color", InletTypeColor, &this->color, NULL),
    MakeInlet("colorView", InletTypeView, &this->colorView, NULL),
    MakeInlet("redSlider", InletTypeView, &this->redSlider, NULL),
    MakeInlet("redInput", InletTypeView, &this->redInput, NULL),
    MakeInlet("greenSlider", InletTypeView, &this->greenSlider, NULL),
    MakeInlet("greenInput", InletTypeView, &this->greenInput, NULL),
    MakeInlet("blueSlider", InletTypeView, &this->blueSlider, NULL),
    MakeInlet("blueInput", InletTypeView, &this->blueInput, NULL),
    MakeInlet("alphaSlider", InletTypeView, &this->alphaSlider, NULL),
    MakeInlet("alphaInput", InletTypeView, &this->alphaInput, NULL),
    MakeInlet("stackView", InletTypeView, &this->stackView, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((RGBColorPicker *) self, initWithFrame, NULL);
}

/**
 * @see View::updateBindings(View *)
 */
static void updateBindings(View *self) {

  super(View, self, updateBindings);

  RGBColorPicker *this = (RGBColorPicker *) self;

  $(this->redSlider, setValue, this->color.r);
  $(this->greenSlider, setValue, this->color.g);
  $(this->blueSlider, setValue, this->color.b);
  $(this->alphaSlider, setValue, this->color.a);

  this->colorView->backgroundColor = this->color;
}

#pragma mark - RGBColorPicker

/**
 * @fn RGBColorPicker *RGBColorPicker::initWithFrame(RGBColorPicker *self, const SDL_Rect *frame)
 * @memberof RGBColorPicker
 */
static RGBColorPicker *initWithFrame(RGBColorPicker *self, const SDL_Rect *frame) {

  self = (RGBColorPicker *) super(Control, self, initWithFrame, frame);
  if (self) {

    self->stackView = $(alloc(StackView), initWithFrame, NULL);
    assert(self->stackView);

    $((View *) self, addSubview, (View *) self->stackView);

    self->colorView = $(alloc(View), initWithFrame, &MakeRect(0, 0, 0, 24));
    assert(self->colorView);

    $(self->colorView, addClassName, "colorView");
    $((View *) self->stackView, addSubview, self->colorView);

    self->redSlider = $(alloc(Slider), initWithFrame, NULL);
    assert(self->redSlider);

    self->redSlider->delegate.self = self;
    self->redSlider->delegate.didSetValue = didSetComponent;
    self->redSlider->max = 255.0;
    $(self->redSlider, setLabelFormat, "%.0f");

    self->redInput = $(alloc(Input), initWithFrame, NULL);
    assert(self->redInput);

    $(self->redInput, setControl, (Control *) self->redSlider);
    $(self->redInput->label->text, setText, "R");

    $((View *) self->stackView, addSubview, (View *) self->redInput);

    self->greenSlider = $(alloc(Slider), initWithFrame, NULL);
    assert(self->greenSlider);

    self->greenSlider->delegate.self = self;
    self->greenSlider->delegate.didSetValue = didSetComponent;
    self->greenSlider->max = 255.0;
    $(self->greenSlider, setLabelFormat, "%.0f");

    self->greenInput = $(alloc(Input), initWithFrame, NULL);
    assert(self->greenInput);

    $(self->greenInput, setControl, (Control *) self->greenSlider);
    $(self->greenInput->label->text, setText, "G");

    $((View *) self->stackView, addSubview, (View *) self->greenInput);

    self->blueSlider = $(alloc(Slider), initWithFrame, NULL);
    assert(self->blueSlider);

    self->blueSlider->delegate.self = self;
    self->blueSlider->delegate.didSetValue = didSetComponent;
    self->blueSlider->max = 255.0;
    $(self->blueSlider, setLabelFormat, "%.0f");

    self->blueInput = $(alloc(Input), initWithFrame, NULL);
    assert(self->blueInput);

    $(self->blueInput, setControl, (Control *) self->blueSlider);
    $(self->blueInput->label->text, setText, "B");

    $((View *) self->stackView, addSubview, (View *) self->blueInput);

    self->alphaSlider = $(alloc(Slider), initWithFrame, NULL);
    assert(self->alphaSlider);

    self->alphaSlider->delegate.self = self;
    self->alphaSlider->delegate.didSetValue = didSetComponent;
    self->alphaSlider->max = 255.0;
    $(self->alphaSlider, setLabelFormat, "%.0f");

    self->alphaInput = $(alloc(Input), initWithFrame, NULL);
    assert(self->alphaInput);

    $(self->alphaInput, setControl, (Control *) self->alphaSlider);
    $(self->alphaInput->label->text, setText, "A");

    $((View *) self->stackView, addSubview, (View *) self->alphaInput);

    $(self, setColor, &Colors.White);
  }

  return self;
}

/**
 * @fn void RGBColorPicker::setColor(RGBColorPicker *self, const SDL_Color *color)
 * @memberof RGBColorPicker
 */
static void setColor(RGBColorPicker *self, const SDL_Color *color) {

  assert(color);

  self->color = *color;

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

  ((RGBColorPickerInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((RGBColorPickerInterface *) clazz->interface)->setColor = setColor;
}

/**
 * @fn Class *RGBColorPicker::_RGBColorPicker(void)
 * @memberof RGBColorPicker
 */
Class *_RGBColorPicker(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "RGBColorPicker",
      .superclass = _Control(),
      .instanceSize = sizeof(RGBColorPicker),
      .interfaceOffset = offsetof(RGBColorPicker, interface),
      .interfaceSize = sizeof(RGBColorPickerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
