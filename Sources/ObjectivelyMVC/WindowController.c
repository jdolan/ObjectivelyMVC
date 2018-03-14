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

#include <Objectively/String.h>

#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/WindowController.h>

#define _Class _WindowController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	WindowController *this = (WindowController *) self;

	release(this->debugViewController);
	release(this->renderer);
	release(this->viewController);

	super(Object, self, dealloc);
}

#pragma mark - WindowController

/**
 * @fn void WindowController::debug(WindowController *self)
 * @memberof WindowController
 */
static void debug(WindowController *self) {

	if (self->debugViewController) {

		ViewController *debugViewController = (ViewController *) self->debugViewController;

		if (self->hover) {
			$(self->debugViewController, debug, self->hover);
		}

		$(debugViewController->view, applyThemeIfNeeded, self->theme);
		$(debugViewController->view, layoutIfNeeded);
		$(debugViewController->view, draw, self->renderer);
	}
}

/**
 * @fn View *WindowController::eventTarget(const WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static View *eventTarget(const WindowController *self, const SDL_Event *event) {

	SDL_Point point;

	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		point = MakePoint(event->button.x, event->button.y);
	} else if (event->type == SDL_MOUSEMOTION) {
		point = MakePoint(event->motion.x, event->motion.y);
	} else if (event->type == SDL_MOUSEWHEEL) {
		SDL_GetMouseState(&point.x, &point.y);
	} else {
		return NULL;
	}

	return $(self->viewController->view, hitTest, &point);
}

/**
 * @fn View *WindowController::firstResponder(const WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static View *firstResponder(const WindowController *self, const SDL_Event *event) {
	return $$(View, firstResponder, self->window) ?: $(self, eventTarget, event);
}

/**
 * @fn WindowController *WindowController::initWithWindow(WindowController *self, SDL_Window *window)
 * @memberof WindowController
 */
static WindowController *initWithWindow(WindowController *self, SDL_Window *window) {

	self = (WindowController *) super(Object, self, init);
	if (self) {
		$(self, setWindow, window);
		$(self, setViewController, NULL);
		$(self, setTheme, NULL);
		$(self, setRenderer, NULL);
	}

	return self;
}

/**
 * @fn void WindowController::render(WindowController *self)
 * @memberof WindowController
 */
static void render(WindowController *self) {

	assert(self->renderer);

	$(self->renderer, beginFrame);

	$(self->viewController->view, applyThemeIfNeeded, self->theme);
	$(self->viewController->view, layoutIfNeeded);
	$(self->viewController->view, draw, self->renderer);

	$(self, debug);

	$(self->renderer, endFrame);
}

/**
 * @fn void WindowController::respondToEvent(WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static void respondToEvent(WindowController *self, const SDL_Event *event) {

	if (event->type == MVC_NOTIFICATION_EVENT) {
		$(self->viewController, handleNotification, &(const Notification) {
			.name = event->user.code,
			.sender = event->user.data1,
			.data = event->user.data2
		});
	} else {

		if (event->type == SDL_WINDOWEVENT) {
			switch (event->window.event) {
				case SDL_WINDOWEVENT_EXPOSED:
					$(self, setWindow, SDL_GL_GetCurrentWindow());
					$(self->renderer, renderDeviceDidReset);
					$(self->viewController->view, renderDeviceDidReset);
					break;
				case SDL_WINDOWEVENT_CLOSE:
					$(self->renderer, renderDeviceWillReset);
					$(self->viewController->view, renderDeviceWillReset);
					break;
				default:
					break;
			}
		} else if (event->type == SDL_MOUSEMOTION) {
			$(self, updateHover, event);
		}

		View *firstResponder = $(self, firstResponder, event);
		if (firstResponder) {

			switch (event->type) {
				case SDL_MOUSEMOTION:
					break;
				default: {
					String *desc = $((Object *) firstResponder, description);
					MVC_LogMessage(SDL_LOG_PRIORITY_DEBUG, "%d -> %s\n", event->type, desc->chars);
					release(desc);
				}
			}

			$(firstResponder, respondToEvent, event);
		}

		if (event->type == SDL_KEYUP) {
			if (event->key.keysym.sym == SDLK_d) {
				if (event->key.keysym.mod & KMOD_CTRL) {
					$(self, toggleDebugger);
				}
			}
		}
	}
}

/**
 * @fn void WindowController::setRenderer(WindowController *self, Renderer *renderer)
 * @memberof WindowController
 */
static void setRenderer(WindowController *self, Renderer *renderer) {

	if (self->renderer != renderer || self->renderer == NULL) {

		release(self->renderer);

		if (renderer) {
			self->renderer = retain(renderer);
		} else {
			self->renderer = $(alloc(Renderer), init);
		}

		$(self->viewController->view, renderDeviceDidReset);
	}
}

/**
 * @fn void WindowController::setTheme(WindowController *self, Theme *theme)
 * @memberof WindowController
 */
