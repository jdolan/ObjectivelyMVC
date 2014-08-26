/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @file SDL_mvc.c
 * @author jdolan
 */

#include "SDL_mvc.h"

int MVC_Init(void) {
	return 0;
}

void MVC_Quit(void) {

}

MVC_ViewController *MVC_CreateViewController(const char *name) {

	MVC_ViewController *controller = calloc(sizeof(MVC_ViewController));

	controller->name = strdup(name);

	return controller;
}

void MVC_DestroyViewController(MVC_ViewController *controller) {

	if (controller) {

		if (controller->name) {
			free(controller->name);
		}

		if (controller->destroy) {
			controller->destroy(controller);
		}
	}

}
