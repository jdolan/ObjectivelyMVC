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
   * @brief The child view controllers.
   */
  MutableArray *childViewControllers;

  /**
   * @brief The parent view controller.
   */
  ViewController *parentViewController;
  
  /**
   * @brief The main view.
   */
  View *view;
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
   * @fn void ViewController::addChildViewController(ViewController *self, ViewController *childViewController)
   * @brief Adds the specified child ViewController to this ViewController.
   * @param self The ViewController.
   * @param childViewController The child ViewController.
   * @memberof ViewController
   */
  void (*addChildViewController)(ViewController *self, ViewController *childViewController);

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
   * @fn void ViewController::moveToParentViewController(ViewController *self, ViewController *parentViewController)
   * @brief Moves this ViewController to the specified parent.
   * @param self The ViewController.
   * @param parentViewController The parent ViewController, or `NULL`.
   * @memberof ViewController
   */
  void (*moveToParentViewController)(ViewController *self, ViewController *parentViewController);

  /**
   * @fn void ViewController::removeChildViewController(ViewController *self, ViewController *childViewController)
   * @brief Removes the specified child ViewController from this ViewController.
   * @param self The ViewController.
   * @param childViewController The child ViewController.
   * @memberof ViewController
   */
  void (*removeChildViewController)(ViewController *self, ViewController *childViewController);

  /**
   * @fn void ViewController::removeFromParentViewController(ViewController *self)
   * @brief Removes this ViewController from its parent.
   * @param self The ViewController.
   * @memberof ViewController
   */
  void (*removeFromParentViewController)(ViewController *self);

  /**
   * @fn void ViewController::respondToEvent(ViewController *self, const SDL_Event *event)
   * @brief Responds to the given event.
   * @param self The ViewController.
   * @param event The event.
   * @memberof ViewController
   */
  void (*respondToEvent)(ViewController *self, const SDL_Event *event);

  /**
   * @fn void ViewController::setView(ViewController *self, View *view)
   * @brief Sets this ViewController's View.
   * @param self The ViewController.
   * @param view The View.
   * @memberof ViewController
   */
  void (*setView)(ViewController *self, View *view);

  /**
   * @fn void ViewController::viewDidAppear(ViewController *self)
   * @brief This method is invoked after this ViewController's View is added to the View hierarchy.
   * @param self The ViewController.
   * @memberof ViewController
   * @remarks The default implementation of this method does nothing.
   */
  void (*viewDidAppear)(ViewController *self);

  /**
   * @fn void ViewController::viewDidDisappear(ViewController *self)
   * @brief This method is invoked after this ViewController's View is removed to the View hierarchy.
   * @param self The ViewController.
   * @memberof ViewController
   * @remarks The default implementation of this method does nothing.
   */
  void (*viewDidDisappear)(ViewController *self);

  /**
   * @fn void ViewController::viewWillAppear(ViewController *self)
   * @brief This method is invoked before this ViewController's View is added to the View hierarchy.
   * @param self The ViewController.
   * @memberof ViewController
   * @remarks The default implementation of this method does nothing.
   */
  void (*viewWillAppear)(ViewController *self);

  /**
   * @fn void ViewController::viewWillDisappear(ViewController *self)
   * @brief This method is invoked before this ViewController's View is removed from the View hierarchy.
   * @param self The ViewController.
   * @memberof ViewController
   * @remarks The default implementation of this method does nothing.
   */
  void (*viewWillDisappear)(ViewController *self);
};

/**
 * @fn Class *ViewController::_ViewController(void)
 * @brief The ViewController archetype.
 * @return The ViewController Class.
 * @memberof ViewController
 */
OBJECTIVELYMVC_EXPORT Class *_ViewController(void);

