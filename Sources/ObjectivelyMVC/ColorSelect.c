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
#include <ObjectivelyMVC/Input.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/View.h>

#define _Class _ColorSelect

#pragma mark - Helper functions

/**
 * @brief Helper function for initializing a slider
 */
static void addSlider(View *view, const char *label, Slider *slider) {

	assert(view);
	assert(slider);

	Input *input = $(alloc(Input), initWithFrame, NULL);
	assert(input);

	input->stackView.view.autoresizingMask = ViewAutoresizingWidth;
	input->stackView.view.padding.right = 6;

	slider->min = 0.0;
	slider->max = 255.0;
	slider->step = 5.0;

	slider->snapToStep = true;

	slider->control.view.autoresizingMask = ViewAutoresizingWidth;

	retain(slider); // Don't release with the Input

	$(input, setControl, (Control *) slider);

	$(input->label->text, setText, label);

	input->label->view.autoresizingMask &= ~ViewAutoresizingContain;

	$(view, addSubview, (View *) input);

	release(input);
}

#pragma mark - Actions & delegates

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetRed(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.r = round(this->sliderR->value);

	$((View *) this, updateBindings);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetGreen(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.g = round(this->sliderG->value);

	$((View *) this, updateBindings);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetBlue(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	this->color.b = round(this->sliderB->value);

	$((View *) this, updateBindings);
}

/**
 * @brief ColorSelectDelegate callback.
 */
static void didSetAlpha(Slider *slider) {

	ColorSelect *this = (ColorSelect *) slider;

	if (this->useAlpha) {
		this->color.a = round(this->sliderA->value);

		$((View *) this, updateBindings);
	} else  if (this->color.a != 255) {
		this->color.a = 255;

		$((View *) this, updateBindings);
	}
}

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

	release(this->colorView);

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

/**
 * @see View::updateBindings(View *)
 */
static void updateBindings(View *self) {

	super(View, self, updateBindings);

	ColorSelect *this = (ColorSelect *) self;

	this->colorView->backgroundColor.r = this->color.r;
	this->colorView->backgroundColor.g = this->color.g;
	this->colorView->backgroundColor.b = this->color.b;
	this->colorView->backgroundColor.a = this->color.a;

	if (this->delegate.didSetColor) {
		this->delegate.didSetColor(this->delegate.self);
	}
}

#pragma mark - ColorSelect

/**
 * @fn ColorSelect *ColorSelect::initWithFrame(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha)
 * @memberof ColorSelect
 */
static ColorSelect *initWithFrame(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha) {

	self = (ColorSelect *) super(StackView, self, initWithFrame, frame);
	if (self) {

		self->useAlpha = useAlpha;

		self->delegate.self = self;

		self->stackView.axis = StackViewAxisHorizontal;
		self->stackView.distribution = StackViewDistributionFill;

		{
			StackView *column = $(alloc(StackView), initWithFrame, NULL);

			column->distribution = StackViewDistributionFillEqually;

			column->view.autoresizingMask = ViewAutoresizingFill;

			{
				// Red slider

				self->sliderR = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

				addSlider((View *) column, "R", self->sliderR);

				self->sliderR->delegate.self = self; // Set self to the ColorSelect
				self->sliderR->delegate.didSetValue = didSetRed;

				// Green slider

				self->sliderG = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

				addSlider((View *) column, "G", self->sliderG);

				self->sliderG->delegate.self = self; // Set self to the ColorSelect
				self->sliderG->delegate.didSetValue = didSetGreen;

				// Blue slider

				self->sliderB = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

				addSlider((View *) column, "B", self->sliderB);

				self->sliderB->delegate.self = self; // Set self to the ColorSelect
				self->sliderB->delegate.didSetValue = didSetBlue;

				// Alpha slider

				if (self->useAlpha) {
					self->sliderA = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);

					addSlider((View *) column, "A", self->sliderA);

					self->sliderA->delegate.self = self; // Set self to the ColorSelect
					self->sliderA->delegate.didSetValue = didSetAlpha;
				}
			}

			$((View *) self, addSubview, (View *) column);
			release(column);
		}

		{
			View *column = $(alloc(View), initWithFrame, NULL); // Not a StackView because we need overlapping stuff

			column->autoresizingMask = ViewAutoresizingHeight;

			column->frame.w = 16;

			{
				self->colorView = $(alloc(View), init);

				self->colorView->autoresizingMask = ViewAutoresizingFill;

				$((View *) column, addSubview, self->colorView);
			}

			$((View *) self, addSubview, column);
			release(column);
		}

		const SDL_Color defaultColor = { .r = 0, .g = 0, .b = 0, .a = 255 };
		$(self, setColor, defaultColor);
	}

	return self;
}

/**
 * @fn void ColorSelect::setColor(ColorSelect *self, const SDL_Color color)
 * @memberof ColorSelect
 */
static void setColor(ColorSelect *self, const SDL_Color color) {

	// Slider delegates will be triggered for updating colors

	$(self->sliderR, setValue, color.r);
	$(self->sliderG, setValue, color.g);
	$(self->sliderB, setValue, color.b);

	if (self->useAlpha) {
		$(self->sliderA, setValue, color.a);
	} else {
		self->color.a = 255;
	}
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
