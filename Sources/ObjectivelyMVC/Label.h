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

#include <ObjectivelyMVC/Text.h>

/**
 * @file
 * @brief The Label type.
 */

typedef struct Label Label;
typedef struct LabelInterface LabelInterface;

/**
 * @brief Labels provide a configurable container for Text.
 * @extends View
 */
struct Label {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	LabelInterface *interface;

	/**
	 * @brief The Label Text.
	 */
	Text *text;
};

/**
 * @brief The Label interface.
 */
struct LabelInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn Label *Label::initWithText(Label *self, const char *text, Font *font)
	 * @brief Initializes this Label with the given text and Font.
	 * @param self The Label.
	 * @param text The text.
	 * @param font The Font.
	 * @return The initialized Label, or `NULL` on error.
	 * @memberof Label
	 */
	Label *(*initWithText)(Label *self, const char *text, Font *font);
};

/**
 * @fn Class *Label::_Label(void)
 * @brief The Label archetype.
 * @return The Label Class.
 * @memberof Label
 */
OBJECTIVELYMVC_EXPORT Class *_Label(void);
