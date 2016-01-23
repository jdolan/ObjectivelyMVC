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

	//..

	super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @brief ArrayEnumerator for draw.
 */
static _Bool drawChildViewControllers(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, drawView, (SDL_Renderer *) data); return false;
}

/**
 * @fn void ViewController::drawView(ViewController *self, SDL_Renderer *renderer)
 *
 * @memberof ViewController
 */
static void drawView(ViewController *self, SDL_Renderer *renderer) {
	
	assert(renderer);
		
	$(self->view, layoutSubviews);
	
	$(self->view, draw, renderer);
		
	$((Array *) self->childViewControllers, enumerateObjects, drawChildViewControllers, renderer);
}

/**
 * @fn ViewController *ViewController::init(ViewController *self)
 *
 * @memberof ViewController
 */
static ViewController *init(ViewController *self) {

	self = (ViewController *) super(Object, self, init);
	if (self) {
		self->childViewControllers = $$(MutableArray, array);
	}

	return self;
}

/**
 * @fn ViewController *ViewController::initRootViewController(ViewController *self, SDL_Window *window)
 *
 * @memberof ViewController
 */
static ViewController *initRootViewController(ViewController *self, SDL_Window *window) {
	
	self = $(self, init);
	if (self) {
		if (window) {
			self->window = window;
		} else {
			self->window = SDL_CreateWindow(__FILE__,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				0,
				0,
				SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
			);
		}
		assert(self->window);
		
		$(self, loadView);
		assert(self->view);
	}
	
	return self;
}

/**
 * @fn void ViewController:loadView(ViewController *self)
 *
 * @memberof ViewController
 */
static void loadView(ViewController *self) {
	
	SDL_Window *window = $(self, rootViewController)->window;
	assert(window);
	
	SDL_Rect frame = { .x = 0, .y = 0 };
	SDL_GetWindowSize(window, &frame.w, &frame.h);
	
	self->view = $(alloc(View), initWithFrame, &frame);
}

/**
 * @fn void ViewController::moveToParentViewController(ViewController *self, ViewController *parentViewController)
 *
 * @memberof ViewController
 */
static void moveToParentViewController(ViewController *self, ViewController *parentViewController) {
	
	if (self->parentViewController) {
		$(self->parentViewController->childViewControllers, removeObject, self);
	}
	
	self->parentViewController = parentViewController;
	
	if (self->parentViewController) {
		$(self->parentViewController->childViewControllers, addObject, self);
		
		if (self->view == NULL) {
			$(self, loadView);
		}
	}
}

/**
 * @fn _Bool ViewController::respondToEvent(ViewController *self, SDL_Event *event)
 *
 * @memberof ViewController
 */
static _Bool respondToEvent(ViewController *self, SDL_Event *event) {
	
	assert(event);
	
	Array *childViewControllers = (Array *) self->childViewControllers;
	for (size_t i = 0; i < childViewControllers->count; i++) {
		
		ViewController *child = $(childViewControllers, objectAtIndex, i);
		if ($(child, respondToEvent, event)) {
			return true;
		}
	}
	
	return $(self->view, respondToEvent, event);
}

/**
 * @fn ViewController *ViewController::rootViewController(const ViewController *self)
 *
 * @memberof ViewController
 */
static ViewController *rootViewController(const ViewController *self) {
	
	ViewController *viewController = (ViewController *) self;
	while (viewController) {
		if (viewController->window) {
			return viewController;
		}
		viewController = viewController->parentViewController;
	}
	
	assert(false);
}

/**
 * @fn void ViewController::viewDidAppear(ViewController *self)
 *
 * @memberof ViewController
 */
static void viewDidAppear(ViewController *self) {
	
}

/**
 * @fn void ViewController::viewDidDisappear(ViewController *self)
 *
 * @memberof ViewController
 */
static void viewDidDisappear(ViewController *self) {
	
}

/**
 * @fn void ViewController::viewWillAppear(ViewController *self)
 *
 * @memberof ViewController
 */
static void viewWillAppear(ViewController *self) {
	
}

/**
 * @fn void ViewController::viewWillDisappear(ViewController *self)
 *
 * @memberof ViewController
 */
static void viewWillDisappear(ViewController *self) {
	
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	ViewControllerInterface *viewController = (ViewControllerInterface *) clazz->interface;

	viewController->drawView = drawView;
	viewController->init = init;
	viewController->initRootViewController = initRootViewController;
	viewController->loadView = loadView;
	viewController->moveToParentViewController = moveToParentViewController;
	viewController->respondToEvent = respondToEvent;
	viewController->rootViewController = rootViewController;
	viewController->viewDidAppear = viewDidAppear;
	viewController->viewDidDisappear = viewDidDisappear;
	viewController->viewWillAppear = viewWillAppear;
	viewController->viewWillDisappear = viewWillDisappear;
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
