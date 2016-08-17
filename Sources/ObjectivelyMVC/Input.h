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
#include <ObjectivelyMVC/Label.h>
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 *
 * @brief An Input stacks a Control and Label.
 */

#define DEFAULT_INPUT_SPACING 10

/**
 * @brief Input orientation defines the relative positioning of the Label.
 */
typedef enum {
	InputOrientationLeft,
	InputOrientationRight,
	InputOrientationAbove,
	InputOrientationBelow
} InputOrientation;

typedef struct Input Input;
typedef struct InputInterface InputInterface;

/**
 * @brief An Input stacks a Control and Label.
 *
 * @extends StackView
 *
 * @ingroup Containers
 *
 * @remarks Inputs are containers used to group a Label with a Control. They simplify the
 * programmatic layout of forms, and give your application a standard, consistent aesthetic.
 */
struct Input {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	StackView stackView;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
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
	 * @brief The parent interface.
	 */
	StackViewInterface stackViewInterface;
	
	/**
	 * @fn Input *Input::initWithOrientation(Input *self, InputOrientation orientation, Control *control, Label *label)
	 *
	 * @brief Initializes this Input with the given orientation.
	 *
	 * @param orientation The InputOrientation.
	 * @param control The Control.
	 * @param Label The Label.
	 *
	 * @return The initialized Input, or `NULL` on error.
	 *
	 * @memberof Input
	 *
	 * @remarks The Label is instantiated and inserted at the specified orientation.
	 */
	Input *(*initWithOrientation)(Input *self, InputOrientation orientation, Control *control, Label *label);
};

/**
 * @brief The Input Class.
 */
extern Class _Input;
