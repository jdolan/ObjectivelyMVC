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

	self->view->autoresizingMask = ViewAutoresizingContain;

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

	TabViewItem *tab = $(alloc(TabViewItem), initWithView, childViewController->view);

	$(this->tabView, addTab, tab);

	release(tab);
}

/**
 * @see ViewController::removeChildViewController(ViewController *, ViewController *)
 */
static void removeChildViewController(ViewController *self, ViewController *childViewController) {

	super(ViewController, self, removeChildViewController, childViewController);

	TabViewController *this = (TabViewController *) self;

	TabViewItem *tab = $(this, tabForViewController, childViewController);

	$(this->tabView, removeTab, tab);
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
static _Bool tabForViewController_predicate(const ident obj, ident data) {
	return ((TabViewItem *) obj)->view == data;
}

/**
 * @fn TabViewItem *TabViewController::tabForViewController(const TabViewController *self, const ViewController *viewController)
 * @memberof TabViewController
 */
static TabViewItem *tabForViewController(const TabViewController *self, const ViewController *viewController) {

	assert(viewController);

	if (viewController->view) {
		return $((Array *) self->tabView->tabs, findObject, tabForViewController_predicate, (ident) viewController->view);
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

	((TabViewControllerInterface *) clazz->interface)->init = init;
	((TabViewControllerInterface *) clazz->interface)->tabForViewController = tabForViewController;
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
			.interfaceOffset = offsetof(TabViewController, interface),
			.interfaceSize = sizeof(TabViewControllerInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
