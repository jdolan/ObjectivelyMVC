/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  NavigationViewController *this = (NavigationViewController *) self;

  release(this->disappearingViewController);

  super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @brief True while the NavigationViewController is appearing or visible.
 */
static bool isShown(const NavigationViewController *self) {
  return self->appearance == ViewControllerAppearanceAppearing ||
         self->appearance == ViewControllerAppearanceVisible;
}

/**
 * @brief Shows the view of `viewController`, which is now the top ViewController, with the
 * appearance callbacks while the NavigationViewController is shown.
 * @remarks While the NavigationViewController is appearing, `viewController` receives
 * `viewDidAppear` with it. If its `viewWillAppear` pushes or pops, it is no longer the top, and
 * its view is not shown.
 */
static void showTopViewController(NavigationViewController *self, ViewController *viewController) {

  ViewController *this = (ViewController *) self;

  retain(viewController);

  const bool shown = isShown(self);

  if (shown) {
    $(viewController, viewWillAppear);

    if (viewController != $(self, topViewController)) {
      release(viewController);
      return;
    }
  }

  $(this->view, addSubview, viewController->view);

  if (shown && self->appearance == ViewControllerAppearanceVisible) {
    $(viewController, viewDidAppear);
  }

  release(viewController);
}

/**
 * @brief Hides the view of `viewController`, which is no longer the top ViewController, with the
 * disappearance callbacks while the NavigationViewController is shown.
 */
static void hideTopViewController(NavigationViewController *self, ViewController *viewController) {

  retain(viewController);

  const bool shown = isShown(self);

  if (shown) {
    $(viewController, viewWillDisappear);
  }

  $(viewController->view, removeFromSuperview);

  if (shown) {
    $(viewController, viewDidDisappear);
  }

  release(viewController);
}

/**
 * @see ViewController::addChildViewController(ViewController *, ViewController *)
 * @remarks This does not call `super`, which would make the child appear whenever the
 * NavigationViewController is in a window. The child appears only while the
 * NavigationViewController is shown. To cover the top ViewController, use `pushViewController`.
 */
static void addChildViewController(ViewController *self, ViewController *childViewController) {

  assert(childViewController);

  ViewController *that = retain(childViewController);

  $(childViewController, removeFromParentViewController);

  $(self->childViewControllers, addObject, childViewController);
  childViewController->parentViewController = self;

  $(self, loadViewIfNeeded);
  $(childViewController, loadViewIfNeeded);

  $(childViewController->view, updateBindings, NULL);

  showTopViewController((NavigationViewController *) self, childViewController);

  release(that);
}

/**
 * @see ViewController::removeChildViewController(ViewController *, ViewController *)
 * @remarks This does not call `super`, which would make the child disappear whether or not it was
 * shown. Only the top ViewController disappears, and only while the NavigationViewController is
 * shown. While it is disappearing, the child already received `viewWillDisappear`, and receives
 * `viewDidDisappear` with the NavigationViewController.
 */
static void removeChildViewController(ViewController *self, ViewController *childViewController) {

  assert(childViewController);
  assert(childViewController->parentViewController == self);

  NavigationViewController *this = (NavigationViewController *) self;

  ViewController *that = retain(childViewController);

  const bool top = childViewController == $(this, topViewController);

  $(self->childViewControllers, removeObject, childViewController);
  childViewController->parentViewController = NULL;

  if (top) {
    hideTopViewController(this, childViewController);
  } else {
    $(childViewController->view, removeFromSuperview);
  }

  release(that);
}

/**
 * @see ViewController::respondToEvent(ViewController *, const SDL_Event *)
 */
static void respondToEvent(ViewController *self, const SDL_Event *event) {

  ViewController *topViewController = $((NavigationViewController *) self, topViewController);
  if (topViewController) {
    $(topViewController, respondToEvent, event);
  }
}

/**
 * @see ViewController::viewDidAppear(ViewController *)
 */
static void viewDidAppear(ViewController *self) {

  NavigationViewController *this = (NavigationViewController *) self;

  this->appearance = ViewControllerAppearanceVisible;

  ViewController *topViewController = $(this, topViewController);
  if (topViewController) {
    $(topViewController, viewDidAppear);
  }
}

/**
 * @see ViewController::viewDidDisappear(ViewController *)
 */
static void viewDidDisappear(ViewController *self) {

  NavigationViewController *this = (NavigationViewController *) self;

  this->appearance = ViewControllerAppearanceHidden;

  ViewController *disappearing = this->disappearingViewController;
  this->disappearingViewController = NULL;

  if (disappearing) {
    $(disappearing, viewDidDisappear);
    release(disappearing);
  }
}

/**
 * @see ViewController::viewWillAppear(ViewController *)
 */
static void viewWillAppear(ViewController *self) {

  NavigationViewController *this = (NavigationViewController *) self;

  this->appearance = ViewControllerAppearanceAppearing;

  ViewController *topViewController = $(this, topViewController);
  if (topViewController) {
    $(topViewController, viewWillAppear);
  }
}

/**
 * @see ViewController::viewWillDisappear(ViewController *)
 */
static void viewWillDisappear(ViewController *self) {

  NavigationViewController *this = (NavigationViewController *) self;

  this->appearance = ViewControllerAppearanceDisappearing;

  release(this->disappearingViewController);
  this->disappearingViewController = NULL;

  ViewController *topViewController = $(this, topViewController);
  if (topViewController) {
    this->disappearingViewController = retain(topViewController);
    $(topViewController, viewWillDisappear);
  }
}

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

  ViewController *topViewController = $(self, topViewController);
  if (topViewController) {
    hideTopViewController(self, topViewController);
  }

  $((ViewController *) self, addChildViewController, viewController);
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
 * @remarks The ViewControllers between the top and `viewController` are covered, so they are
 * removed without appearing.
 */
static void popToViewController(NavigationViewController *self, const ViewController *viewController) {

  ViewController *this = (ViewController *) self;

  ViewController *topViewController = $(self, topViewController);
  if (topViewController == NULL || topViewController == viewController) {
    return;
  }

  $(this, removeChildViewController, topViewController);

  while ((topViewController = $(self, topViewController)) && topViewController != viewController) {

    ViewController *that = retain(topViewController);

    $(this->childViewControllers, removeObject, topViewController);
    topViewController->parentViewController = NULL;

    $(topViewController->view, removeFromSuperview);

    release(that);
  }

  if (topViewController) {
    showTopViewController(self, topViewController);
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

  topViewController = $(self, topViewController);
  if (topViewController) {
    showTopViewController(self, topViewController);
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

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewControllerInterface *) clazz->interface)->addChildViewController = addChildViewController;
  ((ViewControllerInterface *) clazz->interface)->removeChildViewController = removeChildViewController;
  ((ViewControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
  ((ViewControllerInterface *) clazz->interface)->viewDidAppear = viewDidAppear;
  ((ViewControllerInterface *) clazz->interface)->viewDidDisappear = viewDidDisappear;
  ((ViewControllerInterface *) clazz->interface)->viewWillAppear = viewWillAppear;
  ((ViewControllerInterface *) clazz->interface)->viewWillDisappear = viewWillDisappear;

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
      .interfaceSize = sizeof(NavigationViewControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

