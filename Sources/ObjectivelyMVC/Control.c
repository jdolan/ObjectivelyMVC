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
 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame)
 *
 * @memberof Control
 */
static Control *initWithFrame(Control *self, const SDL_Rect *frame) {
	
	self = (Control *) super(View, self, initWithFrame, frame);
	if (self) {
		self->actions = $$(MutableArray, array);
		assert(self->actions);
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

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	ObjectInterface *object = (ObjectInterface *) clazz->interface;
	
	object->dealloc = dealloc;

	ControlInterface *control = (ControlInterface *) clazz->interface;
	
	control->actionForEvent = actionForEvent;
	control->addActionForEventType = addActionForEventType;
	control->enabled = enabled;
	control->focused = focused;
	control->highlighted = highlighted;
	control->initWithFrame = initWithFrame;
	control->selected = selected;
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
