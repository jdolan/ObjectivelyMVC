/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_ViewController_h_
#define _ObjectivelyMVC_ViewController_h_

#include <Objectively/Object.h>

#include <ObjectivelyMVC/View.h>

typedef struct ViewController ViewController;
typedef struct ViewControllerInterface ViewControllerInterface;

/**
 * @brief The ViewController type.
 */
struct ViewController {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	ViewControllerInterface *interface;

	/**
	 * @brief The title.
	 */
	String *title;

	/**
	 * @brief The main view.
	 */
	View *view;

	/**
	 * @brief The parent view controller.
	 */
	ViewController *parentViewController;

	/**
	 * @brief The child view controllers.
	 */
	Array *childViewControllers;
};

/**
 * @brief The ViewController type.
 */
struct ViewControllerInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;

	/**
	 * @brief Initializes this view controller.
	 *
	 * @return The initialized view controller, or NULL on error.
	 */
	ViewController *(*init)(ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be shown.
	 *
	 * @remark Subclasses may implement this method to prepare the
	 * view for rendering.
	 */
	void (*viewWillAppear)(ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be hidden.
	 *
	 * @remark Subclasses may implement this method to free resources
	 * allocated in `viewWillAppear`.
	 */
	void (*viewWillDisappear)(ViewController *self);
};

/**
 * @brief The ViewController Class.
 */
extern Class _ViewController;

#endif
