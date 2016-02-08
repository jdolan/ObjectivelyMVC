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

#ifndef _ObjectivelyMVC_Select_h_
#define _ObjectivelyMVC_Select_h_

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/Option.h>

/**
 * @file
 *
 * @brief Select Control.
 */

#define DEFAULT_SELECT_WIDTH 100

typedef struct SelectDelegate SelectDelegate;

typedef struct Select Select;
typedef struct SelectInterface SelectInterface;

/**
 * @brief The SelectDelegate protocol.
 */
struct SelectDelegate {
	
	/**
	 * @brief Called when a selection is made.
	 *
	 * @param select The Select.
	 * @param option The selected Option.
	 */
	void (*didSelect)(Select *select, Option *option);
};

/**
 * @brief The Select type.
 *
 * @extends Control
 *
 * @ingroup Controls
 */
struct Select {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	SelectInterface *interface;
	
	/**
	 * @brief The Options.
	 */
	MutableArray *options;	
};

/**
 * @brief The Select interface.
 */
struct SelectInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;
	
	/**
	 * @fn void Select::addOption(Select *self, const char *title, ident value)
	 *
	 * @brief Creates and adds a new Option to this Select.
	 *
	 * @param title The Option title.
	 * @param value The Option value.
	 *
	 * @memberof Select
	 */
	void (*addOption)(const Select *self, const char *title, ident value);
	
	/**
	 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style)
	 *
	 * @brief Initializes this Select with the specified frame and style.
	 *
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 *
	 * @return The initialized Select, or `NULL` on error.
	 *
	 * @memberof Select
	 */
	Select *(*initWithFrame)(Select *self, const SDL_Rect *frame, ControlStyle style);
	
	/**
	 * @fn void Select::removeOptionWithValue(Select *self, const ident value)
	 *
	 * @brief Removes first the Option with the given value.
	 *
	 * @param value The Option value to remove.
	 *
	 * @memberof Select
	 */
	void (*removeOptionWithValue)(const Select *self, const ident value);
};

/**
 * @brief The Select Class.
 */
extern Class _Select;

#endif /* _ObjectivelyMVC_Select_h_ */

