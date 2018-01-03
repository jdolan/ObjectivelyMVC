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

#include <ObjectivelyMVC/TableView.h>
#include <ObjectivelyMVC/ViewController.h>

/**
 * @file
 * @brief The DebugViewController type.
 */

typedef struct DebugViewController DebugViewController;
typedef struct DebugViewControllerInterface DebugViewControllerInterface;

/**
 * @brief The DebugViewController type.
 * @extends ViewController
 */
struct DebugViewController {

	/**
	 * @brief The superclass.
	 */
	ViewController viewController;

	/**
	 * @brief The interface.
	 * @protected
	 */
	DebugViewControllerInterface *interface;

	/**
	 * @brief The View to debug.
	 */
	const View *debug;

	/**
	 * @brief The StackView.
	 */
	StackView *stackView;

	/**
	 * @brief The View description.
	 */
	Text *description;

	/**
	 * @brief The CSS selectors matching the View.
	 */
	TableView *selectors;

	/**
	 * @brief The computed style of the View.
	 */
	TableView *computedStyle;
};

/**
 * @brief The DebugViewController interface.
 */
struct DebugViewControllerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewControllerInterface viewControllerInterface;

	/**
	 * @fn void DebugViewController::debug(DebugViewController *self, const View *view)
	 * @brief Debugs the given View.
	 * @param self The DebugViewController.
	 * @param view The View to debug.
	 * @memberof DebugViewController
	 */
	void (*debug)(DebugViewController *self, const View *view);

	/**
	 * @fn DebugViewController *DebugViewController::init(DebugViewController *self)
	 * @brief Initializes this DebugViewController.
	 * @param self The DebugViewController.
	 * @return The initialized DebugViewController, or `NULL` on error.
	 * @memberof DebugViewController
	 */
	DebugViewController *(*init)(DebugViewController *self);
};

/**
 * @fn Class *DebugViewController::_DebugViewController(void)
 * @brief The DebugViewController archetype.
 * @return The DebugViewController Class.
 * @memberof DebugViewController
 */
OBJECTIVELYMVC_EXPORT Class *_DebugViewController(void);
