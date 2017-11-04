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

/**
 * @file
 * @brief The Theme type.
 */

/**
 * @defgroup Theme Theming
 * @brief Themes provides aggregation, management and application of Styles.
 * @details Theming is a core part of the layout process. A Theme manages a collection
 * of Styles. Prior to layout, the Styles of the current Theme are matched against and
 * applied to the View hierarchy.
 */

typedef struct Theme Theme;
typedef struct ThemeInterface ThemeInterface;

/**
 * @brief The Theme type.
 * @extends Object
 * @ingroup Theme
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
	 * @brief The Selectors, ordered by specificity.
	 */
	MutableArray *selectors;

	/**
	 * @brief The Styles, keyed by Selector.
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
	 * @fn void Theme::apply(const Theme *self, View *view)
	 * @brief Applies this Theme to the given View.
	 * @param self The Theme.
	 * @param view The View.
	 * @memberof Theme
	 */
	void (*apply)(const Theme *self, View *view);

	/**
	 * @fn Theme *Theme::init(Theme *self)
	 * @brief Initializes this Theme.
	 * @param self The Theme.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*init)(Theme *self);

	/**
	 * @fn void Theme::removeStyle(Theme *self, Style *style)
	 * @brief Removes the given Style from this Theme.
	 * @param self The Theme.
	 * @param style The Style.
	 * @memberof Theme
	 */
	void (*removeStyle)(Theme *self, Style *style);

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
