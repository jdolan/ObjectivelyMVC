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

#define _Class _Checkbox

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
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {
	
	super(View, self, render, renderer);
	
	Checkbox *this = (Checkbox *) self;
	
	if (this->control.state & ControlStateSelected) {
		this->check->view.hidden = false;
	} else {
		this->check->view.hidden = true;
	}
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	if (event->type == SDL_MOUSEBUTTONUP) {

		Checkbox *this = (Checkbox *) self;
		
		if ($((View *) this->box, didReceiveEvent, event)) {
			self->state ^= ControlStateSelected;
			return true;
		}
	}
	
	return super(Control, self, captureEvent, event);
}

#pragma mark - Checkbox

static Image *_check;

/**
 * @fn Checkbox *Checkbox::initWithFrame(Checkbox *self, const SDL_Frame *frame, ControlStyle style)
 *
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
		
		$((View *) self->box, addSubview, (View *) self->check);
		$((View *) self, addSubview, (View *) self->box);
		
		if (self->control.style == ControlStyleDefault) {
			self->box->bevel = BevelTypeInset;

			self->box->view.backgroundColor = Colors.DimGray;

			self->box->view.frame.w = DEFAULT_CHECKBOX_SIZE;
			self->box->view.frame.h = DEFAULT_CHECKBOX_SIZE;
			
			self->box->view.padding.top = DEFAULT_CHECKBOX_PADDING;
			self->box->view.padding.right = DEFAULT_CHECKBOX_PADDING;
			self->box->view.padding.bottom = DEFAULT_CHECKBOX_PADDING;
			self->box->view.padding.left = DEFAULT_CHECKBOX_PADDING;
		}

		$((View *) self, sizeToFit);
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->render = render;
	
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	
	((CheckboxInterface *) clazz->interface)->initWithFrame = initWithFrame;
	
	_check = $(alloc(Image), initWithName, "check.png");
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_check);
}

Class _Checkbox = {
	.name = "Checkbox",
	.superclass = &_Control,
	.instanceSize = sizeof(Checkbox),
	.interfaceOffset = offsetof(Checkbox, interface),
	.interfaceSize = sizeof(CheckboxInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
