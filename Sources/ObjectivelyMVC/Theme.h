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

#include <ObjectivelyMVC/Style.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief The Theme type.
 */

typedef struct Theme Theme;
typedef struct ThemeInterface ThemeInterface;

/**
 * @brief A Theme consists of one or more Styles, defined in one or more JSON stylesheets.
 * @extends Object
 */
struct Theme {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ThemeInterface *interface;

	/**
	 * @brief The styles.
	 */
	MutableDictionary *styles;
};

/**
 * @brief The Theme interface.
 */
struct ThemeInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Theme::addStyle(Theme *self, Style *style)
	 * @brief Adds the given Style to this Theme.
	 * @param self The Theme.
	 * @param style The Style
	 * @memberof Theme
	 */
	void (*addStyle)(Theme *self, Style *style);

	/**
	 * @fn void Theme::addStylesheet(Theme *self, const Dictionary *stylesheet)
	 * @brief Adds all Styles from the parsed JSON stylesheet to this Theme.
	 * @param self The Theme.
	 * @param stylesheet The parsed JSON stylesheet.
	 * @memberof Theme
	 */
	void (*addStylesheet)(Theme *self, const Dictionary *stylesheet);

	/**
	 * @static
	 * @fn Theme *Theme::currentTheme(void)
	 * @return The current Theme.
	 * @memberof Theme
	 */
	Theme *(*currentTheme)(void);

	/**
	 * @static
	 * @fn Theme *Theme::defaultTheme(void)
	 * @return The default Theme.
	 * @memberof Theme
	 */
	Theme *(*defaultTheme)(void);

	/**
	 * @fn Theme *Theme::init(Theme *self)
	 * @brief Initializes this Theme.
	 * @param theme The Theme.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*init)(Theme *self);

	/**
	 * @static
	 * @fn void Theme::setCurrentTheme(Theme *theme)
	 * @brief Sets the current Theme.
	 * @param theme The Theme.
	 * @memberof Theme
	 */
	void (*setCurrentTheme)(Theme *theme);

	/**
	 * @static
	 * @fn Theme *Theme::themeWithContentsOfFile(const char *path)
	 * @brief Instantiates a Theme initialized with the contents of the JSON file at `path`.
	 * @param path A path to a JSON stylesheet.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*themeWithContentsOfFile)(const char *path);

	/**
	 * @static
	 * @fn Theme *Theme::themeWithData(const Data *data)
	 * @brief Instantiates a Theme initialized with the contents of `data`.
	 * @param data A Data containing a JSON stylesheet.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*themeWithData)(const Data *data);

	/**
	 * @static
	 * @fn Theme *Theme::themeWithDictionary(const Dictionary *dictionary)
	 * @brief Instantiates a Theme initialized with the parsed JSON stylesheet.
	 * @param dictionary A parsed JSON stylesheet.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*themeWithDictionary)(const Dictionary *dictionary);
};

/**
 * @fn Class *Theme::_Theme(void)
 * @brief The Theme archetype.
 * @return The Theme Class.
 * @memberof Theme
 */
OBJECTIVELY_EXPORT Class *_Theme(void);
