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

#include <Objectively/Dictionary.h>

#include <ObjectivelyMVC/Types.h>

#define DEFAULT_CONSTRAINT_PRIORITY 100

/**
 * @file
 * @brief Constraints define relationships between Views that should be satisfied at layout.
 */

typedef enum {
	ConstraintAttributeNone,
	ConstraintAttributeWidth,
	ConstraintAttributeHeight,
	ConstraintAttributeCenter,
	ConstraintAttributeTop,
	ConstraintAttributeRight,
	ConstraintAttributeBottom,
	ConstraintAttributeLeft
} ConstraintAttribute;

typedef enum {
	ConstraintRelationNone,
	ConstraintRelationLessThan,
	ConstraintRelationLessThanOrEqual,
	ConstraintRelationEqual,
	ConstraintRelationGreaterThan,
	ConstraintRelationGreaterThanOrEqual
} ConstraintRelation;

typedef struct Constraint Constraint;
typedef struct ConstraintInterface ConstraintInterface;

/**
 * @brief The Constraint type.
 * @details Constraints bind a layout attribute of one View to that of another, or to a constant. 
 *
 * A Constraint specifying a minimum width of 500 pixels:
 *   `"w >= 500"`
 *
 * A Constraint specifying the width of the the ancestor _stackView_, minus `20` pixels.
 *   `"w = stackView.w - 20"`
 *
 * A Constraint specifying half the height of the ancestor _panel_, minus `50` pixels.
 *   `"h = panel.h * 0.5 - 50"`
 *
 * The same Constraint, with a priority of `10` (default `100`).
 *   `"h = panel.h * 0.5 - 50 [10]"`
 *
 * @extends Object
 */
struct Constraint {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ConstraintInterface *interface;

	/**
	 * @brief The constant offset or limit.
	 */
	float constant;

	/**
	 * @brief The descriptor.
	 */
	char *descriptor;

	/**
	 * @brief The identifier of the View enforcing this Constraint.
	 */
	char *identifier;

	/**
	 * @brief The multiplier.
	 */
	float multiplier;

	/**
	 * @brief The priority.
	 */
	int priority;

	/**
	 * @brief The relation by which to constrain.
	 */
	ConstraintRelation relation;

	/**
	 * @brief The attribute to constrain against.
	 */
	ConstraintAttribute source;

	/**
	 * @brief The attribute to constrain.
	 */
	ConstraintAttribute target;
};

/**
 * @brief The Constraint interface.
 */
struct ConstraintInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Constraint::apply(const Constraint *self, View *source)
	 * @brief Applies this Constraint on the source View.
	 * @param self The Constraint.
	 * @param source The source View.
	 * @memberof Constraint
	 */
	void (*apply)(const Constraint *self, View *source);

	/**
	 * @fn Constraint *Constraint::initWithDescriptor(Constraint *self, const char *descriptor)
	 * @brief Initializes this Constraint with the specified descriptor.
	 * @param self The Constraint.
	 * @param descriptor The Constraint descriptor.
	 * @return The initialized Constraint, or `NULL` on error.
	 * @memberof Constraint
	 */
	Constraint *(*initWithDescriptor)(Constraint *self, const char *descriptor);
};

/**
 * @fn Class *Constraint::_Constraint(void)
 * @brief The Constraint archetype.
 * @return The Constraint Class.
 * @memberof Constraint
 */
OBJECTIVELY_EXPORT Class *_Constraint(void);
