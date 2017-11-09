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
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 * @brief A container View with a positioned Label.
 */

typedef struct Box Box;
typedef struct BoxInterface BoxInterface;

/**
 * @brief A container View with a positioned Label.
 * @extends View
 * @ingroup Containers
 * @remarks Boxes are container Views with a positioned label. Boxes are useful when grouping
 * Controls, Inputs or custom Views within a larger View (e.g. Panel).
 */
struct Box {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	BoxInterface *interface;

	/**
	 * @brief The internal container.
	 */
	StackView *contentView;

	/**
	 * @brief The Label.
	 */
	Label *label;
};

/**
 * @brief The Box interface.
 */
struct BoxInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn Box *Box::initWithFrame(Box *self, const SDL_Rect *frame)
	 * @brief Initializes this Box with the given frame.
	 * @param self The Box.
	 * @param frame The frame.
	 * @return The initialized Box, or `NULL` on error.
	 * @memberof Box
	 */
	Box *(*initWithFrame)(Box *self, const SDL_Rect *frame);
};

/**
 * @fn Class *Box::_Box(void)
 * @brief The Box archetype.
 * @return The Box Class.
 * @memberof Box
 */
OBJECTIVELYMVC_EXPORT Class *_Box(void);

