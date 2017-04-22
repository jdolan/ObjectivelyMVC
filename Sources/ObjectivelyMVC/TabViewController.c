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

#include <assert.h>

#include <ObjectivelyMVC/TabViewController.h>

#define _Class _TabViewController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TabViewController *this = (TabViewController *) self;

	release(this->tabView);

	super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);

	TabViewController *this = (TabViewController *) self;

	this->tabView = $(alloc(TabView), initWithFrame, NULL);
	assert(this->tabView);

	this->tabView->delegate.self = this;

	$(self->view, addSubview, (View *) this->tabView);
}

/**
 * @see ViewController::addChildViewController(ViewController *, ViewController *)
 */
static void addChildViewController(ViewController *self, ViewController *childViewController) {

	super(ViewController, self, addChildViewController, childViewController);

	TabViewController *this = (TabViewController *) self;

	TabViewItem *tabViewItem = $(alloc(TabViewItem), initWithView, childViewController->view);

	$(this->tabView, addTabViewItem, tabViewItem);

	release(tabViewItem);
}

/**
 * @see ViewController::removeChildViewController(ViewController *, ViewController *)
 */
static void removeChildViewController(ViewController *self, ViewController *childViewController) {

	super(ViewController, self, removeChildViewController, childViewController);

	TabViewController *this = (TabViewController *) self;

	TabViewItem *tabViewItem = $(this, tabViewItemFor, childViewController);

	$(this->tabView, removeTabViewItem, tabViewItem);
}

#pragma mark - TabViewController

/**
 * @fn TabViewController *TabViewController::init(TabViewController *self)
 * @memberof TabViewController
 */
static TabViewController *init(TabViewController *self) {

	return (TabViewController *) super(ViewController, self, init);
}

/**
 * @brief Predicate for tabViewItemFor.
 */
static _Bool tabViewItemFor_predicate(const ident obj, ident data) {
	return ((TabViewItem *) obj)->view == data;
}

/**
 * @fn TabViewItem *TabViewController::tabViewForItem(const TabViewController *self, const ViewController *viewController)
 * @memberof TabViewController
 */
static TabViewItem *tabViewItemFor(const TabViewController *self, const ViewController *viewController) {

	const Array *tabViewItems = (Array *) self->tabView->tabViewItems;

	return $(tabViewItems, findObject, tabViewItemFor_predicate, (ident) viewController->view);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewControllerInterface *) clazz->def->interface)->loadView = loadView;
	((ViewControllerInterface *) clazz->def->interface)->addChildViewController = addChildViewController;
	((ViewControllerInterface *) clazz->def->interface)->removeChildViewController = removeChildViewController;

	((TabViewControllerInterface *) clazz->def->interface)->init = init;
	((TabViewControllerInterface *) clazz->def->interface)->tabViewItemFor = tabViewItemFor;
}

/**
 * @fn Class *TabViewController::_TabViewController(void)
 * @memberof TabViewController
 */
Class *_TabViewController(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "TabViewController";
		clazz.superclass = _ViewController();
		clazz.instanceSize = sizeof(TabViewController);
		clazz.interfaceOffset = offsetof(TabViewController, interface);
		clazz.interfaceSize = sizeof(TabViewControllerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
