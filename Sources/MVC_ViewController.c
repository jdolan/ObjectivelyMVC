/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <assert.h>

#include "MVC_viewcontroller.h"

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
static MVC_ViewController *init(MVC_ViewController *self) {

	self = (MVC_ViewController *) super(Object, self, init);
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

	((MVC_ViewControllerInterface *) clazz->interface)->init = init;

	//..
}

Class __ViewController = {
	.name = "ViewController",
	.superclass = &__Object,
	.instanceSize = sizeof(MVC_ViewController),
	.interfaceOffset = offsetof(MVC_ViewController, interface),
	.interfaceSize = sizeof(MVC_ViewControllerInterface),
	.initialize = initialize,
};

#undef __class
