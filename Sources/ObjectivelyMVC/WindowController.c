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

#include <ObjectivelyMVC/WindowController.h>

#define _Class _WindowController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	WindowController *this = (WindowController *) self;

	release(this->viewController);
	
	super(Object, self, dealloc);
}

#pragma mark - WindowController

/**
 * @fn WindowController *WindowController::initWithWindow(WindowController *self, SDL_Window *window)
 *
 * @memberof WindowController
 */
static WindowController *initWithWindow(WindowController *self, SDL_Window *window) {
	
	self = (WindowController *) super(Object, self, init);
	if (self) {
		
		self->window = window;
		assert(window);

		Uint32 flags = SDL_GetWindowFlags(self->window);
		assert(flags & SDL_WINDOW_OPENGL);
	}
	
	return self;
}

/**
 * @fn void WindowController::setViewController(WindowController *self, ViewController *viewController)
 *
 * @memberof WindowController
 */
static void setViewController(WindowController *self, ViewController *viewController) {

	if (self->viewController != viewController) {

		if (self->viewController) {
			$(self->viewController, viewWillDisappear);

			release(self->viewController);
		}

		if (viewController) {
			self->viewController = retain(viewController);

			$(self->viewController, loadViewIfNeeded);

			$(self->viewController, viewWillAppear);
		} else {
			self->viewController = NULL;
		}
	}
}

/**
 * @fn void WindowController::render(WindowController *self, SDL_Renderer *renderer)
 *
 * @memberof WindowController
 */
static void render(WindowController *self, SDL_Renderer *renderer) {

	if (self->viewController) {
		$(self->viewController, drawView, renderer);
	}
}

/**
 * @fn void WindowController:respondToEvent(WindowController *self, const SDL_Event *event)
 *
 * @memberof WindowController
 */
static void respondToEvent(WindowController *self, const SDL_Event *event) {

	if (self->viewController) {
		$(self->viewController, respondToEvent, event);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((WindowControllerInterface *) clazz->interface)->initWithWindow = initWithWindow;
	((WindowControllerInterface *) clazz->interface)->render = render;
	((WindowControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((WindowControllerInterface *) clazz->interface)->setViewController = setViewController;
}

Class _WindowController = {
	.name = "WindowController",
	.superclass = &_Object,
	.instanceSize = sizeof(WindowController),
	.interfaceOffset = offsetof(WindowController, interface),
	.interfaceSize = sizeof(WindowControllerInterface),
	.initialize = initialize,
};

#undef _Class
