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

#pragma once

#include <Objectively/Object.h>

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
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	WindowControllerInterface *interface;

	/**
	 * @brief The renderer.
	 */
	Renderer *renderer;

	/**
	 * @brief The ViewController.
	 */
	ViewController *viewController;

	/**
	 * @brief The Window.
	 */
	SDL_Window *window;
};

/**
 * @brief The WindowController interface.
 */
struct WindowControllerInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @fn WindowController *WindowController::initWithWindow(WindowController *self, SDL_Window *window)
	 * @brief Initializes this WindowController with the given window.
	 * @param self The WindowController.
	 * @param window The window.
	 * @return The initialized WindowController, or `NULL` on error.
	 * @remarks The window must be accelerated (`SDL_WINDOW_OPENGL`). See `SDL_CreateWindow`.
	 * @memberof WindowController
	 */
	WindowController *(*initWithWindow)(WindowController *self, SDL_Window *window);

	/**
	 * @fn void WindowController::setViewController(WindowController *self, ViewController *viewController)
	 * @brief Presents the specified ViewController.
	 * @param self The WindowController.
	 * @param viewController The ViewController.
	 * @memberof WindowController
	 */
	void (*setViewController)(WindowController *self, ViewController *viewController);

	/**
	 * @fn void WindowController::render(WindowController *self)
	 * @brief Renders the ViewController's View.
	 * @param self The WindowController.
	 * @remarks Your application should call this method once per frame to render the View hierarchy.
	 * @memberof WindowController
	 */
	void (*render)(WindowController *self);

	/**
	 * @fn void WindowController:respondToEvent(WindowController *self, const SDL_Event *event)
	 * @brief Responds to the given event.
	 * @param self The WindowController.
	 * @param event The SDL_Event.
	 * @memberof WindowController
	 */
	void (*respondToEvent)(WindowController * self, const SDL_Event *event);
};

/**
 * @brief The WindowController Class.
 */
extern Class _WindowController;
