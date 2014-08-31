/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_view_h
#define _MVC_view_h

#include "MVC_stdinc.h"
#include "MVC_color.h"

/*
 * @brief
 */
Interface(MVC_View, Object)

	MVC_View *superview;
	GList *subviews;

	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Rect frame, bounds;
	SDL_bool hidden;

	SDL_Color backgroundColor;

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
Constructor(MVC_View, SDL_Rect *frame);

#endif
