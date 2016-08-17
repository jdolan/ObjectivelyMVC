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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/ScrollView.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/TableCellView.h>
#include <ObjectivelyMVC/TableColumn.h>
#include <ObjectivelyMVC/TableHeaderView.h>
#include <ObjectivelyMVC/TableRowView.h>

/**
 * @file
 *
 * @brief TableViews provide sortable, tabular presentations of data.
 */

typedef struct TableViewDataSource TableViewDataSource;
typedef struct TableViewDelegate TableViewDelegate;

typedef struct TableViewInterface TableViewInterface;

/**
 * @brief The TableView data source protocol.
 */
struct TableViewDataSource {

	/**
	 * @return The number of rows in the table.
	 */
	size_t (*numberOfRows)(const TableView *tableView);

	/**
	 * @param tableView The table.
	 * @param colum The column.
	 * @param row The row.
	 *
	 * @return The value for the cell at the given column and row.
	 */
	ident (*valueForColumnAndRow)(const TableView *tableView, const TableColumn *column, int row);
};

/**
 * @brief The TableView delegate protocol.
 */
struct TableViewDelegate {

	/**
	 * @param tableView The table.
	 * @param colum The column.
	 * @param row The row.
	 *
	 * @return The cell for the given column and row.
	 */
	TableCellView *(*cellForColumnAndRow)(const TableView *tableView, const TableColumn *column, int row);
};

#define DEFAULT_TABLE_VIEW_PADDING 4
#define DEFAULT_TABLE_VIEW_CELL_SPACING 2
#define DEFAULT_TABLE_VIEW_ROW_HEIGHT 20

/**
 * @brief The TableView type.
 *
 * @extends StackView
 *
 * @ingroup Tables
 */
struct TableView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	StackView stackView;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	TableViewInterface *interface;

	/**
	 * @brief The
	 */
	SDL_Color alternateBackgroundColor;

	/**
	 * @brief Inter-cell (horizontal) spacing.
	 */
	int cellSpacing;

	/**
	 * @brief The TableColumn definitions.
	 */
	MutableArray *columns;

	/**
	 * @brief The content View.
	 */
	StackView *contentView;
	
	/**
	 * @brief The data source.
	 */
	TableViewDataSource dataSource;

	/**
	 * @brief The delegate.
	 */
	TableViewDelegate delegate;

	/**
	 * @brief The TableHeaderView.
	 */
	TableHeaderView *headerView;

	/**
	 * @brief The TableRowViews.
	 */
	MutableArray *rows;

	/**
	 * @brief The row height.
	 */
	int rowHeight;

	/**
	 * @brief The ScrollView.
	 */
	ScrollView *scrollView;

	/**
	 * @brief The selected row, or `-1` when no row is selected.
	 */
	int selectedRow;

	/**
	 * @brief Set to `true` to enable alternate row coloring.
	 */
	_Bool usesAlternateBackgroundColor;
};

/**
 * @brief The TableView interface.
 */
struct TableViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn void TableView::addColumn(TableView *self, TableColumn *column)
	 *
	 * @brief Adds the specified column to this table.
	 *
	 * @param column The column.
	 *
	 * @memberof TableView
	 */
	void (*addColumn)(TableView *self, TableColumn *column);

	/**
	 * @fn TableColumn *TableView::columnWithIdentifier(const TableView *self)
	 *
	 * @param identifie The column identifier.
	 *
	 * @return The TableColumn with the specified identifier, or `NULL` if not found.
	 *
	 * @memberof TableView
	 */
	TableColumn *(*columnWithIdentifier)(const TableView *self, const char *identifier);

	/**
	 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this TableView with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized TableView, or `NULL` on error.
	 *
	 * @memberof TableView
	 */
	TableView *(*initWithFrame)(TableView *self, const SDL_Rect *frame);

	/**
	 * @fn void TableView::reloadData(TableView *self)
	 *
	 * @brief Reloads this TableView's visible rows.
	 *
	 * @remarks This method must be called after changes to the data source,
	 * delegate, or column definitions. Failure to call this method after such
	 * changes leads to undefined behavior.
	 *
	 * @memberof TableView
	 */
	void (*reloadData)(TableView *self);

	/**
	 * @fn void TableView::removeColumn(TableView *self, TableColumn *column)
	 *
	 * @brief Removes the specified column from this table.
	 *
	 * @param column The column.
	 *
	 * @memberof TableView
	 */
	void (*removeColumn)(TableView *self, TableColumn *column);
};

/**
 * @brief The TableView Class.
 */
extern Class _TableView;
