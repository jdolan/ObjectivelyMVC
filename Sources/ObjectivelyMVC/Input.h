/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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
#include <ObjectivelyMVC/Label.h>
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 * @brief An Input stacks a Control and Label.
 */

/**
 * @brief Input orientation defines the relative positioning of the Label.
 */
typedef enum {
	InputOrientationLeft,
	InputOrientationRight,
	InputOrientationAbove,
	InputOrientationBelow
} InputOrientation;

OBJECTIVELYMVC_EXPORT const EnumName InputOrientationNames[];

typedef struct Input Input;
typedef struct InputInterface InputInterface;

/**
 * @brief An Input stacks a Label with a Control.
 * @extends StackView
 * @ingroup Containers
 * @remarks Inputs are containers used to group a Text label with a Control. They simplify the
 * programmatic layout of forms, and give your application a standard, consistent aesthetic.
 */
struct Input {

	/**
	 * @brief The superclass.
	 */
	StackView stackView;

	/**
	 * @brief The interface.
	 * @protected
	 */
	InputInterface *interface;

	/**
	 * @brief The Control.
	 */
	Control *control;

	/**
	 * @brief The Label.
	 */
	Label *label;

	/**
	 * @brief The Label orientation.
	 */
	InputOrientation orientation;
};

/**
 * @brief The Input interface.
 */
struct InputInterface {

	/**
	 * @brief The superclass interface.
	 */
	StackViewInterface viewInterface;

	/**
	 * @fn Input *Input::initWithFrame(Input *self, const SDL_Rect *frame)
	 * @brief Initializes this Input with the specified frame.
	 * @param self The Input.
	 * @param frame The frame.
	 * @return The initialized Input, or `NULL` on error.
	 * @memberof Input
	 */
	Input *(*initWithFrame)(Input *self, const SDL_Rect *frame);

	/**
	 * @fn void Input::setControl(Input *self, Control *control)
	 * @brief Sets this Input's Control.
	 * @param self The Input.
	 * @param control The Control.
	 * @memberof Input
	 */
	void (*setControl)(Input *self, Control *control);

	/**
	 * @fn void Input::setLabel(Input *self, Label *label)
	 * @brief Sets this Input's Label.
	 * @param self The Input.
	 * @param label The Label.
	 * @memberof Input
	 */
	void (*setLabel)(Input *self, Label *label);

	/**
	 * @fn void Input::setOrientation(Input *self, InputOrientation orientation)
	 * @brief Sets this Input's orientation.
	 * @param self The Input.
	 * @param orientation The InputOrientation.
	 * @memberof Input
	 */
	void (*setOrientation)(Input *self, InputOrientation orientation);
};

/**
 * @fn Class *Input::_Input(void)
 * @brief The Input archetype.
 * @return The Input Class.
 * @memberof Input
 */
OBJECTIVELYMVC_EXPORT Class *_Input(void);
