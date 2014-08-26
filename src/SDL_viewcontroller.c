/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "SDL_mvc.h"

MVC_ViewController *MVC_CreateViewController(const char *name) {

	MVC_ViewController *controller = malloc(sizeof(MVC_ViewController));
	memset(controller, 0, sizeof(*controller));

	controller->name = strdup(name);

	return controller;
}

void MVC_DestroyViewController(MVC_ViewController *controller) {

	if (controller) {

		if (controller->destroy) {
			controller->destroy(controller);
		}

		if (controller->name) {
			free(controller->name);
		}

		free(controller);
	}
}
