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
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {
	
	$(self, sizeToFit);
	
	super(View, self, layoutSubviews);
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static _Bool respondToEvent(View *self, const SDL_Event *event) {
	
	Control *this = (Control *) self;
	
	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		if (event->type == SDL_MOUSEBUTTONUP) {
			this->state &= ~ControlStateSelected;
			if (this->style == ControlStyleDefault) {
				this->bevel = BevelTypeOutset;
			}
		}
		const SDL_Point point = { .x = event->button.x, .y = event->button.y };
		if ($(self, containsPoint, &point)) {
			
			if (event->type == SDL_MOUSEBUTTONDOWN) {
				this->state |= ControlStateSelected;
				if (this->style == ControlStyleDefault) {
					this->bevel = BevelTypeInset;
				}
			}
			
			const Action *action = $(this, actionForEvent, event);
			if (action) {
				action->function(self, event, action->data);
			}
			
			return true;
		}
	} else  if (event->type == SDL_MOUSEMOTION) {
		const SDL_Point point = { .x = event->motion.x, .y = event->motion.y };
		if ($(self, containsPoint, &point)) {
			this->state |= ControlStateHighlighted;
		} else {
			this->state &= ~ControlStateHighlighted;
		}
	}
	
	return super(View, self, respondToEvent, event);
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
		
		self->title = $(alloc(Label), initWithText, NULL, NULL);
		assert(self->title);
		
		$((View *) self, addSubview, (View *) self->title);
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = BevelTypeOutset;
			
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

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

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
