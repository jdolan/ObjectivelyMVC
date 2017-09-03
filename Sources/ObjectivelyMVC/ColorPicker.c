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

#include <ObjectivelyMVC/ColorPicker.h>
#include <ObjectivelyMVC/Input.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/View.h>

#define _Class _ColorPicker

#pragma mark - Actions & delegates

/**
 * @brief SliderDelegate callback for color component modification.
 */
static void didSetComponent(Slider *slider) {

	ColorPicker *this = (ColorPicker *) slider->delegate.self;

	const int c = round(slider->value);

	if (slider == this->r) {
		this->color.r = c;
	} else if (slider == this->g) {
		this->color.g = c;
	} else if (slider == this->b) {
		this->color.b = c;
	} else if (slider == this->a) {
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

	ColorPicker *this = (ColorPicker *) self;

	release(this->colorView);

	release(this->r);
	release(this->g);
	release(this->b);
	release(this->a);

	release(this->red);
	release(this->green);
	release(this->blue);
	release(this->alpha);

	super(Object, self, dealloc);
}

#pragma mark - View

static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	ColorPicker *this = (ColorPicker *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("color", InletTypeColor, &this->color, NULL),
		MakeInlet("colorView", InletTypeView, &this->colorView, NULL),
		MakeInlet("r", InletTypeView, &this->r, NULL),
		MakeInlet("g", InletTypeView, &this->g, NULL),
		MakeInlet("b", InletTypeView, &this->b, NULL),
		MakeInlet("a", InletTypeView, &this->a, NULL),
		MakeInlet("red", InletTypeView, &this->red, NULL),
		MakeInlet("green", InletTypeView, &this->green, NULL),
		MakeInlet("blue", InletTypeView, &this->blue, NULL),
		MakeInlet("alpha", InletTypeView, &this->alpha, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((ColorPicker *) self, initWithFrame, NULL);
}

/**
 * @see View::updateBindings(View *)
 */
static void updateBindings(View *self) {

	super(View, self, updateBindings);

	ColorPicker *this = (ColorPicker *) self;

	this->colorView->backgroundColor.r = this->color.r;
	this->colorView->backgroundColor.g = this->color.g;
	this->colorView->backgroundColor.b = this->color.b;
	this->colorView->backgroundColor.a = this->color.a;
}

#pragma mark - ColorPicker

/**
 * @fn ColorPicker *ColorPicker::initWithFrame(ColorPicker *self, const SDL_Rect *frame)
 * @memberof ColorPicker
 */
static ColorPicker *initWithFrame(ColorPicker *self, const SDL_Rect *frame) {

	self = (ColorPicker *) super(StackView, self, initWithFrame, frame);
	if (self) {

		self->colorView = $(alloc(View), initWithFrame, &MakeRect(0, 0, 96, 96));
		assert(self->colorView);

		$((View *) self, addSubview, self->colorView);

		self->r = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
		self->r->delegate.self = self;
		self->r->delegate.didSetValue = didSetComponent;
		self->r->max = 255.0;
		$(self->r, setLabelFormat, "%.0f");

		self->red = $(alloc(Input), initWithFrame, NULL);
		assert(self->red);

		$(self->red, setControl, (Control *) self->r);
		$(self->red->label->text, setText, "R");

		$((View *) self, addSubview, (View *) self->red);

		self->g = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
		self->g->delegate.self = self;
		self->g->delegate.didSetValue = didSetComponent;
		self->g->max = 255.0;
		$(self->g, setLabelFormat, "%.0f");

		self->green = $(alloc(Input), initWithFrame, NULL);
		assert(self->green);

		$(self->green, setControl, (Control *) self->g);
		$(self->green->label->text, setText, "G");

		$((View *) self, addSubview, (View *) self->green);

		self->b = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
		self->b->delegate.self = self;
		self->b->delegate.didSetValue = didSetComponent;
		self->b->max = 255.0;
		$(self->b, setLabelFormat, "%.0f");

		self->blue = $(alloc(Input), initWithFrame, NULL);
		assert(self->blue);

		$(self->blue, setControl, (Control *) self->b);
		$(self->blue->label->text, setText, "B");

		$((View *) self, addSubview, (View *) self->blue);

		self->a = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
		self->a->delegate.self = self;
		self->a->delegate.didSetValue = didSetComponent;
		self->a->max = 255.0;
		$(self->a, setLabelFormat, "%.0f");

		self->alpha = $(alloc(Input), initWithFrame, NULL);
		assert(self->alpha);

		$(self->alpha, setControl, (Control *) self->a);
		$(self->alpha->label->text, setText, "A");

		$((View *) self, addSubview, (View *) self->alpha);

		$(self, setColor, Colors.White);
	}

	return self;
}

/**
 * @fn void ColorPicker::setColor(ColorPicker *self, const SDL_Color color)
 * @memberof ColorPicker
 */
static void setColor(ColorPicker *self, const SDL_Color color) {

	$(self->r, setValue, color.r);
	$(self->g, setValue, color.g);
	$(self->b, setValue, color.b);
	$(self->a, setValue, color.a);
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

	((ColorPickerInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ColorPickerInterface *) clazz->def->interface)->setColor = setColor;
}

/**
 * @fn Class *ColorPicker::_ColorPicker(void)
 * @memberof ColorPicker
 */
Class *_ColorPicker(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "ColorPicker";
		clazz.superclass = _StackView();
		clazz.instanceSize = sizeof(ColorPicker);
		clazz.interfaceOffset = offsetof(ColorPicker, interface);
		clazz.interfaceSize = sizeof(ColorPickerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
