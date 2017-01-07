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

#include <ObjectivelyMVC/ViewController.h>

#define _Class _ViewController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ViewController *this = (ViewController *) self;
	
	release(this->childViewControllers);
	release(this->view);

	super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @fn void ViewController::drawView(ViewController *self, Renderer *renderer)
 * @memberof ViewController
 */
static void drawView(ViewController *self, Renderer *renderer) {
	
	assert(renderer);

	$(self, loadViewIfNeeded);

	assert(self->view);

	$(self->view, layoutIfNeeded);

	$(self->view, draw, renderer);
}

/**
 * @fn ViewController *ViewController::init(ViewController *self)
 * @memberof ViewController
 */
static ViewController *init(ViewController *self) {

	self = (ViewController *) super(Object, self, init);
	if (self) {
		self->childViewControllers = $$(MutableArray, array);
		assert(self->childViewControllers);
	}

	return self;
}

/**
 * @fn void ViewController:loadView(ViewController *self)
 * @memberof ViewController
 */
static void loadView(ViewController *self) {

	self->view = $(alloc(View), initWithFrame, NULL);
	assert(self->view);

	self->view->autoresizingMask = ViewAutoresizingFill;

	SDL_Window *window = $(self->view, window);
	assert(window);

	SDL_Size size;
	SDL_GetWindowSize(window, &size.w, &size.h);

	$(self->view, resize, &size);
}

/**
 * @fn void ViewController::loadViewIfNeeded(ViewController *self)
 * @memberof ViewController
 */
static void loadViewIfNeeded(ViewController *self) {

	if (self->view == NULL) {
		$(self, loadView);
	}

	assert(self->view);
}

/**
 * @fn void ViewController::moveToParentViewController(ViewController *self, ViewController *parentViewController)
 * @memberof ViewController
 */
static void moveToParentViewController(ViewController *self, ViewController *parentViewController) {

	$(self, loadViewIfNeeded);

	$(self->view, removeFromSuperview);
	
	if (self->parentViewController) {
		$(self->parentViewController->childViewControllers, removeObject, self);
	}

	self->parentViewController = parentViewController;

	if (self->parentViewController) {
		$(self->parentViewController->childViewControllers, addObject, self);
		$(self->parentViewController->view, addSubview, self->view);

		$(self->view, updateBindings);
	}
}

/**
 * @fn void ViewController::renderDeviceDidReset(ViewController *self)
 * @memberof ViewController
 */
static void renderDeviceDidReset(ViewController *self) {

	if (self->view) {
		$(self->view, renderDeviceDidReset);
	}
}

/**
 * @fn void ViewController::respondToEvent(ViewController *self, const SDL_Event *event)
 * @memberof ViewController
 */
static void respondToEvent(ViewController *self, const SDL_Event *event) {

	if (self->view) {
		$(self->view, respondToEvent, event);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewControllerInterface *) clazz->def->interface)->drawView = drawView;
	((ViewControllerInterface *) clazz->def->interface)->init = init;
	((ViewControllerInterface *) clazz->def->interface)->loadView = loadView;
	((ViewControllerInterface *) clazz->def->interface)->loadViewIfNeeded = loadViewIfNeeded;
	((ViewControllerInterface *) clazz->def->interface)->moveToParentViewController = moveToParentViewController;
	((ViewControllerInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((ViewControllerInterface *) clazz->def->interface)->respondToEvent = respondToEvent;
}

/**
 * @fn Class *ViewController::_ViewController(void)
 * @memberof ViewController
 */
Class *_ViewController(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "ViewController";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(ViewController);
		clazz.interfaceOffset = offsetof(ViewController, interface);
		clazz.interfaceSize = sizeof(ViewControllerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
