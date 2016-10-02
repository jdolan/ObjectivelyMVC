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

#include <Objectively/IndexSet.h>
#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/ScrollView.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/TableCellView.h>
#include <ObjectivelyMVC/TableColumn.h>
#include <ObjectivelyMVC/TableHeaderView.h>
#include <ObjectivelyMVC/TableRowView.h>

/**
 * @file
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
	 * @brief Called by the TableView for the associated value of a cell.
	 * @param tableView The table.
	 * @param colum The column.
	 * @param row The row.
	 * @return The value for the cell at the given column and row.
	 */
	ident (*valueForColumnAndRow)(const TableView *tableView, const TableColumn *column, int row);
};

/**
 * @brief The TableView delegate protocol.
 */
struct TableViewDelegate {

	/**
	 * @brief Called by the TableView to instantiate cells.
	 * @param tableView The table.
	 * @param colum The column.
	 * @param row The row.
	 * @return The cell for the given column and row.
	 */
	TableCellView *(*cellForColumnAndRow)(const TableView *tableView, const TableColumn *column, int row);

	/**
	 * @brief Called by the TableView when the row selection changes.
	 * @param tableView The table.
	 * @param selectedRowIndexes The indexes of the selected rows.
	 * @remarks This function is optional.
	 */
	void (*didSelectRowsAtIndexes)(TableView *tableView, const IndexSet *selectedRowIndexes);
};

#define DEFAULT_TABLE_VIEW_PADDING 4
#define DEFAULT_TABLE_VIEW_CELL_SPACING 2
#define DEFAULT_TABLE_VIEW_ROW_HEIGHT 20

/**
 * @brief TableViews provide sortable, tabular presentations of data.
 * @extends Control
 * @ingroup Tables
 */
struct TableView {
	
	/**
	 * @brief The parent.
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 * @protected
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
	 * @brief The column definitions.
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
	 * @brief The header.
	 */
	TableHeaderView *headerView;

	/**
	 * @brief The rows.
	 */
	MutableArray *rows;

	/**
	 * @brief The row height.
	 */
	int rowHeight;

	/**
	 * @brief The scroll view.
	 */
	ScrollView *scrollView;

	/**
	 * @brief The column to sort by.
	 */
	TableColumn *sortColumn;

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
	ControlInterface controlInterface;

	/**
	 * @fn void TableView::addColumn(TableView *self, TableColumn *column)
	 * @brief Adds the specified column to this table.
	 * @param self The TableView.
	 * @param column The column.
	 * @memberof TableView
	 */
	void (*addColumn)(TableView *self, TableColumn *column);

	/**
	 * @fn TableColumn *TableView::columnAtPoint(const TableView *self, const SDL_Point *point)
	 * @param self The TableView.
	 * @param point A point in window coordinate space.
	 * @return The column at the specified point.
	 * @memberof TableView
	 */
	TableColumn *(*columnAtPoint)(const TableView *self, const SDL_Point *point);

	/**
	 * @fn TableColumn *TableView::columnWithIdentifier(const TableView *self, const char *identifier)
	 * @param self The TableView.
	 * @param identifier The column identifier.
	 * @return The TableColumn with the specified identifier, or `NULL` if not found.
	 * @memberof TableView
	 */
	TableColumn *(*columnWithIdentifier)(const TableView *self, const char *identifier);

	/**
	 * @fn void TableView::deselectAll(TableView *self)
	 * @brief Deselects all rows in this TableView.
	 * @param self The TableView.
	 * @memberof TableView
	 */
	void (*deselectAll)(TableView *self);

	/**
	 * @fn void TableView::deselectRowAtIndex(TableView *self, int index)
	 * @brief Deselects the row at the given index.
	 * @param self The TableView.
	 * @param index The index of the row to deselect.
	 * @memberof TableView
	 */
	void (*deselectRowAtIndex)(TableView *self, int index);

	/**
	 * @fn void TableView::deselectItemsAtIndexSets(TableView *self, const IndexSet *indexSet)
	 * @brief Deselects the rows at the given indexes.
	 * @param self The TableView.
	 * @param indexSet The index set of the rows to deselect.
	 * @memberof TableView
	 */
	void (*deselectRowsAtIndexes)(TableView *self, const IndexSet *indexSet);

	/**
	 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this TableView with the specified frame and style.
	 * @param self The TableView.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized TableView, or `NULL` on error.
	 * @memberof TableView
	 */
	TableView *(*initWithFrame)(TableView *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn void TableView::reloadData(TableView *self)
	 * @brief Reloads this TableView's visible rows.
	 * @param self The TableView.
	 * @remarks This method must be called after changes to the data source, delegate, or column
	 * definitions. Failure to call this method after such changes leads to undefined behavior.
	 * @memberof TableView
	 */
	void (*reloadData)(TableView *self);

	/**
	 * @fn void TableView::removeColumn(TableView *self, TableColumn *column)
	 * @brief Removes the specified column from this table.
	 * @param self The TableView.
	 * @param column The column.
	 * @memberof TableView
	 */
	void (*removeColumn)(TableView *self, TableColumn *column);

	/**
	 * @fn int TableView::rowAtPoint(const TableView *self, const SDL_Point *point)
	 * @param self The TableView.
	 * @param point A point in window coordinate space.
	 * @return The row index at the specified point, or `-1` if none.
	 * @memberof TableView
	 */
	int (*rowAtPoint)(const TableView *self, const SDL_Point *point);

	/**
	 * @fn SDL_Rect TableView::scrollableArea(const TableView *self)
	 * @param self The TableView.
	 * @return The frame available to this TableView's ScrollView.
	 * @memberof TableView
	 */
	SDL_Rect (*scrollableArea)(const TableView *self);

	/**
	 * @fn void TableView::selectAll(TableView *self)
	 * @brief Selects all rows in this TableView.
	 * @param self The TableView.
	 * @memberof TableView
	 */
	void (*selectAll)(TableView *self);

	/**
	 * @fn Array *TableView::selectedRowIndexes(const TableView *self)
	 * @param self The TableView.
	 * @return An IndexSet containing the indices of all selected rows.
	 * @memberof TableView
	 */
	IndexSet *(*selectedRowIndexes)(const TableView *self);

	/**
	 * @fn void TableView::selectRowAtIndex(TableView *self, int index)
	 * @brief Selects the row at the given index.
	 * @param self The TableView.
	 * @param index The index of the row to select.
	 * @memberof TableView
	 */
	void (*selectRowAtIndex)(TableView *self, int index);

	/**
	 * @fn void TableView::selectRowsAtIndexes(TableView *self, const IndexSet *indexes)
	 * @brief Selects the rows at the given indexes.
	 * @param indexes The indexes of the rows to selec
	 * @param self The TableView.t.
	 * @memberof TableView
	 */
	void (*selectRowsAtIndexes)(TableView *self, const IndexSet *indexes);

	/**
	 * @fn void TableView::setSortColumn(TableView *self, TableColumn *column)
	 * @brief Sets the sort column for this table.
	 * @param self The TableView.
	 * @param column The sort column.
	 * @memberof TableView
	 */
	void (*setSortColumn)(TableView *self, TableColumn *column);
};

/**
 * @brief The TableView Class.
 */
extern Class _TableView;
