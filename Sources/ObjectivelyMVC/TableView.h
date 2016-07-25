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

#include <ObjectivelyMVC/ScrollView.h>
#include <ObjectivelyMVC/TableColumnView.h>

/**
 * @file
 *
 * @brief TableViews provide sortable, tabular presentations of data.
 */

typedef struct TableCellView TableCellView;
typedef struct TableViewDataSource TableViewDataSource;
typedef struct TableViewDelegate TableViewDelegate;

typedef struct TableView TableView;
typedef struct TableViewInterface TableViewInterface;

/**
 * @brief The TableView data source protocol.
 */
struct TableViewDataSource {
	
	size_t (*numberOfRows)(TableView *tableView);
	
	ident (*valueForColumnAndRow)(TableView *tableView, TableColumnView *column, int row);
};

struct TableViewDelegate {

	View *(*viewForColumnAndRow)(TableView *tableView, TableColumnView *column, int row);
};

/**
 * @brief The TableView type.
 *
 * @extends ScrollView
 *
 * @ingroup
 */
struct TableView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	ScrollView scrollView;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	TableViewInterface *interface;
	
	/**
	 * @brief The column definitions.
	 */
	MutableArray *columns;
	
	/**
	 * @brief The data source.
	 */
	TableViewDataSource dataSource;

	/**
	 * @brief The delegate.
	 */
	TableViewDelegate delegate;
};

/**
 * @brief The TableView interface.
 */
struct TableViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ScrollViewInterface scrolViewInterface;
	
	/**
	 * @fn TableView *TableView::init(TableView *self)
	 *
	 * @brief Initializes this TableView.
	 *
	 * @return The initialized TableView, or `NULL` on error.
	 *
	 * @memberof TableView
	 */
	TableView *(*initWithFrame)(TableView *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn void TableView::reloadData(TableView *self)
	 *
	 * @brief Reloads this TableView's visible rows.
	 *
	 * @memberof TableView
	 */
	void (*reloadData)(TableView *self);
};

/**
 * @brief The TableView Class.
 */
extern Class _TableView;


