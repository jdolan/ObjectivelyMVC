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

#include <ObjectivelyMVC/Checkbox.h>

#include <check.png.h>

#define _Class _Checkbox

static Image *_check;

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Checkbox *this = (Checkbox *) self;

	release(this->box);
	release(this->check);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Checkbox *this = (Checkbox *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("check", InletTypeView, &this->check, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Checkbox *) self, initWithFrame, NULL, ControlStyleDefault);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	const View *box = (View *) ((Checkbox *) self)->box;
	
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		if ($(box, didReceiveEvent, event)) {
			self->state |= ControlStateHighlighted;
			return true;
		}
	}
	if (event->type == SDL_MOUSEBUTTONUP) {
		if ($(box, didReceiveEvent, event)) {
			self->state ^= ControlStateSelected;
			self->state &= ~ControlStateHighlighted;
			return true;
		}
	}

	return super(Control, self, captureEvent, event);
}

/**
 * @see Control::stateDidChange(Control *)
 */
static void stateDidChange(Control *self) {

	super(Control, self, stateDidChange);

	Checkbox *this = (Checkbox *) self;

	if (self->state & ControlStateSelected) {
		this->check->view.hidden = false;
	} else {
		this->check->view.hidden = true;
	}
}

#pragma mark - Checkbox

/**
 * @fn Checkbox *Checkbox::initWithFrame(Checkbox *self, const SDL_Frame *frame, ControlStyle style)
 * @memberof Checkbox
 */
static Checkbox *initWithFrame(Checkbox *self, const SDL_Rect *frame, ControlStyle style) {

	self = (Checkbox *) super(Control, self, initWithFrame, frame, style);
	if (self) {

		self->control.view.autoresizingMask = ViewAutoresizingContain;
 
		self->box = $(alloc(Control), initWithFrame, frame, style);
		assert(self->box);

		self->box->view.alignment = ViewAlignmentMiddleCenter;

		self->check = $(alloc(ImageView), initWithImage, _check);
		assert(self->check);

		self->check->view.autoresizingMask = ViewAutoresizingFill;
		self->check->view.hidden = true;

		$((View *) self->box, addSubview, (View *) self->check);
		$((View *) self, addSubview, (View *) self->box);

		if (self->control.style == ControlStyleDefault) {
			self->box->bevel = ControlBevelInset;
		}
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

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->def->interface)->stateDidChange = stateDidChange;

	((CheckboxInterface *) clazz->def->interface)->initWithFrame = initWithFrame;

	_check = $(alloc(Image), initWithBytes, check_png, check_png_len);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_check);
}

/**
 * @fn Class *Checkbox::_Checkbox(void)
 * @memberof Checkbox
 */
Class *_Checkbox(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Checkbox";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(Checkbox);
		clazz.interfaceOffset = offsetof(Checkbox, interface);
		clazz.interfaceSize = sizeof(CheckboxInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
