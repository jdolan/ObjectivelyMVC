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

#include <ObjectivelyMVC/Action.h>

#define _Class _Action

#pragma mark - Action

/**
 * @fn Action *Action::initWithEventType(Action *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data)
 * @memberof Action
 */
static Action *initWithEventType(Action *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data) {

	self = (Action *) super(Object, self, init);
	if (self) {
		self->eventType = eventType;
		assert(self->eventType);
		
		self->function = function;
		assert(self->function);

		self->sender = sender;
		self->data = data;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ActionInterface *) clazz->def->interface)->initWithEventType = initWithEventType;
}

Class *_Action(void) {
	static Class clazz;
	
	do_once({
		clazz.name = "Action";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Action);
		clazz.interfaceOffset = offsetof(Action, interface);
		clazz.interfaceSize = sizeof(ActionInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
