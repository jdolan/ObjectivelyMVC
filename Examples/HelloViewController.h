/*
 * HelloViewController.h
 *
 *  Created on: Jan 19, 2016
 *      Author: jdolan
 */

#ifndef _ObjectivelyMVC_HelloViewController_h_
#define _ObjectivelyMVC_HelloViewController_h_

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
	 * @brief The parent.
	 *
	 * @private
	 */
	ViewController viewController;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	HelloViewControllerInterface *interface;
};

/**
 * @brief The HelloViewController interface.
 */
struct HelloViewControllerInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewControllerInterface viewControllerInterface;
};

/**
 * @brief The HelloViewController Class.
 */
extern Class _HelloViewController;

#endif
