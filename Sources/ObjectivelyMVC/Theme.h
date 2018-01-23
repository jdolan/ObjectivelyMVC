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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Stylesheet.h>

/**
 * @file
 * @brief The Theme type.
 */

/**
 * @defgroup Theme Theming
 * @brief A Theme is a collection of one or more Stylesheets, ordered by priority.
 * @details Theming is a core part of the layout process, managed automatically by the
 * WindowController. In general, applications should not call the Theme API directly. Instead,
 * custom theming can be achieved by adding a Stylesheet to any Theme instance.
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
	 * @brief The Stylesheets, in order of priority.
	 */
	MutableArray *stylesheets;
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
	 * @fn void Theme::addStylesheet(Theme *self, Stylesheet *stylesheet)
	 * @brief Adds the specified Stylesheet to this Theme.
	 * @param self The Theme.
	 * @param stylesheet The Stylesheet.
	 * @memberof Theme
	 */
	void (*addStylesheet)(Theme *self, Stylesheet *stylesheet);

	/**
	 * @fn void Theme::addStylesheetWithIdentifier(Theme *self, Stylesheet *stylesheet, const char *identifier)
	 * @brief Adds the specified Stylesheet to this Theme.
	 * @param self The Theme.
	 * @param stylesheet The Stylesheet.
	 * @param identifier The identifier.
	 * @memberof Theme
	 */
	void (*addStylesheetWithIdentifier)(Theme *self, Stylesheet *stylesheet, const char *identifier);

	/**
	 * @fn void Theme::apply(const Theme *self, const View *view)
	 * @brief Applies this Theme to the given View.
	 * @param self The Theme.
	 * @param view The View.
	 * @memberof Theme
	 */
	Style *(*computeStyle)(const Theme *self, const View *view);

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
	 * @param self The Theme.
	 * @return The initialized Theme, or `NULL` on error.
	 * @memberof Theme
	 */
	Theme *(*init)(Theme *self);

	/**
	 * @fn void Theme::removeStylesheet(Theme *self, Stylesheet *stylesheet)
	 * @brief Removes the given Stylesheet from this Theme.
	 * @param self The Theme.
	 * @param stylesheet The Stylesheet.
	 * @memberof Theme
	 */
	void (*removeStylesheet)(Theme *self, Stylesheet *stylesheet);

	/**
	 * @fn void Theme::removeStylesheetWithIdentifier(Theme *self, const char *identifier)
	 * @brief Removes the first Stylesheet with the given identifier from this Theme.
	 * @param self The Theme.
	 * @param identifier The identifier.
	 * @memberof Theme
	 */
	void (*removeStylesheetWithIdentifier)(Theme *self, const char *identifier);

	/**
	 * @fn Stylesheet *Theme::stylesheetWithIdentifier(const Theme *self, const char *identifier)
	 * @param self The Theme.
	 * @param identifier The identifier.
	 * @return The first Stylesheet with the given identifier in this Theme.
	 * @memberof Theme
	 */
	Stylesheet *(*stylesheetWithIdentifier)(const Theme *self, const char *identifier);
};

/**
 * @fn Class *Theme::_Theme(void)
 * @brief The Theme archetype.
 * @return The Theme Class.
 * @memberof Theme
 */
OBJECTIVELYMVC_EXPORT Class *_Theme(void);
