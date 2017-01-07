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

#include <ObjectivelyMVC/TableRowView.h>

/**
 * @file
 * @brief Header rows for TableViews.
 */

#define DEFAULT_TABLE_HEADER_VIEW_HEIGHT 28

typedef struct TableHeaderView TableHeaderView;
typedef struct TableHeaderViewInterface TableHeaderViewInterface;

/**
 * @brief Header rows for TableViews.
 * @extends TableRowView
 * @ingroup Tables
 */
struct TableHeaderView {

	/**
	 * @brief The superclass.
	 */
	TableRowView tableRowView;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TableHeaderViewInterface *interface;
};

/**
 * @brief The TableHeaderView interface.
 */
struct TableHeaderViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	TableRowViewInterface tableRowViewInterface;

	/**
	 * @fn TableHeaderView *TableHeaderView::initWithTableView(TableHeaderView *self, TableView *tableView)
	 * @brief Initializes this TableHeaderView with the give table.
	 * @param self The TableHeaderView.
	 * @param tableView The table.
	 * @return The initialized TableHeaderView, or `NULL` on error.
	 * @memberof TableHeaderView
	 */
	TableHeaderView *(*initWithTableView)(TableHeaderView *self, TableView *tableView);
};

/**
 * @fn Class *TableHeaderView::_TableHeaderView(void)
 * @brief The TableHeaderView archetype.
 * @return The TableHeaderView Class.
 * @memberof TableHeaderView
 */
OBJECTIVELYMVC_EXPORT Class *_TableHeaderView(void);
