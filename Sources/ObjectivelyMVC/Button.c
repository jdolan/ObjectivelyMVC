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

#include <ObjectivelyMVC/Button.h>

#define _Class _Button

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Button *this = (Button *) self;
	
	release(this->title);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Button *this = (Button *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("title", InletTypeView, &this->title, NULL)
	);

	$(self, bind, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Button *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	SDL_Size size = super(View, self, sizeThatFits);

	const Button *this = (Button *) self;

	if (this->control.style == ControlStyleDefault) {
		size.w = max(size.w, DEFAULT_BUTTON_MIN_WIDTH);
	}

	return size;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		
		if (event->type == SDL_MOUSEBUTTONUP) {
			self->state &= ~ControlStateHighlighted;
			if (self->style == ControlStyleDefault) {
				self->bevel = ControlBevelTypeOutset;
			}
		}
		
		if ($((View *) self, didReceiveEvent, event)) {
			if (event->type == SDL_MOUSEBUTTONDOWN) {
				self->state |= ControlStateHighlighted;
				if (self->style == ControlStyleDefault) {
					self->bevel = ControlBevelTypeInset;
				}
			}
			return true;
		}
	}
	
	return super(Control, self, captureEvent, event);
}

#pragma mark - Button

/**
 * @fn Button *Button::initWithFrame(Button *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof Button
 */
static Button *initWithFrame(Button *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (Button *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		
		self->title = $alloc(Text, initWithText, NULL, NULL);
		assert(self->title);
		
		$((View *) self, addSubview, (View *) self->title);
		self->title->view.alignment = ViewAlignmentMiddleCenter;

		self->control.view.autoresizingMask = ViewAutoresizingContain;
		self->control.view.clipsSubviews = true;

		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = ControlBevelTypeOutset;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_BUTTON_MIN_WIDTH;
			}
		}
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
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

	((ButtonInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _Button = {
	.name = "Button",
	.superclass = &_Control,
	.instanceSize = sizeof(Button),
	.interfaceOffset = offsetof(Button, interface),
	.interfaceSize = sizeof(ButtonInterface),
	.initialize = initialize,
};

#undef _Class
