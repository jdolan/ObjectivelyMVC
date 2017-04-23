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

#include <Objectively/Object.h>

#include <ObjectivelyMVC/Label.h>
#include <ObjectivelyMVC/ViewController.h>

/**
 * @file
 * @brief ..
 */

/**
 * @brief Tab states, which are bit-masked.
 */
typedef enum {
	TabStateDefault = 0x0,
	TabStateSelected = 0x1
} TabState;

typedef struct TabViewItem TabViewItem;
typedef struct TabViewItemInterface TabViewItemInterface;

/**
 * @brief The TabViewItem type.
 * @extends Object
 */
struct TabViewItem {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TabViewItemInterface *interface;

	/**
	 * @brief The identifier.
	 */
	char *identifier;

	/**
	 * @brief The clickable Label to select this TabViewItem.
	 */
	Label *label;

	/**
	 * @brief The bit mask of TabState.
	 */
	int state;

	/**
	 * @brief The View this TabViewItem embeds.
	 */
	View *view;
};

/**
 * @brief The TabViewItem interface.
 */
struct TabViewItemInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn TabViewItem *TabViewItem::initWithIdentifier(TabViewItem *self, const char *identifier)
	 * @brief Initializes this TabViewItem with the specified identifier.
	 * @param self The TabViewItem.
	 * @param identifier The identifier.
	 * @return The initialized TabViewItem, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof TabViewItem
	 */
	TabViewItem *(*initWithIdentifier)(TabViewItem *self, const char *identifier);

	/**
	 * @fn TabViewItem *TabViewItem::initWithView(TabViewItem *self, View *view)
	 * @brief Initializes this TabViewItem with the specified View.
	 * @param self The TabViewItem.
	 * @param view The View to embed.
	 * @return The initialized TabViewItem, or `NULL` on error.
	 * @memberof TabViewItem
	 */
	TabViewItem *(*initWithView)(TabViewItem *self, View *view);
};

/**
 * @fn Class *TabViewItem::_TabViewItem(void)
 * @brief The TabViewItem archetype.
 * @return The TabViewItem Class.
 * @memberof TabViewItem
 */
OBJECTIVELY_EXPORT Class *_TabViewItem(void);
