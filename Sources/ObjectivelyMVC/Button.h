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

/**
 * @file
 * @brief Buttons are Controls that respond to click events.
 */

typedef struct Button Button;
typedef struct ButtonInterface ButtonInterface;

#define DEFAULT_BUTTON_MIN_WIDTH 100

/**
 * @brief Buttons are Controls that respond to click events.
 * @extends Control
 * @ingroup Controls
 */
struct Button {

	/**
	 * @brief The parent.
	 */
	Control control;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	ButtonInterface *interface;
	
	/**
	 * @brief The title.
	 */
	Text *title;
};

/**
 * @brief The Button interface.
 */
struct ButtonInterface {

	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn Button *Button::initWithFrame(Button *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this Button with the specified frame and style.
	 * @param self The Button.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized Button, or `NULL` on error.
	 * @memberof Button
	 */
	Button *(*initWithFrame)(Button *self, const SDL_Rect *frame, ControlStyle style);
};

/**
 * @brief The Button Class.
 */
extern Class _Button;
