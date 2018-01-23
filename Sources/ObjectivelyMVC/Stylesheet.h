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

#include <Objectively/Array.h>
#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>

#include <ObjectivelyMVC/Style.h>

/**
 * @file
 * @brief The Stylesheet type.
 */

typedef struct Stylesheet Stylesheet;
typedef struct StylesheetInterface StylesheetInterface;

/**
 * @brief Stylesheets are comprised of Selectors and Styles.
 * @extends Object
 */
struct Stylesheet {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	StylesheetInterface *interface;

	/**
	 * @brief An optional identifier.
	 */
	char *identifier;

	/**
	 * @brief The Selectors, ordered by specificity.
	 */
	Array *selectors;

	/**
	 * @brief The Styles, keyed by Selector.
	 */
	Dictionary *styles;
};

/**
 * @brief The Stylesheet interface.
 */
struct StylesheetInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Stylesheet *Stylesheet::defaultStylesheet(void)
	 * @return The default Stylesheet.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*defaultStylesheet)(void);

	/**
	 * @fn Stylesheet *Stylesheet::initWithCharacters(Stylesheet *self, const char *chars)
	 * @brief Initializes this Stylesheet with the given CSS definitions.
	 * @param self The Stylesheet.
	 * @param chars The null-terminated C string of CSS definitions.
	 * @return The initialized Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*initWithCharacters)(Stylesheet *self, const char *chars);

	/**
	 * @fn Stylesheet *Stylesheet::initWithContentsOfFile(Stylesheet *self, const char *path, StringEncoding encoding)
	 * @brief Initializes this Stylesheet with the contents of the FILE at `path`.
	 * @param self The Stylesheet.
	 * @param path The path name to a FILE containing CSS definitions.
	 * @param encoding The StringEncoding.
	 * @return The initialized Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*initWithContentsOfFile)(Stylesheet *self, const char *path, StringEncoding encoding);

	/**
	 * @fn Stylesheet *Stylesheet::initWithString(Stylsheet *self, const String *string)
	 * @brief Initializes this Stylsheet with the given CSS definitions.
	 * @param self The Stylesheet.
	 * @param string The String of CSS definitions.
	 * @return The initialized Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*initWithString)(Stylesheet *self, const String *string);

	/**
	 * @static
	 * @fn Stylesheet *Stylesheet::stylesheetWithCharacters(const char *chars)
	 * @brief Instantiates a new Stylesheet with the given CSS definitions.
	 * @param chars The null-terminated C string of CSS definitions.
	 * @return The new Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*stylesheetWithCharacters)(const char *chars);

	/**
	 * @static
	 * @fn Stylesheet *Stylesheet::stylesheetWithContentsOfFile(const char *path, StringEncoding encoding)
	 * @brief Instantiates a new Stylesheet with the contents of the FILE at `path`.
	 * @param path The path name to a FILE containing CSS definitions.
	 * @param encoding The StringEncoding.
	 * @return The new Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*stylesheetWithContentsOfFile)(const char *path, StringEncoding encoding);

	/**
	 * @static
	 * @fn Stylesheet *Stylesheet::stylesheetWithString(const String *string)
	 * @brief Instantiates a new Stylesheet with the given CSS definitions.
	 * @param string The String of CSS definitions.
	 * @return The new Stylesheet, or `NULL` on error.
	 * @memberof Stylesheet
	 */
	Stylesheet *(*stylesheetWithString)(const String *string);
};

/**
 * @fn Class *Stylesheet::_Stylesheet(void)
 * @brief The Stylesheet archetype.
 * @return The Stylesheet Class.
 * @memberof Stylesheet
 */
OBJECTIVELYMVC_EXPORT Class *_Stylesheet(void);
