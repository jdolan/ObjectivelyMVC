/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "SDL_view.h"

MVC_View *MVC_CreateView(const char *name) {

	MVC_View *view = malloc(sizeof(MVC_View));
	memset(view, 0, sizeof(*view));

	if (name) {
		view->name = strdup(name);
	}

	return view;
}

void MVC_DestroyView(MVC_View *view) {

	if (view) {

		if (view->destroy) {
			view->destroy(view);
		}

		if (view->name) {
			free(view->name);
		}

		free(view);
	}
}
