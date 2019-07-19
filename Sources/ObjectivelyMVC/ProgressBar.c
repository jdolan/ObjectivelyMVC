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

#include <ObjectivelyMVC/ProgressBar.h>

#define _Class _ProgressBar

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ProgressBar *this = (ProgressBar *) self;

	release(this->background);
	release(this->foreground);
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

	ProgressBar *this = (ProgressBar *) self;

	double value = this->value;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("background", InletTypeView, &this->background, NULL),
		MakeInlet("foreground", InletTypeView, &this->foreground, NULL),
		MakeInlet("label", InletTypeView, &this->label, NULL),
		MakeInlet("labelFormat", InletTypeCharacters, &this->labelFormat, NULL),
		MakeInlet("min", InletTypeDouble, &this->min, NULL),
		MakeInlet("max", InletTypeDouble, &this->max, NULL),
		MakeInlet("value", InletTypeDouble, &value, NULL)
	);

	$(self, bind, inlets, dictionary);

	$(this, setValue, value);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((ProgressBar *) self, initWithFrame, NULL);
}

#pragma mark - ProgressBar

/**
 * @fn void ProgressBar::formatLabel(ProgressBar *self)
 * @memberof ProgressBar
 */
static void formatLabel(ProgressBar *self) {

	char text[128];
	snprintf(text, sizeof(text), self->labelFormat, self->value);

	$(self->label, setText, text);
}

/**
 * @fn ProgressBar *ProgressBar::initWithFrame(ProgressBar *self, const SDL_Rect *frame)
 * @memberof ProgressBar
 */
static ProgressBar *initWithFrame(ProgressBar *self, const SDL_Rect *frame) {

	self = (ProgressBar *) super(View, self, initWithFrame, frame);
	if (self) {

		self->background = $(alloc(ImageView), initWithFrame, NULL);
		assert(self->background);

		$((View *) self->background, addClassName, "background");
		$((View *) self, addSubview, (View *) self->background);

		self->foreground = $(alloc(ImageView), initWithFrame, NULL);
		assert(self->foreground);

		$((View *) self->foreground, addClassName, "foreground");
		$((View *) self, addSubview, (View *) self->foreground);

		self->label = $(alloc(Text), initWithText, NULL, NULL);
		assert(self->label);

		$((View *) self, addSubview, (View *) self->label);

		$(self, setLabelFormat, "%0.0lf%%");

		self->value = -1.0;
		self->max = 100.0;
	}

	return self;
}

/**
 * @fn double ProgressBar::progress(const ProgressBar *self)
 * @memberof ProgressBar
 */
static double progress(const ProgressBar *self) {
	return 100.0 * self->value / (self->max - self->min);
}

/**
 * @fn void ProgressBar::setLabelFormat(ProgressBar *self, const char *labelFormat)
 * @memberof ProgressBar
 */
static void setLabelFormat(ProgressBar *self, const char *labelFormat) {

	if (self->labelFormat) {
		free(self->labelFormat);
	}

	self->labelFormat = strdup(labelFormat);
	$(self, formatLabel);
}

/**
 * @fn void ProgressBar::setValue(const ProgressBar *self, double value)
 * @memberof ProgressBar
 */
static void setValue(ProgressBar *self, double value) {

	value = clamp(value, self->min, self->max);

	const double delta = fabs(self->value - value);
	if (delta > __DBL_EPSILON__) {
		self->value = value;

		const SDL_Rect bounds = $((View *) self, bounds);
		const double frac = self->value / (self->max - self->min);

		self->foreground->view.frame.w = bounds.w * frac;
		self->view.needsLayout = true;

		$(self, formatLabel);

		if (self->delegate.didSetValue) {
			self->delegate.didSetValue(self, self->value);
		}
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

	((ProgressBarInterface *) clazz->interface)->formatLabel = formatLabel;
	((ProgressBarInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ProgressBarInterface *) clazz->interface)->progress = progress;
	((ProgressBarInterface *) clazz->interface)->setLabelFormat = setLabelFormat;
	((ProgressBarInterface *) clazz->interface)->setValue = setValue;
}

/**
 * @fn Class *ProgressBar::_ProgressBar(void)
 * @memberof ProgressBar
 */
Class *_ProgressBar(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "ProgressBar",
			.superclass = _View(),
			.instanceSize = sizeof(ProgressBar),
			.interfaceOffset = offsetof(ProgressBar, interface),
			.interfaceSize = sizeof(ProgressBarInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
