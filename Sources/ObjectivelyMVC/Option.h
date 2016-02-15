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

#ifndef _ObjectivelyMVC_Option_h_
#define _ObjectivelyMVC_Option_h_

#include <SDL2/SDL_render.h>

#include <ObjectivelyMVC/Text.h>

/**
 * @file
 *
 * @brief Select Options.
 */

#define DEFAULT_OPTION_HEIGHT 16
#define DEFAULT_OPTION_PADDING 2

typedef struct Option Option;
typedef struct OptionInterface OptionInterface;

/**
 * @brief The Option type.
 *
 * @extends View
 *
 * @ingroup Controls
 */
struct Option {
	
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
	OptionInterface *interface;
	
	/**
	 * @brief The title.
	 */
	Text *title;
	
	/**
	 * @brief The value.
	 */
	ident value;
};

/**
 * @brief The Option interface.
 */
struct OptionInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;
	
	/**
	 * @fn Option *Option::initWithTitle(Option *self, const char *title, Font *font, ident value)
	 *
	 * @brief Initializes this Option with the given title, Font and value.
	 *
	 * @param title The title.
	 * @param font The Font.
	 * @param value The value.
	 *
	 * @return The initialized Option, or `NULL` on error.
	 *
	 * @memberof Option
	 */
	Option *(*initWithTitle)(Option *self, const char *title, Font *font, ident value);
};

/**
 * @brief The Option Class.
 */
extern Class _Option;

#endif /* _ObjectivelyMVC_Option_h_ */

