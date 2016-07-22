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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/View.h>

/**
 * @file
 *
 * @brief ViewController implementation.
 */

typedef struct ViewController ViewController;
typedef struct ViewControllerInterface ViewControllerInterface;

/**
 * @brief The ViewController type.
 *
 * @extends Object
 */
struct ViewController {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	ViewControllerInterface *interface;

	/**
	 * @brief The main view.
	 */
	View *view;

	/**
	 * @brief The parent view controller.
	 */
	ViewController *parentViewController;

	/**
	 * @brief The child view controllers.
	 */
	MutableArray *childViewControllers;
	
	/**
	 * @brief The window.
	 */
	SDL_Window *window;
};

/**
 * @brief The ViewController type.
 */
struct ViewControllerInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;
	
	/**
	 * @fn void ViewController::draw(ViewController *self, SDL_Renderer *renderer)
	 *
	 * @brief Draws this controller's View.
	 *
	 * @param renderer The SDL_Renderer with which to draw.
	 *
	 * @remarks Call this method on a root ViewController to draw its View hierarchy.
	 *
	 * @memberof ViewController
	 */
	void (*drawView)(ViewController *self, SDL_Renderer *renderer);
	
	/**
	 * @fn ViewController *ViewController::init(ViewController *self)
	 *
	 * @brief Initializes this controller.
	 *
	 * @return The intialized ViewController, or `NULL` on error.
	 *
	 * @memberof ViewController
	 */
	ViewController *(*init)(ViewController *self);
	
	/**
	 * @fn ViewController *ViewController::initRootViewController(ViewController *self, SDL_Window *window)
	 *
	 * @brief Initializes this controller as the root ViewController for the given window.
	 *
	 * @param window The window, or `NULL` to have a fullscreen window created.
	 *
	 * @return The initialized ViewController, or `NULL` on error.
	 *
	 * @memberof ViewController
	 */
	ViewController *(*initRootViewController)(ViewController *self, SDL_Window *window);
	
	/**
	 * @fn void ViewController::loadView(ViewController *self)
	 *
	 * @brief Loads this controller's View.
	 *
	 * @remarks Override this method to populate this controller's view.
	 *
	 * @memberof ViewController
	 */
	void (*loadView)(ViewController *self);
	
	/**
	 * @fn void ViewController::moveToParent(ViewController *self, ViewController *parentViewController)
	 *
	 * @brief Moves this ViewController to the specified parent.
	 *
	 * @param parentViewController The parent ViewController, or `NULL`.
	 *
	 * @memberof ViewController
	 */
	void (*moveToParentViewController)(ViewController *self, ViewController *parentViewController);
	
	/**
	 * @fn _Bool ViewController:respondToEvent(ViewController *self, const SDL_Event *event)
	 *
	 * @brief Responds to the given event.
	 *
	 * @param event The SDL_Event.
	 *
	 * @memberof ViewController
	 */
	void (*respondToEvent)(ViewController *self, const SDL_Event *event);
	
	/**
	 * @fn ViewController *ViewController::rootViewController(const ViewController *self)
	 *
	 * @return The root ViewController of this ViewController.
	 *
	 * @memberof ViewController
	 */
	ViewController *(*rootViewController)(const ViewController *self);

	/**
	 * @fn void ViewController::viewDidAppear(ViewController *self)
	 *
	 * @brief Called after each time this ViewController's View appears.
	 *
	 * @memberof ViewController
	 */
	void (*viewDidAppear)(ViewController *self);
	
	/**
	 * @fn void ViewController::viewDidDisappear(ViewController *self)
	 *
	 * @brief Called after each time this ViewController's View disappears.
	 *
	 * @memberof ViewController
	 */
	void (*viewDidDisappear)(ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be shown.
	 *
	 * @remark Subclasses may implement this method to prepare the
	 * view for rendering.
	 */
	void (*viewWillAppear)(ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be hidden.
	 *
	 * @remark Subclasses may implement this method to free resources
	 * allocated in `viewWillAppear`.
	 */
	void (*viewWillDisappear)(ViewController *self);	
};

/**
 * @brief The ViewController Class.
 */
extern Class _ViewController;
