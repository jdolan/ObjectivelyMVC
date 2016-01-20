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
	
	release(this->label);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static _Bool respondToEvent(View *self, const SDL_Event *event) {
	
	Button *this = (Button *) self;
	
	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		if (event->type == SDL_MOUSEBUTTONUP) {
			this->control.state &= ~ControlStateSelected;
			if (this->type == ButtonTypeDefault) {
				this->control.bevel = BevelTypeOutset;
			}
		}
		const SDL_Point point = { .x = event->button.x, .y = event->button.y };
		if ($(self, containsPoint, &point)) {
			
			if (event->type == SDL_MOUSEBUTTONDOWN) {
				this->control.state |= ControlStateSelected;
				if (this->type == ButtonTypeDefault) {
					this->control.bevel = BevelTypeInset;
				}
			}
			
			const Action *action = $((Control *) self, actionForEvent, event);
			if (action) {
				action->function(self, event, action->data);
			}
			
			return true;
		}
	} else  if (event->type == SDL_MOUSEMOTION) {
		const SDL_Point point = { .x = event->motion.x, .y = event->motion.y };
		if ($(self, containsPoint, &point)) {
			this->control.state |= ControlStateHighlighted;
		} else {
			this->control.state &= ~ControlStateHighlighted;
		}
	}
	
	return super(View, self, respondToEvent, event);
}

#pragma mark - Button

/**
 * @fn Button *Button::initWithType(Button *self, ButtonType type)
 *
 * @memberof Button
 */
static Button *initWithType(Button *self, ButtonType type) {
	
	self = (Button *) super(Control, self, initWithFrame, NULL);
	if (self) {
		
		self->type = type;
		
		self->label = $(alloc(Label), initWithText, NULL, NULL);
		assert(self->label);
		
		View *this = (View *) self;
		
		$(this, addSubview, (View *) self->label);
		
		if (self->type == ButtonTypeDefault) {
			self->control.bevel = BevelTypeOutset;
			this->margin.top = this->margin.bottom = 20;
			this->margin.left = this->margin.right = 20;
			this->padding.top = this->padding.bottom = 10;
			this->padding.left = this->padding.right = 10;
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

	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

	((ButtonInterface *) clazz->interface)->initWithType = initWithType;
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
