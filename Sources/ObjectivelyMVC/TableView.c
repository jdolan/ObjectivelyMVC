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

#include <assert.h>

#include <ObjectivelyMVC/TableView.h>

#define _Class _TableView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	TableView *this = (TableView *) self;

	release(this->columns);
	release(this->rows);
	release(this->headerView);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	TableView *this = (TableView *) self;

	const Array *columns = (Array *) this->columns;
	const Array *rows = (Array *) this->rows;

	for (size_t i = 0; i < rows->count; i++) {

		TableRowView *row = (TableRowView *) $(rows, objectAtIndex, i);

		row->stackView.spacing = this->cellSpacing;
		row->stackView.view.frame.h = this->rowHeight;

		if (this->usesAlternateBackgroundColor) {
			if (i & 1) {
				row->stackView.view.backgroundColor = this->alternateBackgroundColor;
			}
		}

		const Array *cells = (Array *) row->stackView.view.subviews;
		assert(cells->count == columns->count);

		for (size_t j = 0; j < columns->count; j++) {

			const TableColumn *column = $(columns, objectAtIndex, j);

			TableCellView *cell = $(cells, objectAtIndex, j);
			cell->view.frame.w = column->width;
		}

		$(self, addSubview, (View *) row);
	}

	super(View, self, layoutSubviews);
}

#pragma mark - TableView

/**
 * @fn void TableView::addColumn(TableView *self, TableColumn *column)
 *
 * @memberof TableView
 */
static void addColumn(TableView *self, TableColumn *column) {

	assert(column);

	$(self->columns, addObject, column);

	//$((View *) self->headerView, addSubview, (View *) column);
}

/**
 * @fn TableCellView *TableView::cellForColumnAndRow(const TableView *self, const TableColumn *column, int row)
 *
 * @param column The column.
 * @param row The row.
 *
 * @return The cell for the given column and row.
 *
 * @memberof TableView
 */
static TableCellView *cellForColumnAndRow(const TableView *self, const TableColumn *column, int row) {

	const Array *columns = (Array *) self->columns;
	const int index = $(columns, indexOfObject, (ident) column);
	if (index != -1) {

		const Array *rows = (Array *) self->rows;
		if (row < rows->count) {

			const StackView *stackView = $(rows, objectAtIndex, row);
			const Array *cells = (Array *) stackView->view.subviews;

			if (index < cells->count) {
				return (TableCellView *) $(cells, objectAtIndex, index);
			}
		}
	}

	return NULL;
}

/**
 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame)
 *
 * @memberof TableView
 */
static TableView *initWithFrame(TableView *self, const SDL_Rect *frame) {
	
	self = (TableView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->columns = $$(MutableArray, array);
		assert(self->columns);

		self->rows = $$(MutableArray, array);
		assert(self->rows);

		self->headerView = $(alloc(TableHeaderView), initWithFrame, NULL);
		assert(self->headerView);

		$((View *) self, addSubview, (View *) self->headerView);

		self->alternateBackgroundColor = Colors.AlternateTableRowColor;
		self->usesAlternateBackgroundColor = true;

		self->cellSpacing = DEFAULT_TABLE_VIEW_CELL_SPACING;
		self->rowHeight = DEFAULT_TABLE_VIEW_ROW_HEIGHT;
	}
	
	return self;
}

/**
 * @fn void TableView::reloadData(TableView *self)
 *
 * @memberof TableView
 */
static void reloadData(TableView *self) {

	$(self->rows, removeAllObjects);

	const size_t rows = self->dataSource.numberOfRows(self);
	for (size_t i = 0; i < rows; i++) {

		TableRowView *row = $(alloc(TableRowView), initWithFrame, NULL);
		assert(row);

		$(self->rows, addObject, row);

		const Array *columns = (Array *) self->columns;
		for (size_t j = 0; j < columns->count; j++) {

			const TableColumn *column = $(columns, objectAtIndex, j);
			ident value = self->dataSource.valueForColumnAndRow(self, column, i);

			TableCellView *cell = self->delegate.cellForColumnAndRow(self, column, i, value);
			assert(cell);

			$((View *) row, addSubview, (View *) cell);
		}
	}

	((View *) self)->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

	((TableViewInterface *) clazz->interface)->addColumn = addColumn;
	((TableViewInterface *) clazz->interface)->cellForColumnAndRow = cellForColumnAndRow;
	((TableViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((TableViewInterface *) clazz->interface)->reloadData = reloadData;
}

Class _TableView = {
	.name = "TableView",
	.superclass = &_StackView,
	.instanceSize = sizeof(TableView),
	.interfaceOffset = offsetof(TableView, interface),
	.interfaceSize = sizeof(TableViewInterface),
	.initialize = initialize,
};

#undef _Class
