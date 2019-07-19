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

#include <ObjectivelyMVC/ViewController.h>

#define _Class _ViewController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ViewController *this = (ViewController *) self;

	release(this->childViewControllers);
	release(this->view);

	super(Object, self, dealloc);
}

#pragma mark - ViewController

/**
 * @fn void ViewController::addChildViewController(ViewController *self, ViewController *childViewController)
 * @memberof ViewController
 */
static void addChildViewController(ViewController *self, ViewController *childViewController) {

	assert(childViewController);

	ViewController *that = retain(childViewController);

	$(childViewController, removeFromParentViewController);

	$(self->childViewControllers, addObject, childViewController);
	childViewController->parentViewController = self;

	$(self, loadViewIfNeeded);
	$(childViewController, loadViewIfNeeded);

	if (self->view->window) {
		$(childViewController->view, updateBindings);
		$(childViewController, viewWillAppear);
	}

	$(self->view, addSubview, childViewController->view);

	if (self->view->window) {
		$(childViewController, viewDidAppear);
	}

	release(that);
}

/**
 * @brief ArrayEnumerator for handleNotification recursion.
 */
static void handleNotification_recurse(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, handleNotification, data);
}

/**
 * @fn void ViewController::handleNotification(ViewController *self, const Notification *notification)
 * @memberof ViewController
 */
static void handleNotification(ViewController *self, const Notification *notification) {
	$((Array *) self->childViewControllers, enumerateObjects, handleNotification_recurse, (ident) notification);
}

/**
 * @fn ViewController *ViewController::init(ViewController *self)
 * @memberof ViewController
 */
static ViewController *init(ViewController *self) {

	self = (ViewController *) super(Object, self, init);
	if (self) {
		self->childViewControllers = $$(MutableArray, arrayWithCapacity, 0);
		assert(self->childViewControllers);
	}

	return self;
}

/**
 * @fn void ViewController::loadView(ViewController *self)
 * @memberof ViewController
 */
static void loadView(ViewController *self) {

	View *view = $(alloc(View), initWithFrame, NULL);
	assert(view);

	view->autoresizingMask = ViewAutoresizingFill;

	$(self, setView, view);
	release(view);
}

/**
 * @fn void ViewController::loadViewIfNeeded(ViewController *self)
 * @memberof ViewController
 */
static void loadViewIfNeeded(ViewController *self) {

	if (self->view == NULL) {
		$(self, loadView);
	}

	assert(self->view);
}

/**
 * @fn void ViewController::moveToParentViewController(ViewController *self, ViewController *parentViewController)
 * @memberof ViewController
 */
static void moveToParentViewController(ViewController *self, ViewController *parentViewController) {

	ViewController *this = retain(self);

	$(self, removeFromParentViewController);

	if (parentViewController) {
		$(parentViewController, addChildViewController, self);
	}

	release(this);
}

/**
 * @fn void ViewController::removeChildViewController(ViewController *self, ViewController *childViewController)
 * @memberof ViewController
 */
static void removeChildViewController(ViewController *self, ViewController *childViewController) {

	assert(childViewController);
	assert(childViewController->parentViewController == self);

	ViewController *that = retain(childViewController);

	$(self->childViewControllers, removeObject, childViewController);
	childViewController->parentViewController = NULL;

	$(childViewController, viewWillDisappear);
	$(childViewController->view, removeFromSuperview);
	$(childViewController, viewDidDisappear);

	release(that);
}

/**
 * @fn void ViewController::removeFromParentViewController(ViewController *self)
 * @memberof ViewController
 */
static void removeFromParentViewController(ViewController *self) {

	if (self->parentViewController) {
		$(self->parentViewController, removeChildViewController, self);
	}
}

/**
 * @fn void ViewController::respondToEvent(ViewController *self, const SDL_Event *event)
 * @memberof ViewController
 */
static void respondToEvent(ViewController *self, const SDL_Event *event) {

}

/**
 * @fn void ViewController::setView(ViewController *self, View *view)
 * @memberof ViewController
 */
static void setView(ViewController *self, View *view) {

	if (view != self->view) {

		if (self->view) {
			self->view->viewController = NULL;
			release(self->view);
		}

		if (view) {
			self->view = retain(view);
			self->view->viewController = self;
		} else {
			self->view = NULL;
		}
	}
}

/**
 * @brief ArrayEnumerator for viewDidAppear recursion.
 */
static void viewDidAppear_recurse(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, viewDidAppear);
}

/**
 * @fn void ViewController::viewDidAppear(ViewController *self)
 * @memberof ViewController
 */
static void viewDidAppear(ViewController *self) {
	$((Array *) self->childViewControllers, enumerateObjects, viewDidAppear_recurse, NULL);
}

/**
 * @brief ArrayEnumerator for viewDidDisappear recursion.
 */
static void viewDidDisappear_recurse(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, viewDidAppear);
}

/**
 * @fn void ViewController::viewDidDisappear(ViewController *self)
 * @memberof ViewController
 */
static void viewDidDisappear(ViewController *self) {
	$((Array *) self->childViewControllers, enumerateObjects, viewDidDisappear_recurse, NULL);
}

/**
 * @brief ArrayEnumerator for viewWillAppear recursion.
 */
static void viewWillAppear_recurse(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, viewWillAppear);
}

/**
 * @fn void ViewController::viewWillAppear(ViewController *self)
 * @memberof ViewController
 */
static void viewWillAppear(ViewController *self) {
	$((Array *) self->childViewControllers, enumerateObjects, viewWillAppear_recurse, NULL);
}

/**
 * @brief ArrayEnumerator for viewWillDisappear recursion.
 */
static void viewWillDisappear_recurse(const Array *array, ident obj, ident data) {
	$((ViewController *) obj, viewWillDisappear);
}

/**
 * @fn void ViewController::viewWillDisappear(ViewController *self)
 * @memberof ViewController
 */
static void viewWillDisappear(ViewController *self) {
	$((Array *) self->childViewControllers, enumerateObjects, viewWillDisappear_recurse, NULL);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewControllerInterface *) clazz->interface)->addChildViewController = addChildViewController;
	((ViewControllerInterface *) clazz->interface)->handleNotification = handleNotification;
	((ViewControllerInterface *) clazz->interface)->init = init;
	((ViewControllerInterface *) clazz->interface)->loadView = loadView;
	((ViewControllerInterface *) clazz->interface)->loadViewIfNeeded = loadViewIfNeeded;
	((ViewControllerInterface *) clazz->interface)->moveToParentViewController = moveToParentViewController;
	((ViewControllerInterface *) clazz->interface)->removeChildViewController = removeChildViewController;
	((ViewControllerInterface *) clazz->interface)->removeFromParentViewController = removeFromParentViewController;
	((ViewControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((ViewControllerInterface *) clazz->interface)->setView = setView;
	((ViewControllerInterface *) clazz->interface)->viewDidAppear = viewDidAppear;
	((ViewControllerInterface *) clazz->interface)->viewDidDisappear = viewDidDisappear;
	((ViewControllerInterface *) clazz->interface)->viewWillAppear = viewWillAppear;
	((ViewControllerInterface *) clazz->interface)->viewWillDisappear = viewWillDisappear;
}

/**
 * @fn Class *ViewController::_ViewController(void)
 * @memberof ViewController
 */
Class *_ViewController(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "ViewController",
			.superclass = _Object(),
			.instanceSize = sizeof(ViewController),
			.interfaceOffset = offsetof(ViewController, interface),
			.interfaceSize = sizeof(ViewControllerInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
