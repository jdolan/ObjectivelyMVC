/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
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

#include <Objectively/String.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @brief Warning types.
 */
typedef enum {
  WarningTypeStyle = 1,
  WarningTypeLayout,
} WarningType;

/**
 * @file
 * @brief Warnings are accumulated on Views so that they may be reported by the DebugViewController.
 */

typedef struct Warning Warning;
typedef struct WarningInterface WarningInterface;

/**
 * @brief The Warning type.
 * @extends Object
 */
struct Warning {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  WarningInterface *interface;

  /**
   * @brief The WarningType.
   */
  WarningType type;

  /**
   * @brief The message.
   */
  String *message;
};

/**
 * @brief The Warning interface.
 */
struct WarningInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn Warning *Warning::initWithFormat(Warning *self, WarningType type, const char *fmt, ...)
   * @brief Initializes this Warning with the given type and format string.
   * @param self The Warning.
   * @param type The WarningType.
   * @param fmt The format string.
   * @return The initialized Warning, or `NULL` on error.
   * @memberof Warning
   */
  Warning *(*initWithFormat)(Warning *self, WarningType type, const char *fmt, ...);

  /**
   * @fn Warning *Warning::initWithVaList(Warning *self, WarningType type, const char *fmt, va_list args)
   * @brief Initializes this Warning with the given type and format string.
   * @param self The Warning.
   * @param type The WarningType.
   * @param fmt The format string.
   * @param args The format arguments .
   * @return The initialized Warning, or `NULL` on error.
   * @memberof Warning
   */
  Warning *(*initWithVaList)(Warning *self, WarningType type, const char *fmt, va_list args);
};

/**
 * @fn Class *Warning::_Warning(void)
 * @brief The Warning archetype.
 * @return The Warning Class.
 * @memberof Warning
 */
OBJECTIVELYMVC_EXPORT Class *_Warning(void);
