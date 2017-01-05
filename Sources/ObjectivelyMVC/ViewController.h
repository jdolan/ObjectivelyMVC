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
 * @brief A ViewController manages a View and its descendants.
 */

/**
 * @defgroup ViewControllers View Controllers
 * @brief ViewControllers manage a View and its descendants.
 * @remarks In addition to managing a View hierarchy, ViewControllers themselves may be arranged in 
 * a hierarchical fashion. Typically, an application will employ a single primary ViewController to
 * manage the window via the WindowController. This primary ViewController may have child
 * ViewControllers, and can add or remove those children from the View hierarchy at any time by calling
 * ViewController::moveToParentViewController on the child.
 */

typedef struct ViewController ViewController;
typedef struct ViewControllerInterface ViewControllerInterface;

/**
 * @brief A ViewController manages a View and its descendants.
 * @extends Object
 * @ingroup ViewControllers
 */
struct ViewController {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
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
};

/**
 * @brief The ViewController type.
 */
struct ViewControllerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface parentInterface;
	
	/**
	 * @fn void ViewController::draw(ViewController *self, Renderer *renderer)
	 * @brief Draws this ViewController's View hierarchy.
	 * @param self The ViewController.
	 * @param renderer The Renderer with which to draw.
	 * @remarks This method is called from WindowController::render to draw the
	 * assigned ViewController's View hierarchy.
	 * @memberof ViewController
	 */
	void (*drawView)(ViewController *self, Renderer *renderer);
	
	/**
	 * @fn ViewController *ViewController::init(ViewController *self)
	 * @brief Initializes this ViewController.
	 * @param self The ViewController.
	 * @return The intialized ViewController, or `NULL` on error.
	 * @memberof ViewController
	 */
	ViewController *(*init)(ViewController *self);
	
	/**
	 * @fn void ViewController::loadView(ViewController *self)
	 * @brief Loads this ViewController's View.
	 * @param self The ViewController.
	 * @remarks Override this method to populate this ViewController's view.
	 * @memberof ViewController
	 */
	void (*loadView)(ViewController *self);

	/**
	 * @fn void ViewController::loadViewIfNeeded(ViewController *self)
	 * @brief Loads this ViewController's View if it is not already loaded.
	 * @param self The ViewController.
	 * @memberof ViewController
	 */
	void (*loadViewIfNeeded)(ViewController *self);
	
	/**
	 * @fn void ViewController::moveToParent(ViewController *self, ViewController *parentViewController)
	 * @brief Moves this ViewController to the specified parent.
	 * @param self The ViewController.
	 * @param parentViewController The parent ViewController, or `NULL`.
	 * @memberof ViewController
	 */
	void (*moveToParentViewController)(ViewController *self, ViewController *parentViewController);

	/**
	 * @fn void ViewController::renderDeviceDidReset(ViewController *self)
	 * @brief This method is invoked when the render context is invalidated.
	 * @param self The ViewController.
	 * @remarks Subclasses should override this method to recreate any texture resources or other
	 * OpenGL objects they own.
	 * @memberof ViewController
	 */
	void (*renderDeviceDidReset)(ViewController *self);
	
	/**
	 * @fn void ViewController:respondToEvent(ViewController *self, const SDL_Event *event)
	 * @brief Responds to the given event.
	 * @param self The ViewController.
	 * @param event The SDL_Event.
	 * @memberof ViewController
	 */
	void (*respondToEvent)(ViewController *self, const SDL_Event *event);
};

/**
 * @brief The ViewController Class.
 */
extern Class *_ViewController(void);

