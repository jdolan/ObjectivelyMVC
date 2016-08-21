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

#include <ObjectivelyMVC/View.h>

/**
 * @file
 * 
 * @brief StackViews are containers that manage the arrangement of their subviews.
 */

/**
 * @defgroup Containers Containers
 *
 * @brief Containers allow for grouping and positioning of related Views.
 *
 * @remarks All containers have a default `autoResizingMask` of ViewAutoResizingContain.
 */

/**
 * @brief Alignment constants for positioning of subviews perpendicular to a StackView's axis.
 */
typedef enum {
	StackViewAlignmentFill,
	StackViewAlignmentLeft,
	StackViewAlignmentTop,
	StackViewAlignmentCenter,
	StackViewAlignmentRight,
	StackViewAlignmentBottom,
} StackViewAlignment;

/**
 * @brief Axis constants define a StackView's primary layout direction.
 */
typedef enum {
	StackViewAxisVertical,
	StackViewAxisHorizontal
} StackViewAxis;

/**
 * @brief Distribution models for filling a StackView's frame.
 */
typedef enum {
	StackViewDistributionDefault,
	StackViewDistributionFill,
	StackViewDistributionFillEqually,
} StackViewDistribution;

typedef struct StackView StackView;
typedef struct StackViewInterface StackViewInterface;

/**
 * @brief The StackView type.
 *
 * @extends View
 *
 * @ingroup Containers
 */
struct StackView {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	View view;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	StackViewInterface *interface;

	/**
	 * @brief The alignment.
	 */
	StackViewAlignment alignment;
	
	/**
	 * @brief The axis.
	 */
	StackViewAxis axis;

	/**
	 * @brief The distribution.
	 */
	StackViewDistribution distribution;
	
	/**
	 * @brief The subview spacing.
	 */
	int spacing;
};

/**
 * @brief The StackView interface.
 */
struct StackViewInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn StackView *StackView::initWithFrame(StackView *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this StackView with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized StackView, or `NULL` on error.
	 *
	 * @memberof StackView
	 */
	StackView *(*initWithFrame)(StackView *self, const SDL_Rect *frame);
};

/**
 * @brief The StackView Class.
 */
extern Class _StackView;
