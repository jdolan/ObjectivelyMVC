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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/Text.h>

/**
 * @file
 * @brief Checkboxes are toggle Controls that respond to click events.
 */

typedef struct Checkbox Checkbox;
typedef struct CheckboxInterface CheckboxInterface;

/**
 * @brief Checkboxes are toggle Controls that respond to click events.
 * @extends Control
 * @ingroup Controls
 */
struct Checkbox {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	CheckboxInterface *interface;

	/**
	 * @brief The box.
	 */
	Control *box;

	/**
	 * @brief The check.
	 */
	ImageView *check;
};

/**
 * @brief The Checkbox interface.
 */
struct CheckboxInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controltInterface;

	/**
	 * @fn Checkbox *Checkbox::initWithFrame(Checkbox *self, const SDL_Frame *frame, ControlStyle style)
	 * @brief Initializes this Checkbox with the specified frame and style.
	 * @param self The Checkbox.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized Checkbox, or `NULL` on error.
	 * @memberof Checkbox
	 */
	Checkbox *(*initWithFrame)(Checkbox *self, const SDL_Rect *frame, ControlStyle style);
};

/**
 * @fn Class *Checkbox::_Checkbox(void)
 * @brief The Checkbox archetype.
 * @return The Checkbox Class.
 * @memberof Checkbox
 */
OBJECTIVELYMVC_EXPORT Class *_Checkbox(void);
