//
//  Window.c
//  ObjectivelyMVC
//
//  Created by Jay Dolan on 8/24/16.
//  Copyright © 2016 https://github.com/jdolan. All rights reserved.
//

#include <assert.h>

#include <ObjectivelyMVC/Window.h>

#define _Class _Window

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	//..
	
	super(Object, self, dealloc);
}

#pragma mark - Window

/**
 * @fn Window *Window::init(Window *self)
 *
 * @memberof Window
 */
static Window *init(Window *self) {
	
	self = (Window *) super(Object, self, init);
	if (self) {
		
		//..
	}
	
	return self;
}

//..

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((WindowInterface *) clazz->interface)->init = init;
	
	//..
}

Class _Window = {
	.name = "Window",
	.superclass = &_Object,
	.instanceSize = sizeof(Window),
	.interfaceOffset = offsetof(Window, interface),
	.interfaceSize = sizeof(WindowInterface),
	.initialize = initialize,
};

#undef _Class

