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

#include <ObjectivelyMVC/Control.h>

#define _Class _Control

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Control *this = (Control *) self;
	
	release(this->actions);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::didReceiveEvent(const View *, const SDL_Event *)
 */
static _Bool didReceiveEvent(const View *self, const SDL_Event *event) {
	
	if (super(View, self, didReceiveEvent, event)) {
		return true;
	}
	
	return ((Control *) self)->state & ControlStateFocused;
}

/**
 * @see View::render(View *, SDL_Renderer *)
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	super(View, self, render, renderer);
	
	Control *this = (Control *) self;
	
	const SDL_Rect frame = $(self, renderFrame);
	
	if (this->bevel == BevelTypeInset) {
		
		SetRenderDrawColor(renderer, Colors.Silver);
		
		SDL_Point points[3];
		
		points[0].x = frame.x + 2;
		points[0].y = frame.y + frame.h - 2;
		
		points[1].x = frame.x + frame.w - 2;
		points[1].y = frame.y + frame.h - 2;
		
		points[2].x = frame.x + frame.w - 2;
		points[2].y = frame.y + 1;
		
		SDL_RenderDrawLines(renderer, points, lengthof(points));
		
		SetRenderDrawColor(renderer, Colors.Charcoal);
		
		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 3;
		
		points[1].x = frame.x + 1;
		points[1].y = frame.y + 1;
		
		points[2].x = frame.x + frame.w - 2;
		points[2].y = frame.y + 1;
		
		SDL_RenderDrawLines(renderer, points, lengthof(points));
		
		SetRenderDrawColor(renderer, Colors.White);
		
	} else if (this->bevel == BevelTypeOutset) {
		
		SetRenderDrawColor(renderer, Colors.Charcoal);
		
		SDL_Point points[3];
		
		points[0].x = frame.x + 2;
		points[0].y = frame.y + frame.h - 2;
		
		points[1].x = frame.x + frame.w - 2;
		points[1].y = frame.y + frame.h - 2;
		
		points[2].x = frame.x + frame.w - 2;
		points[2].y = frame.y + 1;
		
		SDL_RenderDrawLines(renderer, points, lengthof(points));
		
		SetRenderDrawColor(renderer, Colors.Silver);
		
		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 3;
		
		points[1].x = frame.x + 1;
		points[1].y = frame.y + 1;
		
		points[2].x = frame.x + frame.w - 2;
		points[2].y = frame.y + 1;
		
		SDL_RenderDrawLines(renderer, points, lengthof(points));
		
		SetRenderDrawColor(renderer, Colors.White);
	}
	
	if (this->state & ControlStateFocused) {
		SetRenderDrawColor(renderer, Colors.Black);
		
		SDL_RenderDrawRect(renderer, &frame);
		
		SetRenderDrawColor(renderer, Colors.White);
	}
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {
	
	Control *this = (Control *) self;
	
	const ControlState state = this->state;
	
	if ($(this, captureEvent, event)) {
		
		Action *action = $(this, actionForEvent, event);
		if (action) {
			action->function(self, event, action->data);
		}
	}
	
	if (this->state != state) {
		$(this, stateDidChange);
	}
}

#pragma mark - Control

/**
 * @fn Action *Controll::actionForEvent(const Control *self, const SDL_Event *event)
 *
 * @memberof Control
 */
static Action *actionForEvent(const Control *self, const SDL_Event *event) {
	
	Array *actions = (Array *) self->actions;
	for (size_t i = 0; i < actions->count; i++) {
		
		Action *action = (Action *) $(actions, objectAtIndex, i);
		if (action->eventType == event->type) {
			return action;
		}
	}

	return NULL;
}

/**
 * @fn void Control::addActionForEventType(Control *self, SDL_EventType eventType, ActionFunction function, ident data)
 *
 * @memberof Control
 */
static void addActionForEventType(Control *self, SDL_EventType eventType, ActionFunction function, ident data) {
	
	Action *action = $(alloc(Action), initWithEventType, eventType, function, data);
	
	$(self->actions, addObject, action);
	
	release(action);
}

/**
 * @fn _Bool Control::captureEvent(Control *self, const SDL_Event *event)
 *
 * @memberof Control
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	return false;
}

/**
 * @fn _Bool Control::enabled(const Control *self)
 *
 * @memberof Control
 */
static _Bool enabled(const Control *self) {
	return (self->state & ControlStateDisabled) == 0;
}

/**
 * @fn _Bool Control::focused(const Control *self)
 *
 * @memberof Control
 */
static _Bool focused(const Control *self) {
	return (self->state & ControlStateFocused) == ControlStateFocused;
}

/**
 * @fn _Bool Control::highlighted(const Control *self)
 *
 * @memberof Control
 */
static _Bool highlighted(const Control *self) {
	return (self->state & ControlStateHighlighted) == ControlStateHighlighted;
}

/**
 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof Control
 */
static Control *initWithFrame(Control *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (Control *) super(View, self, initWithFrame, frame);
	if (self) {
		self->style = style;

		self->actions = $$(MutableArray, array);
		assert(self->actions);
		
		if (self->style == ControlStyleDefault) {
			
			if (self->view.frame.h == 0) {
				self->view.frame.h = DEFAULT_CONTROL_HEIGHT;
			}
			
			self->view.padding.top = DEFAULT_CONTROL_PADDING;
			self->view.padding.right = DEFAULT_CONTROL_PADDING;
			self->view.padding.bottom = DEFAULT_CONTROL_PADDING;
			self->view.padding.left = DEFAULT_CONTROL_PADDING;
		}
	}
	
	return self;
}

/**
 * @fn _Bool Control::selected(const Control *self)
 *
 * @memberof Control
 */
static _Bool selected(const Control *self) {
	return (self->state & ControlStateSelected) == ControlStateSelected;
}

/**
 * @fn void Control::stateDidChange(Control *self)
 *
 * @memberof Control
 */
static void stateDidChange(Control *self) {
	
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
	
	((ControlInterface *) clazz->interface)->actionForEvent = actionForEvent;
	((ControlInterface *) clazz->interface)->addActionForEventType = addActionForEventType;
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->interface)->enabled = enabled;
	((ControlInterface *) clazz->interface)->focused = focused;
	((ControlInterface *) clazz->interface)->highlighted = highlighted;
	((ControlInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ControlInterface *) clazz->interface)->selected = selected;
	((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;
}

Class _Control = {
	.name = "Control",
	.superclass = &_View,
	.instanceSize = sizeof(Control),
	.interfaceOffset = offsetof(Control, interface),
	.interfaceSize = sizeof(ControlInterface),
	.initialize = initialize,
};

#undef _Class
