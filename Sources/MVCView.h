/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_view_h_
#define _MVC_view_h_

#include <Objectively/Array.h>
#include <SDL2/SDL_opengl.h>

#include "MVC_color.h"

/**
 * @file
 *
 * @brief MVC View implementation.
 */

typedef struct MVC_View MVC_View;
typedef struct MVC_ViewInterface MVC_ViewInterface;

/**
 * @brief The View type.
 */
struct MVC_View {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	MVC_ViewInterface *interface;

	/**
	 * @brief The containing view.
	 */
	MVC_View *superview;

	/**
	 * @brief All contained views.
	 */
	Array *subviews;

	/**
	 * @brief The window.
	 */
	SDL_Window *window;

	/**
	 * @brief The renderer.
	 */
	SDL_Renderer *renderer;

	/**
	 * @brief The frame and bounds.
	 */
	SDL_Rect frame, bounds;

	/**
	 * @brief If `YES`, this view is not drawn.
	 */
	SDL_bool hidden;

	/**
	 * @brief The background color.
	 */
	SDL_Color backgroundColor;
};

/**
 * @brief The View interface.
 */
struct MVC_ViewInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;

	/**
	 * @brief Initializes this view with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized view, or NULL on error.
	 */
	MVC_View *(*initWithFrame)(MVC_View *self, SDL_Rect *frame);

	/**
	 * @brief Adds a subview to this view, to be drawn above its siblings.
	 *
	 * @param subview The subview.
	 */
	void (*addSubview)(MVC_View *self, MVC_View *subview);
	void (*removeSubview)(MVC_View *self, MVC_View *subview);
	void (*removeFromSuperview)(MVC_View *self);

	void (*draw)(MVC_View *self);

	void (*activate)(MVC_View *self);
	void (*deactivate)(MVC_View *self);
	void (*respondToEvent)(MVC_View *self, SDL_Event *event, SDL_bool *cancel);
};

extern Class __MVC_View;

#endif
