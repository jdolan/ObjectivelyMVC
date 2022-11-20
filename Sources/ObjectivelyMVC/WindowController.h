/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#pragma once

#include <Objectively/Object.h>

#include <ObjectivelyMVC/DebugViewController.h>
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/ViewController.h>

/**
 * @file
 * @brief A WindowController manages a ViewController and its descendants within an SDL_Window.
 */

typedef struct WindowController WindowController;
typedef struct WindowControllerInterface WindowControllerInterface;

/**
 * @brief A WindowController manages a ViewController and its descendants within an SDL_Window.
 * @extends Object
 * @ingroup ViewControllers
 */
struct WindowController {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	WindowControllerInterface *interface;

	/**
	 * @brief The DebugViewController.
	 */
	DebugViewController *debugViewController;

	/**
	 * @brief The Renderer.
	 */
	Renderer *renderer;

	/**
	 * @brief The Theme.
	 */
	Theme *theme;

	/**
	 * @brief The ViewController.
	 */
	ViewController *viewController;

	/**
	 * @brief The window.
	 */
	SDL_Window *window;
};

/**
 * @brief The WindowController interface.
 */
struct WindowControllerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @private
	 * @fn void WindowController::debug(WindowController *self)
	 * @brief Debugs the current frame.
	 * @param self The WindowController.
	 * @memberof WindowController
	 */
	void (*debug)(WindowController *self);

	/**
	 * @fn View *WindowController::eventTarget(const WindowController *self, const SDL_Event *event)
	 * @param self The WindowController.
	 * @param event The event.
	 * @return The View that was targeted by the given event.
	 * @see View::hitTest(const View *, const SDL_Point *)
	 * @memberof WindowController
	 */
	View *(*eventTarget)(const WindowController *self, const SDL_Event *event);

	/**
	 * @fn View *WindowController::firstResponder(const WindowController *self, const SDL_Event *event)
	 * @param self The WindowController.
	 * @param event The event.
	 * @return The first responder for the given event.
	 * @see MVC_FirstResponder(SDL_Window *)
	 * @memberof WindowController
	 */
	View *(*firstResponder)(const WindowController *self, const SDL_Event *event);

	/**
	 * @fn WindowController *WindowController::initWithWindow(WindowController *self, SDL_Window *window)
	 * @brief Initializes this WindowController with the given window.
	 * @param self The WindowController.
	 * @param window The window.
	 * @return The initialized WindowController, or `NULL` on error.
	 * @memberof WindowController
	 */
	WindowController *(*initWithWindow)(WindowController *self, SDL_Window *window);

	/**
	 * @fn void WindowController::render(WindowController *self)
	 * @brief Renders the ViewController's View.
	 * @param self The WindowController.
	 * @remarks Your application should call this method once per frame to render the View hierarchy.
	 * @memberof WindowController
	 */
	void (*render)(WindowController *self);

	/**
	 * @fn void WindowController::respondToEvent(WindowController *self, const SDL_Event *event)
	 * @brief Responds to the given event.
	 * @param self The WindowController.
	 * @param event The event.
	 * @remarks Your application should call this method for each event that the View hierarchy is
	 * expected to respond to. The event will be dispatched to WindowController::firstResponder,
	 * which is the first of:
	 *  * The View that has claimed first responder status for the window via
	 *    View::becomeFirstResponder
	 *  * The inner-most descendant in the View hierarchy that received the event, according to
	 *    View::hitTest
	 *  * This WindowController's ViewController
	 * @remarks By default, the event is passed up the View hierarchy by View::respondToEvent.
	 * Subclasses of View, such as Control, may stop event propagation if an event has been
	 * adequately responded to.
	 * @memberof WindowController
	 */
	void (*respondToEvent)(WindowController * self, const SDL_Event *event);

	/**
	 * @fn void WindowController::setRenderer(WindowController *self, Renderer *renderer)
	 * @brief Sets this WindowController's Renderer.
	 * @param self The WindowController.
	 * @param renderer The Renderer.
	 * @memberof WindowController
	 */
	void (*setRenderer)(WindowController *self, Renderer *renderer);

	/**
	 * @fn void WindowController::setTheme(WindowController *self, Theme *theme)
	 * @brief Sets this WindowController's Theme.
	 * @param self The WindowController.
	 * @param theme The Theme.
	 * @memberof WindowController
	 */
	void (*setTheme)(WindowController *self, Theme *theme);

	/**
	 * @fn void WindowController::setViewController(WindowController *self, ViewController *viewController)
	 * @brief Sets this WindowController's ViewController.
	 * @param self The WindowController.
	 * @param viewController The ViewController.
	 * @memberof WindowController
	 */
	void (*setViewController)(WindowController *self, ViewController *viewController);

	/**
	 * @fn void WindowController::setWindow(WindowController *self, SDL_Window *window)
	 * @brief Sets this WindowController's window.
	 * @param self The WindowController.
	 * @param window The window.
	 * @memberof WindowController
	 */
	void (*setWindow)(WindowController *self, SDL_Window *window);

	/**
	 * @fn void WindowController::toggleDebugger(WindowController *self)
	 * @brief Toggles the debugger tools.
	 * @param self The WindowViewController.
	 * @memberof WindowController
	 */
	void (*toggleDebugger)(WindowController *self);

	/**
	 * @static
	 * @fn WindowController *WindowController::windowController(SDL_Window *window)
	 * @param window The window.
	 * @return The WindowController bound to the given window.
	 * @memberof WindowController
	 */
	WindowController *(*windowController)(SDL_Window *window);
};

/**
 * @fn Class *WindowController::_WindowController(void)
 * @brief The WindowController archetype.
 * @return The WindowController Class.
 * @memberof WindowController
 */
OBJECTIVELYMVC_EXPORT Class *_WindowController(void);
