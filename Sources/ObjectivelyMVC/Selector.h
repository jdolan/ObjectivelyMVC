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

/**
 * @file
 * @brief The Selector type.
 */

typedef struct Selector Selector;
typedef struct SelectorInterface SelectorInterface;

/**
 * @brief The Selector type.
 * @extends Object
 * @ingroup Theme
 */
struct Selector {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SelectorInterface *interface;

	/**
	 * @brief The attribute component.
	 */
	char *attribute;

	/**
	 * @brief The identifier component.
	 */
	char *identifier;

	/**
	 * @brief The rule.
	 */
	char *rule;

	/**
	 * @brief The type component.
	 */
	char *type;
};

/**®
 * @brief The Selector interface.
 */
struct SelectorInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Selector *Selector::initWithRule(Selector *self, const char *rule)
	 * @brief Initializes this Selector with the given rule.
	 * @param self The Selector.
	 * @param rule The rule.
	 * @return The initialized Selector, or `NULL` on error.
	 * @memberof Selector
	 */
	Selector *(*initWithRule)(Selector *self, const char *rule);

	/**
	 * @static
	 * @fn Array *Selector::parse(const char *rules)
	 * @brief Parses the null-terminated C string of Selector rules into an Array of Selectors.
	 * @param chars A null-terminated C string of Selector rules.
	 * @return An Array of Selectors.
	 * @memberof Selector
	 */
	Array *(*parse)(const char *rules);
};

/**
 * @fn Class *Selector::_Selector(void)
 * @brief The Selector archetype.
 * @return The Selector Class.
 * @memberof Selector
 */
OBJECTIVELY_EXPORT Class *_Selector(void);
