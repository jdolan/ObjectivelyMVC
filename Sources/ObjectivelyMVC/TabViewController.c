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
#include <string.h>

#include "TabViewController.h"

#define _Class _TabViewController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TabViewController *this = (TabViewController *) self;

  if (this->tabView) {
    memset(&this->tabView->delegate, 0, sizeof(this->tabView->delegate));
  }

  release(this->tabView);

  super(Object, self, dealloc);
}

#pragma mark - TabViewDelegate

/**
 * @brief TabViewDelegate callback that makes the previous tab disappear and the selected tab
 * appear, while the TabViewController is visible.
 */
static void didSelectTab(TabView *tabView, TabViewItem *tab) {

  TabViewController *this = tabView->delegate.self;

  ViewController *previous = this->selectedViewController;
  ViewController *selected = $(this, viewControllerForTab, tab);

  if (selected != previous) {
    this->selectedViewController = selected;

    if (this->visible) {
      if (previous) {
        $(previous, viewWillDisappear);
      }
      if (selected) {
        $(selected, viewWillAppear);
      }
      if (previous) {
        $(previous, viewDidDisappear);
      }
      if (selected) {
        $(selected, viewDidAppear);
      }
    }
  }

  if (this->delegate.didSelectTab) {
    this->delegate.didSelectTab(this, tab);
  }
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

  super(ViewController, self, loadView);

  $(self->view, addClassName, "tabViewController");

  TabViewController *this = (TabViewController *) self;

  this->tabView = $(alloc(TabView), initWithFrame, NULL);
  assert(this->tabView);

  this->tabView->delegate.self = this;
  this->tabView->delegate.didSelectTab = didSelectTab;

  $(self->view, addSubview, (View *) this->tabView);
}

/**
 * @see ViewController::addChildViewController(ViewController *, ViewController *)
 * @remarks This does not call `super`, which would make every new child appear. The TabView
 * selects the first tab that is added, and `didSelectTab` makes that tab appear.
 */
static void addChildViewController(ViewController *self, ViewController *childViewController) {

  assert(childViewController);

  TabViewController *this = (TabViewController *) self;

  ViewController *that = retain(childViewController);

  $(childViewController, removeFromParentViewController);

  $(self->childViewControllers, addObject, childViewController);
  childViewController->parentViewController = self;

  $(self, loadViewIfNeeded);
  $(childViewController, loadViewIfNeeded);

  $(childViewController->view, updateBindings, NULL);

  TabViewItem *tab = $(alloc(TabViewItem), initWithView, childViewController->view);

  $(this->tabView, addTab, tab);

  release(tab);
  release(that);
}

/**
 * @see ViewController::removeChildViewController(ViewController *, ViewController *)
 * @remarks This does not call `super`, which would make every removed child disappear. Only the
 * selected tab disappears, and the TabView then selects another tab.
 */
static void removeChildViewController(ViewController *self, ViewController *childViewController) {

  assert(childViewController);
  assert(childViewController->parentViewController == self);

  TabViewController *this = (TabViewController *) self;

  ViewController *that = retain(childViewController);

  const bool selected = childViewController == this->selectedViewController;
  const bool visible = selected && this->visible;

  if (selected) {
    this->selectedViewController = NULL;
  }

  if (visible) {
    $(childViewController, viewWillDisappear);
  }

  TabViewItem *tab = $(this, tabForViewController, childViewController);
  if (tab) {
    $(this->tabView, removeTab, tab);
  }

  $(self->childViewControllers, removeObject, childViewController);
  childViewController->parentViewController = NULL;

  if (visible) {
    $(childViewController, viewDidDisappear);
  }

  release(that);
}

/**
 * @see ViewController::respondToEvent(ViewController *, const SDL_Event *)
 */
static void respondToEvent(ViewController *self, const SDL_Event *event) {

  const TabViewController *this = (TabViewController *) self;

  if (this->selectedViewController) {
    $(this->selectedViewController, respondToEvent, event);
  }
}

