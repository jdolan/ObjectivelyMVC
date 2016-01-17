/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_View_h_
#define _ObjectivelyMVC_View_h_

#include <Objectively/Array.h>

#include <SDL2/SDL.h>

/**
 * @file
 *
 * @brief MVC View implementation.
 */

typedef struct View View;
typedef struct ViewInterface ViewInterface;

/**
 * @brief The View type.
 */
struct View {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	ViewInterface *interface;

	/**
	 * @brief The containing view.
	 */
	View *superview;

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
	 * @brief If `true`, this view is not drawn.
	 */
	_Bool hidden;

	/**
	 * @brief The background color.
	 */
	SDL_Color backgroundColor;
};

/**
 * @brief The View interface.
 */
struct ViewInterface {

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
	View *(*initWithFrame)(View *self, SDL_Rect *frame);

	/**
	 * @brief Adds a subview to this view, to be drawn above its siblings.
	 *
	 * @param subview The subview.
	 */
	void (*addSubview)(View *self, View *subview);
	void (*removeSubview)(View *self, View *subview);
	void (*removeFromSuperview)(View *self);

	void (*draw)(View *self);

	void (*activate)(View *self);
	void (*deactivate)(View *self);
	void (*respondToEvent)(View *self, SDL_Event *event, SDL_bool *cancel);
};

extern Class __View;

#endif
