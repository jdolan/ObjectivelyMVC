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

#include <ObjectivelyMVC/ViewController.h>

/**
 * @file
 * @brief Navigation controllers arrange their child controllers as a stack to facilitate
 * navigation between them.
 */

typedef struct NavigationViewController NavigationViewController;
typedef struct NavigationViewControllerInterface NavigationViewControllerInterface;

/**
 * @brief Navigation controllers arrange their child controllers as a stack to facilitate
 * navigation between them.
 * @extends ViewController
 * @ingroup ViewControllers
 */
struct NavigationViewController {
	
	/**
	 * @brief The superclass.
	 */
	ViewController viewController;
	
	/**
	 * @brief The interface.
	 * @protected
	 */
	NavigationViewControllerInterface *interface;
};

/**
 * @brief The NavigationViewController interface.
 */
struct NavigationViewControllerInterface {
	
	/**
	 * @brief The superclass interface.
	 */
	ViewControllerInterface viewControllerInterface;
	
	/**
	 * @fn NavigationViewController *NavigationViewController::init(NavigationViewController *self)
	 * @brief Initializes this NavigationViewController.
	 * @param self The NavigationViewController.
	 * @return The initialized NavigationViewController, or `NULL` on error.
	 * @memberof NavigationViewController
	 */
	NavigationViewController *(*init)(NavigationViewController *self);

	/**
	 * @fn void NavigationViewController::pushViewController(NavigationViewController *self, ViewController *viewController)
	 * @brief Pushes the specified ViewController to the top of the stack.
	 * @param self The NavigationViewController.
	 * @param viewController The ViewController.
	 * @memberof NavigationViewController
	 */
	void (*pushViewController)(NavigationViewController *self, ViewController *viewController);

	/**
	 * @fn void NavigationViewController::popToRootViewController(NavigationViewController *self)
	 * @brief Pops all ViewControllers on the stack except for the root ViewController.
	 * @param self The NavigationViewController.
	 * @memberof NavigationViewController
	 */
	void (*popToRootViewController)(NavigationViewController *self);

	/**
	 * @fn void NavigationViewController::popToViewController(NavigationViewController *self, const ViewController *viewController)
	 * @brief Pops the top ViewController from the stack until the specified ViewController is visible.
	 * @param self The NavigationViewController.
	 * @param viewController The ViewController to remain at the top of the stack.
	 * @memberof NavigationViewController
	 */
	void (*popToViewController)(NavigationViewController *self, const ViewController *viewController);

	/**
	 * @fn void NavigationViewController::popViewController(NavigationViewController *self)
	 * @brief Pops the top ViewController from the stack.
	 * @param self The NavigationViewController.
	 * @memberof NavigationViewController
	 */
	void (*popViewController)(NavigationViewController *self);

	/**
	 * @fn ViewController NavigationViewController::rootViewController(const NavigationViewController *self)
	 * @param self The NavigationViewController.
	 * @return The root ViewController on the stack.
	 * @memberof NavigationViewController
	 */
	ViewController *(*rootViewController)(const NavigationViewController *self);

	/**
	 * @fn ViewController NavigationViewController::topViewController(const NavigationViewController *self)
	 * @param self The NavigationViewController.
	 * @return The top ViewController on the stack.
	 * @memberof NavigationViewController
	 */
	ViewController *(*topViewController)(const NavigationViewController *self);
};

/**
 * @fn Class *NavigationViewController::_NavigationViewController(void)
 * @brief The NavigationViewController archetype.
 * @return The NavigationViewController Class.
 * @memberof NavigationViewController
 */
OBJECTIVELYMVC_EXPORT Class *_NavigationViewController(void);

