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

#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/TableCellView.h>

/**
 * @file
 *
 * @brief Rows for TableViews.
 */

typedef struct TableRowView TableRowView;
typedef struct TableRowViewInterface TableRowViewInterface;

/**
 * @brief The TableRowView type.
 *
 * @extends StackView
 *
 * @ingroup Tables
 */
struct TableRowView {
	
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
	TableRowViewInterface *interface;

	/**
	 * @brief The cells.
	 */
	MutableArray *cells;
};

/**
 * @brief The TableRowView interface.
 */
struct TableRowViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn void TableRowView::addCell(TableRowView *self, TableCellView *cell)
	 *
	 * @brief Adds the specified cell to this row.
	 *
	 * @param cell The cell.
	 *
	 * @memberof TableRowView
	 */
	void (*addCell)(TableRowView *self, TableCellView *cell);
	
	/**
	 * @fn TableRowView *TableRowView::initWithFrame(TableRowView *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this TableRowView with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized TableRowView, or `NULL` on error.
	 *
	 * @memberof TableRowView
	 */
	TableRowView *(*initWithFrame)(TableRowView *self, const SDL_Rect *frame);

	/**
	 * @fn void TableRowView::removeAllCells(TableRowView *self)
	 *
	 * @brief Removes all cells from this row.
	 *
	 * @memberof TableRowView
	 */
	void (*removeAllCells)(TableRowView *self);

	/**
	 * @fn void TableRowView::removeCell(TableRowView *self, TableCellView *cell)
	 *
	 * @brief Removes the specified cell from this row.
	 *
	 * @param cell The cell.
	 *
	 * @memberof TableRowView
	 */
	void (*removeCell)(TableRowView *self, TableCellView *cell);
};

/**
 * @brief The TableRowView Class.
 */
extern Class _TableRowView;
