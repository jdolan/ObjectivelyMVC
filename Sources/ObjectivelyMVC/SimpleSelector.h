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

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief The SimpleSelector type.
 */

/**
 * @brief The types of SimpleSelectors.
 */
typedef enum {
  SimpleSelectorTypeNone,
  SimpleSelectorTypeUniversal,
  SimpleSelectorTypeType,
  SimpleSelectorTypeClass,
  SimpleSelectorTypeId,
  SimpleSelectorTypePseudo
} SimpleSelectorType;

typedef struct SimpleSelector SimpleSelector;
typedef struct SimpleSelectorInterface SimpleSelectorInterface;

/**
 * @brief The SimpleSelector type.
 * @extends Object
 * @ingroup Theme
 */
struct SimpleSelector {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  SimpleSelectorInterface *interface;

  /**
   * @brief The pattern, as provided by the user.
   */
  char *pattern;

  /**
   * @brief The SimpleSelectorType.
   */
  SimpleSelectorType type;
};

/**
 * @brief The SimpleSelector interface.
 */
struct SimpleSelectorInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn SimpleSelector *SimpleSelector::initWithPattern(SimpleSelector *self, const char *pattern)
   * @brief Initializes this SimpleSelector.
   * @param self The SimpleSelector.
   * @param pattern The pattern.
   * @return The initialized SimpleSelector, or `NULL` on error.
   * @memberof SimpleSelector
   */
  SimpleSelector *(*initWithPattern)(SimpleSelector *self, const char *pattern);

  /**
   * @static
   * @fn Array *SimpleSelector::parse(const char *sequence)
   * @param sequence A C-string specifying a sequence of SimpleSelectors.
   * @return The parsed SimpleSelectors.
   * @memberof SimpleSelector
   */
  Array *(*parse)(const char *sequence);
};

/**
 * @fn Class *SimpleSelector::_SimpleSelector(void)
 * @brief The SimpleSelector archetype.
 * @return The SimpleSelector Class.
 * @memberof SimpleSelector
 */
OBJECTIVELYMVC_EXPORT Class *_SimpleSelector(void);
