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
 * @memberof TableRowView
 */
static void addCell(TableRowView *self, TableCellView *cell) {

	assert(cell);

	$(self->cells, addObject, cell);

	$((View *) self, addSubview, (View *) cell);
}

/**
 * @fn TableRowView *TableRowView::initWithTableView(TableRowView *self, TableView *tableView)
 * @memberof TableRowView
 */
static TableRowView *initWithTableView(TableRowView *self, TableView *tableView) {

	self = (TableRowView *) super(StackView, self, initWithFrame, NULL);
	if (self) {

		const Array *columns = (Array *) tableView->columns;
		
		self->cells = $$(MutableArray, arrayWithCapacity, columns->count);
		assert(self->cells);

		self->tableView = tableView;
		assert(self->tableView);

		self->stackView.axis = StackViewAxisHorizontal;
		self->stackView.distribution = StackViewDistributionFill;

		self->stackView.view.autoresizingMask |= ViewAutoresizingWidth;
	}

	return self;
}

/**
 * @brief ArrayEnumerator to remove TableCellViews from the row.
 */
static void removeAllCells_enumerate(const Array *array, ident obj, ident data) {
	$((View *) obj, removeFromSuperview);
}

/**
 * @fn void TableRowView::removeAllCells(TableRowView *self)
 * @brief Removes all cells from this row.
 * @memberof TableRowView
 */
static void removeAllCells(TableRowView *self) {

	$((Array *) self->cells, enumerateObjects, removeAllCells_enumerate, NULL);

	$(self->cells, removeAllObjects);
}

/**
 * @fn void TableRowView::removeCell(TableRowView *self, TableCellView *cell)
 * @memberof TableRowView
 */
static void removeCell(TableRowView *self, TableCellView *cell) {

	assert(cell);

	$(self->cells, removeObject, cell);

	$((View *) self, removeSubview, (View *) cell);
}

/**
 * @fn void TableRowView::setSelected(TableRowView *self, _Bool isSelected)
 * @memberof TableRowView
 */
static void setSelected(TableRowView *self, _Bool isSelected) {

	self->isSelected = isSelected;
	if (self->isSelected) {
		self->stackView.view.backgroundColor = Colors.DimGray;
	} else {
		self->stackView.view.backgroundColor = self->assignedBackgroundColor;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;

	((TableRowViewInterface *) clazz->def->interface)->addCell = addCell;
	((TableRowViewInterface *) clazz->def->interface)->initWithTableView = initWithTableView;
	((TableRowViewInterface *) clazz->def->interface)->removeAllCells = removeAllCells;
	((TableRowViewInterface *) clazz->def->interface)->removeCell = removeCell;
	((TableRowViewInterface *) clazz->def->interface)->setSelected = setSelected;
}

/**
 * @fn Class *TableRowView::_TableRowView(void)
 * @memberof TableRowView
 */
Class *_TableRowView(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "TableRowView";
		clazz.superclass = _StackView();
		clazz.instanceSize = sizeof(TableRowView);
		clazz.interfaceOffset = offsetof(TableRowView, interface);
		clazz.interfaceSize = sizeof(TableRowViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

