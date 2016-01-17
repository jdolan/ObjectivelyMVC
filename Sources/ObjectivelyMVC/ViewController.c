/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <assert.h>

#include <ObjectivelyMVC/ViewController.h>

#define _Class _ViewController

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	//..

	super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @
 */
static ViewController *init(ViewController *self) {

	self = (ViewController *) super(Object, self, init);
	if (self) {

		//..
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewControllerInterface *) clazz->interface)->init = init;
}

Class _ViewController = {
	.name = "ViewController",
	.superclass = &_Object,
	.instanceSize = sizeof(ViewController),
	.interfaceOffset = offsetof(ViewController, interface),
	.interfaceSize = sizeof(ViewControllerInterface),
	.initialize = initialize,
};

#undef _Class
