/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_viewcontroller_h
#define _MVC_viewcontroller_h

#include "MVC_view.h"

typedef struct _MVC_ViewController MVC_ViewController;

typedef void (*ViewControllerInit)(MVC_ViewController *self);
typedef void (*ViewControllerActivate)(MVC_ViewController *self);
typedef void (*ViewControllerDeactivate)(MVC_ViewController *self);
typedef void (*ViewControllerDestroy)(MVC_ViewController *self);

/*
 * @brief
 */
struct _MVC_ViewController {
	int tag;

	ViewControllerInit init;
	ViewControllerActivate activate;
	ViewControllerDeactivate deactivate;
	ViewControllerDestroy destroy;

	SDL_Window *window;
	MVC_View *view;
};

/*
 * @brief
 */
extern DECLSPEC MVC_ViewController * SDLCALL MVC_CreateViewController(void);

/*
 * @brief
 */
extern DECLSPEC void SDLCALL MVC_DestroyViewController(MVC_ViewController *controller);

/*
 * @brief
 */
extern DECLSPEC void SDLCALL MVC_SwitchToViewController(MVC_ViewController *controller);

#endif
