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

#ifndef _ObjectivelyMVC_ViewController_h_
#define _ObjectivelyMVC_ViewController_h_

#include <Objectively/MutableArray.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/View.h>

typedef struct ViewController ViewController;
typedef struct ViewControllerInterface ViewControllerInterface;

/**
 * @brief The ViewController type.
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
	 * @brief The title.
	 */
	String *title;

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
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;

	/**
	 * @brief Initializes this view controller.
	 *
	 * @return The initialized view controller, or NULL on error.
	 */
	ViewController *(*init)(ViewController *self);

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

#endif
