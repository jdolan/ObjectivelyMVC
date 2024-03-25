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

#include "Checkbox.h"

#include "check.png.h"

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
	return (View *) $((Checkbox *) self, initWithFrame, NULL);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

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
 * @fn Checkbox *Checkbox::initWithFrame(Checkbox *self, const SDL_Frame *frame)
 * @memberof Checkbox
 */
static Checkbox *initWithFrame(Checkbox *self, const SDL_Rect *frame) {

	self = (Checkbox *) super(Control, self, initWithFrame, frame);
	if (self) {

		self->control.view.autoresizingMask = ViewAutoresizingContain;
 
		self->box = $(alloc(Control), initWithFrame, frame);
		assert(self->box);

		self->box->view.alignment = ViewAlignmentMiddleCenter;

		self->check = $(alloc(ImageView), initWithImage, _check);
		assert(self->check);

		self->check->view.autoresizingMask = ViewAutoresizingFill;
		self->check->view.hidden = true;

		$((View *) self->box, addSubview, (View *) self->check);
		$((View *) self, addSubview, (View *) self->box);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->interface)->init = init;

	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;

	((CheckboxInterface *) clazz->interface)->initWithFrame = initWithFrame;

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
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Checkbox",
			.superclass = _Control(),
			.instanceSize = sizeof(Checkbox),
			.interfaceOffset = offsetof(Checkbox, interface),
			.interfaceSize = sizeof(CheckboxInterface),
			.initialize = initialize,
			.destroy = destroy,
		});
	});

	return clazz;
}

#undef _Class
