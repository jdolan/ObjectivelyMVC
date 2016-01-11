/*
 * ViewController.h
 *
 *  Created on: Nov 18, 2014
 *      Author: jdolan
 */


#ifndef _MVC_viewcontroller_h_
#define _MVC_viewcontroller_h_

#include <Objectively/Object.h>
#include <SDL2/SDL_stdinc.h>

#include "MVC_view.h"

typedef struct MVC_ViewController MVC_ViewController;
typedef struct MVC_ViewControllerInterface MVC_ViewControllerInterface;

/**
 * @brief The ViewController type.
 */
struct MVC_ViewController {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	MVC_ViewControllerInterface *interface;

	/**
	 * @brief The title.
	 */
	String *title;

	/**
	 * @brief The main view.
	 */
	MVC_View *view;

	/**
	 * @brief The parent view controller.
	 */
	MVC_ViewController *parentViewController;

	/**
	 * @brief The child view controllers.
	 */
	Array *childViewControllers;
};

/**
 * @brief The ViewController type.
 */
struct MVC_ViewControllerInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;

	/**
	 * @brief Initializes this view controller.
	 *
	 * @return The initialized view controller, or NULL on error.
	 */
	MVC_ViewController *(*init)(MVC_ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be shown.
	 *
	 * @remark Subclasses may implement this method to prepare the
	 * view for rendering.
	 */
	void (*viewWillAppear)(MVC_ViewController *self);

	/**
	 * @brief Called when this controller's view is about to be hidden.
	 *
	 * @remark Subclasses may implement this method to free resources
	 * allocated in `viewWillAppear`.
	 */
	void (*viewWillDisappear)(MVC_ViewController *self);
};

/**
 * @brief The ViewController Class.
 */
extern Class __ViewController;

/**
 * @brief
 */
extern DECLSPEC MVC_ViewController * SDLCALL CreateViewController(void);

/**
 * @brief
 */
extern DECLSPEC void SDLCALL DestroyViewController(MVC_ViewController *controller);

/**
 * @brief
 */
extern DECLSPEC void SDLCALL SwitchToViewController(MVC_ViewController *controller);

#endif
