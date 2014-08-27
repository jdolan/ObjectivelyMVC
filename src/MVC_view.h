/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_view_h
#define _MVC_view_h

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>

#include "MVC_stdinc.h"
#include "MVC_color.h"
#include "MVC_object.h"

/*
 * @brief
 */
Interface(MVC_View, MVC_Object)

	struct {
		MVC_View *superview;
		GList *subviews;
	} hierarchy;

	struct {
		SDL_Rect frame, bounds;

		SDL_bool hidden;
		SDL_bool needsDisplay;

		SDL_Color backgroundColor;
	} display;

	void (*addSubview)(MVC_View *self, MVC_View *subview);
	void (*removeSubview)(MVC_View *self, MVC_View *subview);
	void (*removeFromSuperview)(MVC_View *self);

	void (*draw)(MVC_View *self);

	void (*activate)(MVC_View *self);
	void (*deactivate)(MVC_View *self);
	void (*respondToEvent)(MVC_View *self, SDL_Event *event, SDL_bool *cancel);

End

/*
 * @brief
 */
Constructor(MVC_View);

#endif
