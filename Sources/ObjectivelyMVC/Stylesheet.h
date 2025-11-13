/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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
#include <Objectively/Resource.h>

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
   * @brief The Selectors, ordered by specificity.
   */
  Array *selectors;

  /**
   * @brief The Styles, keyed by Selector.
   */
  Array *styles;
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
   * @remarks Designated initializer.
   * @memberof Stylesheet
   */
  Stylesheet *(*initWithCharacters)(Stylesheet *self, const char *chars);

  /**
   * @fn Stylesheet *Stylesheet::initWithData(Stylesheet *self, const Data *data)
   * @brief Initializes this Stylesheet with the CSS definitions in `data`.
   * @param self The Stylesheet.
   * @param data The Data containing CSS definitions.
   * @return The initialized Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*initWithData)(Stylesheet *self, const Data *data);

  /**
   * @fn Stylesheet *Stylesheet::initWithResource(Stylsheet *self, const Resource *resource)
   * @brief Initializes this Stylsheet with the CSS definitions in `resource`.
   * @param self The Stylesheet.
   * @param resource The Resource containing CSS definitions.
   * @return The initialized Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*initWithResource)(Stylesheet *self, const Resource *resource);

  /**
   * @fn Stylesheet *Stylesheet::initWithResourceName(Stylsheet *self, const char *name)
   * @brief Initializes this Stylsheet with the Resource by the specified `name`.
   * @param self The Stylesheet.
   * @param name The name of a Resource containing CSS definitions.
   * @return The initialized Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*initWithResourceName)(Stylesheet *self, const char *name);

  /**
   * @fn Stylesheet *Stylesheet::initWithString(Stylesheet *self, const String *string)
   * @brief Initializes this Stylesheet with the CSS definitions in `string`.
   * @param self The Stylesheet.
   * @param string The String containing CSS definitions.
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
   * @fn Stylesheet *Stylesheet::stylesheetWithCharacters(const Data *data)
   * @brief Instantiates a new Stylesheet with the given CSS Data.
   * @param data The Data containing CSS definitions.
   * @return The new Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*stylesheetWithData)(const Data *data);

  /**
   * @static
   * @fn Stylesheet *Stylesheet::stylesheetWithResource(const Resource *resource)
   * @brief Instantiates a new Stylesheet with the given CSS Resource.
   * @param resource The Resource containing CSS definitions.
   * @return The new Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*stylesheetWithResource)(const Resource *resource);

  /**
   * @static
   * @fn Stylesheet *Stylesheet::stylesheetWithResourceName(const char *name)
   * @brief Instantiates a new Stylesheet with the CSS Resource by the specified `name`.
   * @param name The name of a Resource containing CSS definitions.
   * @return The new Stylesheet, or `NULL` on error.
   * @memberof Stylesheet
   */
  Stylesheet *(*stylesheetWithResourceName)(const char *name);

  /**
   * @static
   * @fn Stylesheet *Stylesheet::stylesheetWithString(const String *string)
   * @brief Instantiates a new Stylesheet with the given CSS String.
   * @param string The String of containing CSS definitions.
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