/**
 * @see ViewController::viewDidAppear(ViewController *)
 */
static void viewDidAppear(ViewController *self) {

  const TabViewController *this = (TabViewController *) self;

  if (this->selectedViewController) {
    $(this->selectedViewController, viewDidAppear);
  }
}

/**
 * @see ViewController::viewDidDisappear(ViewController *)
 */
static void viewDidDisappear(ViewController *self) {

  const TabViewController *this = (TabViewController *) self;

  if (this->selectedViewController) {
    $(this->selectedViewController, viewDidDisappear);
  }
}

/**
 * @see ViewController::viewWillAppear(ViewController *)
 */
static void viewWillAppear(ViewController *self) {

  TabViewController *this = (TabViewController *) self;

  this->visible = true;

  if (this->selectedViewController) {
    $(this->selectedViewController, viewWillAppear);
  }
}

/**
 * @see ViewController::viewWillDisappear(ViewController *)
 */
static void viewWillDisappear(ViewController *self) {

  TabViewController *this = (TabViewController *) self;

  this->visible = false;

  if (this->selectedViewController) {
    $(this->selectedViewController, viewWillDisappear);
  }
}

#pragma mark - TabViewController

/**
 * @fn TabViewController *TabViewController::init(TabViewController *)
 * @memberof TabViewController
 */
static TabViewController *init(TabViewController *self) {
  return (TabViewController *) super(ViewController, self, init);
}

/**
 * @brief Predicate for tabViewItemFor.
 */
static bool tabForViewController_predicate(const ident obj, ident data) {
  return ((TabViewItem *) obj)->view == data;
}

/**
 * @fn TabViewItem *TabViewController::tabForViewController(const TabViewController *, const ViewController *)
 * @memberof TabViewController
 */
static TabViewItem *tabForViewController(const TabViewController *self, const ViewController *viewController) {

  assert(viewController);

  if (viewController->view) {
    return $((Array *) self->tabView->tabs, find, tabForViewController_predicate, (ident) viewController->view);
  }

  return NULL;
}

/**
 * @brief Predicate for viewControllerForTab.
 */
static bool viewControllerForTab_predicate(const ident obj, ident data) {
  return ((ViewController *) obj)->view == data;
}

/**
 * @fn ViewController *TabViewController::viewControllerForTab(const TabViewController *, const TabViewItem *)
 * @memberof TabViewController
 */
static ViewController *viewControllerForTab(const TabViewController *self, const TabViewItem *tab) {

  assert(tab);

  if (tab->view) {
    return $((Array *) self->viewController.childViewControllers, find, viewControllerForTab_predicate, tab->view);
  }

  return NULL;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewControllerInterface *) clazz->interface)->loadView = loadView;
  ((ViewControllerInterface *) clazz->interface)->addChildViewController = addChildViewController;
  ((ViewControllerInterface *) clazz->interface)->removeChildViewController = removeChildViewController;
  ((ViewControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
  ((ViewControllerInterface *) clazz->interface)->viewDidAppear = viewDidAppear;
  ((ViewControllerInterface *) clazz->interface)->viewDidDisappear = viewDidDisappear;
  ((ViewControllerInterface *) clazz->interface)->viewWillAppear = viewWillAppear;
  ((ViewControllerInterface *) clazz->interface)->viewWillDisappear = viewWillDisappear;

  ((TabViewControllerInterface *) clazz->interface)->init = init;
  ((TabViewControllerInterface *) clazz->interface)->tabForViewController = tabForViewController;
  ((TabViewControllerInterface *) clazz->interface)->viewControllerForTab = viewControllerForTab;
}

/**
 * @fn Class *TabViewController::_TabViewController(void)
 * @memberof TabViewController
 */
Class *_TabViewController(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TabViewController",
      .superclass = _ViewController(),
      .instanceSize = sizeof(TabViewController),
      .interfaceSize = sizeof(TabViewControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
