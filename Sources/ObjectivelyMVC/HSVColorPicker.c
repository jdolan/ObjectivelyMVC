/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#include <ObjectivelyMVC/HSVColorPicker.h>

#define _Class _HSVColorPicker

#pragma mark - Actions & delegates

/**
 * @brief SliderDelegate callback for color component modification.
 */
static void didSetComponent(Slider *slider) {

	HSVColorPicker *this = (HSVColorPicker *) slider->delegate.self;

	if (slider == this->hueSlider) {
		this->hue = slider->value;
	} else if (slider == this->saturationSlider) {
		this->saturation = slider->value;
	} else if (slider == this->valueSlider) {
		this->value = slider->value;
	} else {
		assert(false);
	}

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

	HSVColorPicker *this = (HSVColorPicker *) self;

	release(this->colorView);
	release(this->hueSlider);
	release(this->hueInput);
	release(this->saturationSlider);
	release(this->saturationInput);
	release(this->valueSlider);
	release(this->valueInput);
	release(this->stackView);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	HSVColorPicker *this = (HSVColorPicker *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("colorView", InletTypeView, &this->colorView, NULL),
		MakeInlet("hue", InletTypeDouble, &this->hue, NULL),
		MakeInlet("saturation", InletTypeDouble, &this->saturation, NULL),
		MakeInlet("value", InletTypeDouble, &this->value, NULL),
		MakeInlet("hueSlider", InletTypeView, &this->hueSlider, NULL),
		MakeInlet("hueInput", InletTypeView, &this->hueInput, NULL),
		MakeInlet("saturationSlider", InletTypeView, &this->saturationSlider, NULL),
		MakeInlet("saturationInput", InletTypeView, &this->saturationInput, NULL),
		MakeInlet("valueSlider", InletTypeView, &this->valueSlider, NULL),
		MakeInlet("valueInput", InletTypeView, &this->valueInput, NULL),
		MakeInlet("stackView", InletTypeView, &this->stackView, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((HSVColorPicker *) self, initWithFrame, NULL);
}

/**
 * @see View::updateBindings(View *)
 */
static void updateBindings(View *self) {

	super(View, self, updateBindings);

	HSVColorPicker *this = (HSVColorPicker *) self;

	$(this->hueSlider, setValue, this->hue);
	$(this->saturationSlider, setValue, this->saturation);
	$(this->valueSlider, setValue, this->value);

	this->colorView->backgroundColor = $(this, rgbColor);
}

#pragma mark - HSVColorPicker

/**
 * @fn HSVColorPicker *HSVColorPicker::initWithFrame(HSVColorPicker *self, const SDL_Rect *frame)
 * @memberof HSVColorPicker
 */
static HSVColorPicker *initWithFrame(HSVColorPicker *self, const SDL_Rect *frame) {

	self = (HSVColorPicker *) super(Control, self, initWithFrame, frame);
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
		self->hueSlider->delegate.didSetValue = didSetComponent;
		self->hueSlider->max = 360.0;
		$(self->hueSlider, setLabelFormat, "%.0lf");

		self->hueInput = $(alloc(Input), initWithFrame, NULL);
		assert(self->hueInput);

		$(self->hueInput, setControl, (Control *) self->hueSlider);
		$(self->hueInput->label->text, setText, "H");

		$((View *) self->stackView, addSubview, (View *) self->hueInput);

		self->saturationSlider = $(alloc(Slider), initWithFrame, NULL);
		assert(self->saturationSlider);

		self->saturationSlider->delegate.self = self;
		self->saturationSlider->delegate.didSetValue = didSetComponent;
		self->saturationSlider->max = 1.0;
		$(self->saturationSlider, setLabelFormat, "%.3f");

		self->saturationInput = $(alloc(Input), initWithFrame, NULL);
		assert(self->saturationInput);

		$(self->saturationInput, setControl, (Control *) self->saturationSlider);
		$(self->saturationInput->label->text, setText, "S");

		$((View *) self->stackView, addSubview, (View *) self->saturationInput);

		self->valueSlider = $(alloc(Slider), initWithFrame, NULL);
		assert(self->valueSlider);

		self->valueSlider->delegate.self = self;
		self->valueSlider->delegate.didSetValue = didSetComponent;
		self->valueSlider->max = 1.0;
		$(self->valueSlider, setLabelFormat, "%.3f");

		self->valueInput = $(alloc(Input), initWithFrame, NULL);
		assert(self->valueInput);

		$(self->valueInput, setControl, (Control *) self->valueSlider);
		$(self->valueInput->label->text, setText, "V");

		$((View *) self->stackView, addSubview, (View *) self->valueInput);

		$(self, setColor, 0.0, 1.0, 1.0);
	}

	return self;
}

/**
 * @fn SDL_Color HSVColorPicker::rgbColor(const HSVColorPicker *self);
 * @memberof HSVColorPicker
 */
static SDL_Color rgbColor(const HSVColorPicker *self) {
	return MVC_HSVToRGB(self->hue, self->saturation, self->value);
}

/**
 * @fn void HSVColorPicker::setColor(HSVColorPicker *self, double hue, double saturation, double value)
 * @memberof HSVColorPicker
 */
static void setColor(HSVColorPicker *self, double hue, double saturation, double value) {

	self->hue = hue;
	self->saturation = saturation;
	self->value = value;

	$((View *) self, updateBindings);
}

/**
 * @fn void HSVColorPicker::setRGBColor(HSVColorPicker *self, const SDL_Color *color)
 * @memberof HSVColorPicker
 */
static void setRGBColor(HSVColorPicker *self, const SDL_Color *color) {

	MVC_RGBToHSV(color, &self->hue, &self->saturation, &self->value);

	$((View *) self, updateBindings);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->updateBindings = updateBindings;

	((HSVColorPickerInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((HSVColorPickerInterface *) clazz->def->interface)->rgbColor = rgbColor;
	((HSVColorPickerInterface *) clazz->def->interface)->setColor = setColor;
	((HSVColorPickerInterface *) clazz->def->interface)->setRGBColor = setRGBColor;
}

/**
 * @fn Class *HSVColorPicker::_HSVColorPicker(void)
 * @memberof HSVColorPicker
 */
Class *_HSVColorPicker(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "HSVColorPicker";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(HSVColorPicker);
		clazz.interfaceOffset = offsetof(HSVColorPicker, interface);
		clazz.interfaceSize = sizeof(HSVColorPickerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