static void setTheme(WindowController *self, Theme *theme) {

	if (self->theme != theme || self->theme == NULL) {

		release(self->theme);

		if (theme) {
			self->theme = retain(theme);
		} else {
			self->theme = $(alloc(Theme), init);
		}
	}

	$(self->viewController->view, invalidateStyle);
}

/**
 * @fn void WindowController::setViewController(WindowController *self, ViewController *viewController)
 * @memberof WindowController
 */
static void setViewController(WindowController *self, ViewController *viewController) {

	if (self->viewController != viewController || self->viewController == NULL) {

		if (self->viewController) {
			$(self->viewController, viewWillDisappear);
			$(self->viewController->view, moveToWindow, NULL);
			$(self->viewController, viewDidDisappear);
		}

		release(self->viewController);

		if (viewController) {
			self->viewController = retain(viewController);
		} else {
			self->viewController = $(alloc(ViewController), init);
		}

		$(self->viewController, loadViewIfNeeded);

		$(self->viewController, viewWillAppear);
		$(self->viewController->view, moveToWindow, self->window);
		$(self->viewController, viewDidAppear);
	}
}

/**
 * @fn void WindowController::setWindow(WindowController *self, SDL_Window *window)
 * @memberof WindowController
 */
static void setWindow(WindowController *self, SDL_Window *window) {

	self->window = window;
	assert(self->window);

	const Uint32 flags = SDL_GetWindowFlags(self->window);
	assert(flags & SDL_WINDOW_OPENGL);

	self->hover = NULL;

	SDL_SetWindowData(self->window, "windowController", self);
	assert(SDL_GetWindowData(self->window, "windowController") == self);

	if (self->viewController) {
		$(self->viewController->view, moveToWindow, self->window);
	}

	if (self->debugViewController) {
		$(self->debugViewController->viewController.view, moveToWindow, self->window);
	}
}

/**
 * @fn void WindowController::toggleDebugger(WindowController *self)
 * @memberof WindowController
 */
static void toggleDebugger(WindowController *self) {

	if (self->debugViewController == NULL) {

		self->debugViewController = $(alloc(DebugViewController), init);
		assert(self->debugViewController);

		ViewController *debugViewController = (ViewController *) self->debugViewController;

		$(debugViewController, loadView);
		$(debugViewController, viewWillAppear);
		$(debugViewController->view, moveToWindow, self->window);
		$(debugViewController, viewDidAppear);
	} else {
		self->debugViewController = release(self->debugViewController);
	}
}

/**
 * @fn void WindowController::updateHover(WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static void updateHover(WindowController *self, const SDL_Event *event) {

	assert(event);
	assert(event->type == SDL_MOUSEMOTION);

	const SDL_Point point = MakePoint(event->motion.x, event->motion.y);
	View *hover = $(self->viewController->view, hitTest, &point);

	if (self->hover != hover) {

		if (self->hover && hover) {
			View *view = hover;
			while (view) {
				if ($(self->hover, isDescendantOfView, view)) {
					$(view, invalidateStyle);
					break;
				}
				view = view->superview;
			}
		} else {
			if (self->hover) {
				$(self->hover, invalidateStyle);
			} else {
				$(hover, invalidateStyle);
			}
		}

		release(self->hover);
		self->hover = hover ? retain(hover) : NULL;
	}
}

/**
 * @fn WindowController *WindowController::windowController(SDL_Window *window)
 * @memberof WindowController
 */
static WindowController *windowController(SDL_Window *window) {

	assert(window);

	return SDL_GetWindowData(window, "windowController");
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((WindowControllerInterface *) clazz->interface)->debug = debug;
	((WindowControllerInterface *) clazz->interface)->eventTarget = eventTarget;
	((WindowControllerInterface *) clazz->interface)->firstResponder = firstResponder;
	((WindowControllerInterface *) clazz->interface)->initWithWindow = initWithWindow;
	((WindowControllerInterface *) clazz->interface)->render = render;
	((WindowControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((WindowControllerInterface *) clazz->interface)->setRenderer = setRenderer;
	((WindowControllerInterface *) clazz->interface)->setTheme = setTheme;
	((WindowControllerInterface *) clazz->interface)->setViewController = setViewController;
	((WindowControllerInterface *) clazz->interface)->setWindow = setWindow;
	((WindowControllerInterface *) clazz->interface)->toggleDebugger = toggleDebugger;
	((WindowControllerInterface *) clazz->interface)->updateHover = updateHover;
	((WindowControllerInterface *) clazz->interface)->windowController = windowController;
}

/**
 * @fn Class *WindowController::_WindowController(void)
 * @memberof WindowController
 */
Class *_WindowController(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "WindowController",
			.superclass = _Object(),
			.instanceSize = sizeof(WindowController),
			.interfaceOffset = offsetof(WindowController, interface),
			.interfaceSize = sizeof(WindowControllerInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
