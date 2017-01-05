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
	 * @brief The superclass.
	 *
	 * @private
	 */
	ViewController viewController;

	/**
	 * @brief The interface.
	 *
	 * @private
	 */
	HelloViewControllerInterface *interface;

	/**
	 * @brief A Panel.
	 */
	Panel *panel;

	/**
	 * @brief A Button
	 */
	Button *button;

	/**
	 * @brief A TextView.
	 */
	TextView *textView;

	/**
	 * @brief A Checkbox.
	 */
	Checkbox *checkbox;

	/**
	 * @brief A Select.
	 */
	Select *select;

	/**
	 * @brief A Slider.
	 */
	Slider *slider;

	/**
	 * @brief A TableView.
	 */
	TableView *tableView;

	/**
	 * @brief A CollectionView.
	 */
	CollectionView *collectionView;
};

/**
 * @brief The HelloViewController interface.
 */
struct HelloViewControllerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewControllerInterface viewControllerInterface;
};

/**
 * @brief The HelloViewController Class.
 */
Class *_HelloViewController(void);
