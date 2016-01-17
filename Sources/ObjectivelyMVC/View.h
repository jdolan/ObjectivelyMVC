/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef _ObjectivelyMVC_View_h_
#define _ObjectivelyMVC_View_h_

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Colors.h>

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
	MutableArray *subviews;

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

extern Class _View;

#endif
