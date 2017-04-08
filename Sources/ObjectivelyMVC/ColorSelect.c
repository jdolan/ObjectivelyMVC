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

#include <ObjectivelyMVC/ColorSelect.h>
#include <ObjectivelyMVC/StackView.h>

#define _Class _ColorSelect

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ColorSelect *this = (ColorSelect *) self;

	release(this->sliderR);
	release(this->sliderG);
	release(this->sliderB);
	release(this->sliderA);

	super(Object, self, dealloc);
}

#pragma mark - View

static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	ColorSelect *this = (ColorSelect *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("useAlpha", InletTypeBool, &this->useAlpha, NULL),
		MakeInlet("color", InletTypeColor, &this->color, NULL)
	);

	$(self, bind, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((ColorSelect *) self, initWithFrame, NULL, true);
}

#pragma mark - ColorSelect

/**
 * @brief Update colors.
 */
static void updateColor(ColorSelect *this) {

	this->stackView.view.backgroundColor.r = this->color.r;
	this->stackView.view.backgroundColor.g = this->color.g;
	this->stackView.view.backgroundColor.b = this->color.b;

	if (this->delegate.didSetColor) {
		this->delegate.didSetColor(this);
	}
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetRed(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.r = round(this->sliderR->value);

	updateColor(this);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetGreen(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.g = round(this->sliderG->value);

	updateColor(this);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetBlue(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.b = round(this->sliderB->value);

	updateColor(this);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetAlpha(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	if (this->useAlpha) {
		this->color.a = round(this->sliderA->value);

		updateColor(this);
	} else  if (this->color.a != 255) {
		this->color.a = 255;

		updateColor(this);
	}
}

/**
 * @fn ColorSelect *ColorSelect::initWithFrame(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha)
 * @memberof ColorSelect
 */
static ColorSelect *initWithFrame(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha) {

	self = (ColorSelect *) super(StackView, self, initWithFrame, frame);
	if (self) {

		View *this = (View *) self;

		self->useAlpha = useAlpha;

		this->backgroundColor.a = 255;

		// Red slider

		self->sliderR = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

		self->sliderR->delegate.self = self; // Set self to the ColorSelect
		self->sliderR->delegate.didSetValue = didSetRed;

		self->sliderR->min = 0.0;
		self->sliderR->max = 255.0;
		self->sliderR->step = 5.0;

		self->sliderR->snapToStep = true;

		$((View *) self, addSubview, (View *) self->sliderR);

		// Green slider

		self->sliderG = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

		self->sliderG->delegate.self = self; // Set self to the ColorSelect
		self->sliderG->delegate.didSetValue = didSetGreen;

		self->sliderG->min = 0.0;
		self->sliderG->max = 255.0;
		self->sliderG->step = 5.0;

		self->sliderG->snapToStep = true;

		$((View *) self, addSubview, (View *) self->sliderG);

		// Blue slider

		self->sliderB = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

		self->sliderB->delegate.self = self; // Set self to the ColorSelect
		self->sliderB->delegate.didSetValue = didSetBlue;

		self->sliderB->min = 0.0;
		self->sliderB->max = 255.0;
		self->sliderB->step = 5.0;

		self->sliderB->snapToStep = true;

		$((View *) self, addSubview, (View *) self->sliderB);

		// Alpha slider

		if (self->useAlpha) {
			self->sliderA = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

			self->sliderA->delegate.self = self; // Set self to the ColorSelect
			self->sliderA->delegate.didSetValue = didSetAlpha;

			self->sliderA->min = 0.0;
			self->sliderA->max = 255.0;
			self->sliderA->step = 5.0;

			self->sliderA->snapToStep = true;

			$((View *) self, addSubview, (View *) self->sliderA);
		}

		const SDL_Color defaultColor = { .r = 0, .g = 0, .b = 0, .a = 255 };
		$(self, setColor, defaultColor);
	}

	return self;
}

/**
 * @fn ColorSelect *ColorSelect::setColor(ColorSelect *self, const SDL_Color color)
 * @memberof ColorSelect
 */
static ColorSelect *setColor(ColorSelect *self, const SDL_Color color) {

	$(self->sliderR, setValue, color.r);
	$(self->sliderG, setValue, color.g);
	$(self->sliderB, setValue, color.b);

	if (self->useAlpha) {
		$(self->sliderA, setValue, color.a);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;

	((ColorSelectInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ColorSelectInterface *) clazz->def->interface)->setColor = setColor;
}

/**
 * @fn Class *ColorSelect::_ColorSelect(void)
 * @memberof ColorSelect
 */
Class *_ColorSelect(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "ColorSelect";
		clazz.superclass = _StackView();
		clazz.instanceSize = sizeof(ColorSelect);
		clazz.interfaceOffset = offsetof(ColorSelect, interface);
		clazz.interfaceSize = sizeof(ColorSelectInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
