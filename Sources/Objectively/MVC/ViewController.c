/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <assert.h>

#include <Objectively/MVC/ViewController.h>

#define __class __ViewController

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	//..

	super(Object, self, dealloc);
}

#pragma mark - ViewControllerInterface

/**
 * @see ViewControllerInterface::init(ViewController *)
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
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewControllerInterface *) clazz->interface)->init = init;

	//..
}

Class __ViewController = {
	.name = "ViewController",
	.superclass = &__Object,
	.instanceSize = sizeof(ViewController),
	.interfaceOffset = offsetof(ViewController, interface),
	.interfaceSize = sizeof(ViewControllerInterface),
	.initialize = initialize,
};

#undef __class
