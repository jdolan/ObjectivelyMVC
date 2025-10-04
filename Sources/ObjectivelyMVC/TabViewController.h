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

#include <ObjectivelyMVC/ViewController.h>
#include <ObjectivelyMVC/TabView.h>
#include <ObjectivelyMVC/TabViewItem.h>

/**
 * @file
 * @brief TabViewControllers arrange and manage their child ViewControllers in a tab view interface.
 */

typedef struct TabViewController TabViewController;
typedef struct TabViewControllerInterface TabViewControllerInterface;

/**
 * @brief TabViewControllers arrange and manage their child ViewControllers in a tab view interface.
 * @details Tabs are added to the tab view interface by adding child ViewControllers to a 
 * TabViewController.
 * @extends ViewController
 * @ingroup ViewControllers
 */
struct TabViewController {

  /**
   * @brief The superclass.
   */
  ViewController viewController;

  /**
   * @brief The interface.
   * @protected
   */
  TabViewControllerInterface *interface;

  /**
   * @brief The TabView.
   */
  TabView *tabView;
};

/**
 * @brief The TabViewController interface.
 */
struct TabViewControllerInterface {

  /**
   * @brief The superclass interface.
   */
  ViewControllerInterface viewControllerInterface;

  /**
   * @fn TabViewController *TabViewController::init(TabViewController *self)
   * @brief Initializes this TabViewController.
   * @param self The TabViewController.
   * @return The initialized TabViewController, or `NULL` on error.
   * @memberof TabViewController
   */
  TabViewController *(*init)(TabViewController *self);

  /**
   * @fn TabViewItem *TabViewController::tabForViewController(const TabViewController *self, const ViewController *viewController)
   * @brief Returns the TabViewItem for the specified child ViewController, or `NULL`.
   * @param self The TabViewController.
   * @param viewController The child ViewController.
   * @return The TabViewItem associated with the specified child ViewController.
   * @memberof TabViewController
   */
  TabViewItem *(*tabForViewController)(const TabViewController *self, const ViewController *viewController);
};

/**
 * @fn Class *TabViewController::_TabViewController(void)
 * @brief The TabViewController archetype.
 * @return The TabViewController Class.
 * @memberof TabViewController
 */
OBJECTIVELYMVC_EXPORT Class *_TabViewController(void);
