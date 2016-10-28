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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/Option.h>
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 * @brief A Control allowing users to select one or more Options.
 */

#define DEFAULT_SELECT_WIDTH 100

typedef struct SelectDelegate SelectDelegate;

typedef struct Select Select;
typedef struct SelectInterface SelectInterface;

/**
 * @brief The Select delegate protocol.
 */
struct SelectDelegate {
	
	/**
	 * @brief Called when a selection is made.
	 * @param select The Select.
	 * @param option The selected Option.
	 */
	void (*didSelectOption)(Select *select, Option *option);
};

/**
 * @brief A Control allowing users to select one or more Options.
 * @extends Control
 * @ingroup Controls
 */
struct Select {
	
	/**
	 * @brief The parent.
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	SelectInterface *interface;

	/**
	 * @brief An optional Comparator to sort Options.
	 */
	Comparator comparator;
	
	/**
	 * @brief The SelectDelegate.
	 */
	SelectDelegate delegate;
	
	/**
	 * @brief The Select Options.
	 * @private
	 */
	MutableArray *options;

	/**
	 * @brief The selected Option, or `NULL`.
	 */
	Option *selectedOption;

	/**
	 * @brief The StackView for rendering all Options.
	 * @private
	 */
	StackView *stackView;
};

/**
 * @brief The Select interface.
 */
struct SelectInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;
	
	/**
	 * @fn void Select::addOption(Select *self, const char *title, ident value)
	 * @brief Creates and adds a new Option to this Select.
	 * @param self The Select.
	 * @param title The Option title.
	 * @param value The Option value.
	 * @memberof Select
	 */
	void (*addOption)(Select *self, const char *title, ident value);
	
	/**
	 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this Select with the specified frame and style.
	 * @param self The Select.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized Select, or `NULL` on error.
	 * @memberof Select
	 */
	Select *(*initWithFrame)(Select *self, const SDL_Rect *frame, ControlStyle style);
	
	/**
	 * @fn Option *Select::optionWithValue(const Select *self, ident value)
	 * @param self The Select.
	 * @param value The Option value.
	 * @return The first Option with the given value.
	 * @memberof Select
	 */
	Option *(*optionWithValue)(const Select *self, ident value);

	/**
	 * @fn void Select::removeAllOptions(Select *self)
	 * @brief Removes all Options from this Select.
	 * @param self The Select.
	 * @memberof Select
	 */
	void (*removeAllOptions)(Select *self);

	/**
	 * @fn void Select::removeOption(Select *self, Option *option)
	 * @brief Removes the specified Option.
	 * @param self The Select.
	 * @param option The Option to remove.
	 * @memberof Select
	 */
	void (*removeOption)(Select *self, Option *option);

	/**
	 * @fn void Select::removeOptionWithValue(Select *self, ident value)
	 * @brief Removes first the Option with the given value.
	 * @param self The Select.
	 * @param value The Option value.
	 * @memberof Select
	 */
	void (*removeOptionWithValue)(Select *self, ident value);
	
	/**
	 * @fn void Select::selectOptionWithValue(Select *self, ident value)
	 * @brief Selects the first Option with the given value.
	 * @param self The Select.
	 * @param value The Option value.
	 * @memberof Select
	 */
	void (*selectOptionWithValue)(Select *self, ident value);
	
	/**
	 * @fn Option *Select::selectedOption(const Select *self)
	 * @param self The Select.
	 * @return The selected Option, or `NULL`.
	 * @memberof Select
	 */
	Option *(*selectedOption)(const Select *self);
};

/**
 * @brief The Select Class.
 */
extern Class _Select;
