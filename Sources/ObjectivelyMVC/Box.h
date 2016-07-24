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

#pragma once

#include <ObjectivelyMVC/Label.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 *
 * @brief A container View with a positioned label.
 */

#define DEFAULT_BOX_PADDING 10
#define DEFAULT_BOX_LABEL_X 20

typedef struct Box Box;
typedef struct BoxInterface BoxInterface;

/**
 * @brief The Box type.
 *
 * @extends View
 *
 * @ingroup Containers
 */
struct Box {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	View view;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	BoxInterface *interface;

	/**
	 * @brief The label.
	 */
	Label *label;
};

/**
 * @brief The Box interface.
 */
struct BoxInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;
	
	/**
	 * @fn Box *Box::initWithFrame(Box *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this Box.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized Box, or `NULL` on error.
	 *
	 * @memberof Box
	 */
	Box *(*initWithFrame)(Box *self, const SDL_Rect *frame);
};

/**
 * @brief The Box Class.
 */
extern Class _Box;

