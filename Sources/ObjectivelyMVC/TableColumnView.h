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

#ifndef _ObjectivelyMVC_TableColumnView_h_
#define _ObjectivelyMVC_TableColumnView_h_

#include <ObjectivelyMVC/Control.h>

/**
 * @file
 *
 * @brief Columns for TableViews.
 */

#define DEFAULT_TABLE_VIEW_COLUMN_WIDTH 120

typedef struct TableColumnView TableColumnView;
typedef struct TableColumnViewInterface TableColumnViewInterface;

/**
 * @brief The TableColumnView type.
 *
 * @extends Control
 *
 * @ingroup Controls
 */
struct TableColumnView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	TableColumnViewInterface *interface;
	
	/**
	 * @brief The identifier.
	 */
	const char *identifier;
	
	/**
	 * @brief The maximum width.
	 */
	int maxWidth;
	
	/**
	 * @brief The minimum width.
	 */
	int minWidth;
	
	/**
	 * @brief The title.
	 */
	char *title;
	
	/**
	 * @brief The width.
	 */
	int width;
};

/**
 * @brief The TableColumnView interface.
 */
struct TableColumnViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;
	
	/**
	 * @fn TableColumnView *TableColumnView::initWithIdentifier(TableColumnView *self, const char *identifier)
	 *
	 * @brief Initializes this TableColumnView with the given identifier.
	 *
	 * @return The initialized TableColumnView, or `NULL` on error.
	 *
	 * @memberof TableColumnView
	 */
	TableColumnView *(*initWithIdentifier)(TableColumnView *self, const char *identifier);
};

/**
 * @brief The TableColumnView Class.
 */
extern Class _TableColumnView;

#endif /* _ObjectivelyMVC_TableColumnView_h_ */

