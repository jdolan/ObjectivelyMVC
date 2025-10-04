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

#include <assert.h>

#include "NavigationViewController.h"

#define _Class _NavigationViewController

#pragma mark - Object

#pragma mark - NavigationViewController

/**
 * @fn NavigationViewController *NavigationViewController::init(NavigationViewController *self)
 * @memberof NavigationViewController
 */
static NavigationViewController *init(NavigationViewController *self) {

  return (NavigationViewController *) super(ViewController, self, init);
}

/**
 * @fn void NavigationViewController::pushViewController(NavigationViewController *self, ViewController *viewController)
 * @memberof NavigationViewController
 */
static void pushViewController(NavigationViewController *self, ViewController *viewController) {

  assert(viewController);

  ViewController *this = (ViewController *) self;

  ViewController *topViewController = $(self, topViewController);
  if (topViewController) {

    $(topViewController, viewWillDisappear);
    $(this->view, removeSubview, topViewController->view);
    $(topViewController, viewDidDisappear);
  }

  $(this, addChildViewController, viewController);
}

/**
 * @fn void NavigationViewController::popToRootViewController(NavigationViewController *self)
 * @memberof NavigationViewController
 */
static void popToRootViewController(NavigationViewController *self) {

  const ViewController *rootViewController = $(self, rootViewController);
  if (rootViewController) {
    $(self, popToViewController, rootViewController);
  }
}

/**
 * @fn void NavigationViewController::popToViewController(NavigationViewController *self, const ViewController *viewController)
 * @memberof NavigationViewController
 */
static void popToViewController(NavigationViewController *self, const ViewController *viewController) {

  ViewController *topViewController;
  while ((topViewController = $(self, topViewController))) {

    if (topViewController == viewController) {
      break;
    }

    $(self, popViewController);
  }
}

/**
 * @fn void NavigationViewController::popViewController(NavigationViewController *self)
 * @memberof NavigationViewController
 */
static void popViewController(NavigationViewController *self) {

  ViewController *topViewController = $(self, topViewController);
  if (topViewController) {
    $((ViewController *) self, removeChildViewController, topViewController);
  }

  ViewController *this = (ViewController *) self;
  ViewController *that = $(self, topViewController);

  if (that) {
    $(that, viewWillAppear);
    $(this->view, addSubview, that->view);
    $(that, viewDidAppear);
  }
}

/**
 * @fn ViewController NavigationViewController::rootViewController(const NavigationViewController *self)
 * @memberof NavigationViewController
 */
static ViewController *rootViewController(const NavigationViewController *self) {
  return (ViewController *) $((Array *) self->viewController.childViewControllers, firstObject);
}

/**
 * @fn ViewController NavigationViewController::topViewController(const NavigationViewController *self)
 * @memberof NavigationViewController
 */
static ViewController *topViewController(const NavigationViewController *self) {
  return (ViewController *) $((Array *) self->viewController.childViewControllers, lastObject);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((NavigationViewControllerInterface *) clazz->interface)->init = init;
  ((NavigationViewControllerInterface *) clazz->interface)->pushViewController = pushViewController;
  ((NavigationViewControllerInterface *) clazz->interface)->popToRootViewController = popToRootViewController;
  ((NavigationViewControllerInterface *) clazz->interface)->popToViewController = popToViewController;
  ((NavigationViewControllerInterface *) clazz->interface)->popViewController = popViewController;
  ((NavigationViewControllerInterface *) clazz->interface)->rootViewController = rootViewController;
  ((NavigationViewControllerInterface *) clazz->interface)->topViewController = topViewController;
}

/**
 * @fn Class *NavigationViewController::_NavigationViewController(void)
 * @memberof NavigationViewController
 */
Class *_NavigationViewController(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "NavigationViewController",
      .superclass = _ViewController(),
      .instanceSize = sizeof(NavigationViewController),
      .interfaceOffset = offsetof(NavigationViewController, interface),
      .interfaceSize = sizeof(NavigationViewControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

