/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_viewcontroller.h"

MVC_ViewController *MVC_CreateViewController(void) {

	MVC_ViewController *controller = g_malloc0(sizeof(MVC_ViewController));

	return controller;
}

void MVC_DestroyViewController(MVC_ViewController *controller) {

	if (controller) {

		if (controller->destroy) {
			controller->destroy(controller);
		}

		g_free(controller);
	}
}
