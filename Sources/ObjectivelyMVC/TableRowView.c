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

#include <ObjectivelyMVC/TableRowView.h>
#include <ObjectivelyMVC/TableView.h>

#define _Class _TableRowView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TableRowView *this = (TableRowView *) self;

	release(this->cells);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	TableRowView *this = (TableRowView *) self;

	this->stackView.spacing = this->tableView->cellSpacing;

	const Array *cells = (Array *) this->cells;
	const Array *columns = (Array *) this->tableView->columns;

	assert(cells->count == columns->count);

	for (size_t i = 0; i < columns->count; i++) {

		TableCellView *cell = $(cells, objectAtIndex, i);
		const TableColumn *column = $(columns, objectAtIndex, i);

		cell->view.frame.w = column->width;
	}

	super(View, self, layoutSubviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	TableRowView *this = (TableRowView *) self;

	SDL_Size size = MakeSize(0, this->tableView->rowHeight);

	const Array *columns = (Array *) this->tableView->columns;
	if (columns->count) {
		for (size_t i = 0; i < columns->count; i++) {
			const TableColumn *column = $(columns, objectAtIndex, i);
			size.w += column->width;
		}

		size.w += (columns->count - 1) * this->tableView->cellSpacing;
	}

	return size;
}

#pragma mark - TableRowView

/**
 * @fn void TableRowView::addCell(TableRowView *self, TableCellView *cell)
 *
 * @memberof TableRowView
 */
static void addCell(TableRowView *self, TableCellView *cell) {

	assert(cell);

	$(self->cells, addObject, cell);

	$((View *) self, addSubview, (View *) cell);
}

/**
 * @fn TableRowView *TableRowView::initWithTableView(TableRowView *self, TableView *tableView)
 *
 * @memberof TableRowView
 */
static TableRowView *initWithTableView(TableRowView *self, TableView *tableView) {
	
	self = (TableRowView *) super(StackView, self, initWithFrame, NULL);
	if (self) {

		self->cells = $$(MutableArray, array);
		assert(self->cells);

		self->tableView = tableView;
		assert(self->tableView);

		self->stackView.axis = StackViewAxisHorizontal;

		self->stackView.view.autoresizingMask |= ViewAutoresizingWidth;
	}
	
	return self;
}

/**
 * @brief ArrayEnumerator to remove TableCellViews from the row.
 */
static _Bool removeAllCells_enumerate(const Array *array, ident obj, ident data) {
	$((View *) obj, removeFromSuperview); return false;
}

/**
 * @fn void TableRowView::removeAllCells(TableRowView *self)
 *
 * @brief Removes all cells from this row.
 *
 * @memberof TableRowView
 */
static void removeAllCells(TableRowView *self) {

	$((Array *) self->cells, enumerateObjects, removeAllCells_enumerate, NULL);

	$(self->cells, removeAllObjects);
}

/**
 * @fn void TableRowView::removeCell(TableRowView *self, TableCellView *cell)
 *
 * @memberof TableRowView
 */
static void removeCell(TableRowView *self, TableCellView *cell) {

	assert(cell);

	$(self->cells, removeObject, cell);

	$((View *) self, removeSubview, (View *) cell);
}

/**
 * @fn void TableRowView::setSelected(TableRowView *self, _Bool selected)
 *
 * @memberof TableRowView
 */
static void setSelected(TableRowView *self, _Bool selected) {

	self->isSelected = selected;
	if (self->isSelected) {
		self->stackView.view.backgroundColor = Colors.SelectedColor;
	} else {
		self->stackView.view.backgroundColor = self->assignedBackgroundColor;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

	((TableRowViewInterface *) clazz->interface)->addCell = addCell;
	((TableRowViewInterface *) clazz->interface)->initWithTableView = initWithTableView;
	((TableRowViewInterface *) clazz->interface)->removeAllCells = removeAllCells;
	((TableRowViewInterface *) clazz->interface)->removeCell = removeCell;
	((TableRowViewInterface *) clazz->interface)->setSelected = setSelected;
}

Class _TableRowView = {
	.name = "TableRowView",
	.superclass = &_StackView,
	.instanceSize = sizeof(TableRowView),
	.interfaceOffset = offsetof(TableRowView, interface),
	.interfaceSize = sizeof(TableRowViewInterface),
	.initialize = initialize,
};

#undef _Class

