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
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	TableRowView *this = (TableRowView *) self;

	if (simpleSelector->type == SimpleSelectorTypePseudo) {
		if (strcmp("selected", simpleSelector->pattern) == 0) {
			return this->isSelected;
		}
	}

	return super(View, self, matchesSelector, simpleSelector);
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
	$(self->cells, removeAllObjectsWithEnumerator, removeAllCells_enumerate, NULL);
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

	if (isSelected != self->isSelected) {
		self->isSelected = isSelected;
		$((View *) self, invalidateStyle);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;

	((TableRowViewInterface *) clazz->interface)->addCell = addCell;
	((TableRowViewInterface *) clazz->interface)->initWithTableView = initWithTableView;
	((TableRowViewInterface *) clazz->interface)->removeAllCells = removeAllCells;
	((TableRowViewInterface *) clazz->interface)->removeCell = removeCell;
	((TableRowViewInterface *) clazz->interface)->setSelected = setSelected;
}

/**
 * @fn Class *TableRowView::_TableRowView(void)
 * @memberof TableRowView
 */
Class *_TableRowView(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "TableRowView",
			.superclass = _StackView(),
			.instanceSize = sizeof(TableRowView),
			.interfaceOffset = offsetof(TableRowView, interface),
			.interfaceSize = sizeof(TableRowViewInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}
#undef _Class

