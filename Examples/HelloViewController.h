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

#ifndef _ObjectivelyMVC_HelloViewController_h_
#define _ObjectivelyMVC_HelloViewController_h_

#include <ObjectivelyMVC/ViewController.h>

/**
 * @file
 * 
 * @brief The HelloViewController.
 */

typedef struct HelloViewController HelloViewController;
typedef struct HelloViewControllerInterface HelloViewControllerInterface;

/**
 * @brief The HelloViewController.
 */
struct HelloViewController {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	ViewController viewController;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	HelloViewControllerInterface *interface;
};

/**
 * @brief The HelloViewController interface.
 */
struct HelloViewControllerInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewControllerInterface viewControllerInterface;
};

/**
 * @brief The HelloViewController Class.
 */
extern Class _HelloViewController;

#endif
