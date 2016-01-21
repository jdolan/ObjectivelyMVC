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

#ifndef _ObjectivelyMVC_Constraint_h_
#define _ObjectivelyMVC_Constraint_h_

#include <Objectively/Object.h>

/**
 * @file
 * 
 * @brief Layout Constraints.
 */

/**
 * @brief Constraint types.
 */
typedef enum {
	Top,
	Right,
	Bottom,
	Left,
	Width,
	Height
} Attribute;

typedef struct View View;
typedef struct ViewInterface ViewInterface;


typedef struct Constraint Constraint;
typedef struct ConstraintInterface ConstraintInterface;

/**
 * @brief The Constraint type.
 */
struct Constraint {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	ConstraintInterface *interface;
	
	/**
	 * @brief The minimum allowed value.
	 *
	 */
	int max;
	
	/**
	 * @brief The maximum allowed value.
	 */
	int min;
	
	/**
	 * @brief The offset.
	 */
	int offset;

	/**
	 * @brief The source View.
	 */
	View *source;
	
	/**
	 * @brief The source Attribute.
	 */
	Attribute sourceAttribute;
	
	/**
	 * @brief The target View.
	 */
	View *target;

	/**
	 * @brief The target Attribute.
	 */
	Attribute targetAttribute;
};

/**
 * @brief The Constraint interface.
 */
struct ConstraintInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @fn void Constraing::apply(const Constraint *self)
	 *
	 * @memberof Constraint
	 */
	void (*apply)(const Constraint *self);
	
	/**
	 * @fn void Constraint::bind(Constraint *self, View *target, Attribute targetAttribute, int offset)
	 *
	 * @brief Binds this Constraint to the specified target View.
	 *
	 * @param target The target View.
	 * @param targetAttribute The target Attribute.
	 * @param offset The offset.
	 *
	 * @memberof Constraint
	 */
	void (*bind)(Constraint *self, View *target, Attribute targetAttribute, int offset);

	/**
	 * @fn Constraint *Constraint::initWithSource(Constraint *self, View *source, Attribute sourceAttribute)
	 *
	 * @brief Initializes this Constraint with the given source View and Attribute.
	 *
	 * @return The initialized Constraint, or `NULL` on error.
	 *
	 * @see View::constrainTo(View *, Attribute, View *, Attribute)
	 *
	 * @memberof Constraint
	 */
	Constraint *(*initWithSource)(Constraint *self, View *source, Attribute sourceAttribute);
	
	/**
	 * @fn void Constraint::limit(Constraint *self, int min, int max)
	 *
	 * @param min The minimum allowed value.
	 * @param The maximum allowed value.
	 *
	 * @memberof Constraint
	 */
	void (*limit)(Constraint *self, int min, int max);
	
	/**
	 * @fn void Constraint::unbind(Constraint *self)
	 *
	 * @brief Unbinds this Constraint from the target View.
	 *
	 * @memberof Constraint
	 */
	void (*unbind)(Constraint *self);
};

/**
 * @brief The Constraint Class.
 */
extern Class _Constraint;

#endif
