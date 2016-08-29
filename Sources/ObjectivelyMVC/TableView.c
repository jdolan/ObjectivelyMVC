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
	release(this->contentView);
	release(this->headerView);
	release(this->rows);
	release(this->scrollView);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	TableView *this = (TableView *) self;

	const Array *rows = (Array *) this->rows;
	for (size_t i = 0; i < rows->count; i++) {

		TableRowView *row = (TableRowView *) $(rows, objectAtIndex, i);
		row->stackView.view.frame.h = this->rowHeight;

		if (this->selectedRow == i) {
			row->stackView.view.backgroundColor = Colors.SelectedColor;
		} else {
			if (this->usesAlternateBackgroundColor && (i & 1)) {
				row->stackView.view.backgroundColor = this->alternateBackgroundColor;
			} else {
				row->stackView.view.backgroundColor = Colors.Clear;
			}
		}
	}

	super(View, self, layoutSubviews);

	$((View *) this->contentView, sizeToFit);

	View *headerView = (View *) this->headerView;
	View *scrollView = (View *) this->scrollView;

	if (headerView->hidden == false) {
		scrollView->frame.y = headerView->frame.h;
		scrollView->frame.h = max(0, scrollView->frame.h - headerView->frame.h);
	} else {
		scrollView->frame.y = 0;
	}
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	const TableView *this = (TableView *) self;

	const SDL_Size headerSize = $((View *) this->headerView, size);
	const SDL_Size contentSize = $((View *) this->contentView, size);

	return MakeSize(max(headerSize.w, contentSize.w), headerSize.h + contentSize.h);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	TableView *this = (TableView *) self;

	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {

		if ($((View *) this->headerView, didReceiveEvent, event)) {

			if (event->type == SDL_MOUSEBUTTONUP) {
				const SDL_Point point = {
					.x = event->button.x,
					.y = event->button.y
				};

				TableColumn *column = $(this, columnAtPoint, &point);
				if (column) {
					$(this, setSortColumn, column);
					$(this, reloadData);
				}
			}

			return true;
		}

		if ($((View *) this->contentView, didReceiveEvent, event)) {

			if (event->type == SDL_MOUSEBUTTONUP) {
				const SDL_Point point = {
					.x = event->button.x,
					.y = event->button.y
				};

				const int row = $(this, rowAtPoint, &point);
				if (row == this->selectedRow) {
					$(this, selectRowAtIndex, -1);
				} else {
					$(this, selectRowAtIndex, row);
				}
			}

			return true;
		}
	}

	return super(Control, self, captureEvent, event);
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
}

/**
 * @fn TableColumn *TableView::columnAtPoint(const TableView *self, const SDL_Point *point)
 *
 * @memberof TableView
 */
static TableColumn *columnAtPoint(const TableView *self, const SDL_Point *point) {

	const SDL_Rect frame = $((View *) self, renderFrame);
	if (SDL_PointInRect(point, &frame)) {

		int x = frame.x + self->control.view.padding.left;

		Array *columns = (Array *) self->columns;
		for (size_t i = 0; i < columns->count; i++) {

			TableColumn *column = $(columns, objectAtIndex, i);

			if (x + column->width + self->cellSpacing >= point->x) {
				return column;
			}

			x += column->width + self->cellSpacing;
		}
	}

	return NULL;
}

/**
 * @fn TableColumn *TableView::columnWithIdentifier(const TableView *self)
 *
 * @memberof TableView
 */
static TableColumn *columnWithIdentifier(const TableView *self, const char *identifier) {

	assert(identifier);

	const Array *columns = (Array *) self->columns;
	for (size_t i = 0; i < columns->count; i++) {

		TableColumn *column = $(columns, objectAtIndex, i);
		if (strcmp(identifier, column->identifier) == 0) {
			return column;
		}
	}

	return NULL;
}

/**
 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof TableView
 */
static TableView *initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (TableView *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		self->columns = $$(MutableArray, array);
		assert(self->columns);

		self->rows = $$(MutableArray, array);
		assert(self->rows);

		self->headerView = $(alloc(TableHeaderView), initWithTableView, self);
		assert(self->headerView);

		$((View *) self, addSubview, (View *) self->headerView);

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		self->scrollView = $(alloc(ScrollView), initWithFrame, NULL, style);
		assert(self->scrollView);

		self->scrollView->control.view.autoresizingMask = ViewAutoresizingFill;

		$(self->scrollView, setContentView, (View *) self->contentView);

		$((View *) self, addSubview, (View *) self->scrollView);

		if (self->control.style == ControlStyleDefault) {

			self->alternateBackgroundColor = Colors.AlternateColor;
			self->usesAlternateBackgroundColor = true;

			self->cellSpacing = DEFAULT_TABLE_VIEW_CELL_SPACING;
			self->rowHeight = DEFAULT_TABLE_VIEW_ROW_HEIGHT;

			self->control.view.backgroundColor = Colors.DefaultColor;

			self->control.view.padding.top = 0;
			self->control.view.padding.right = 0;
			self->control.view.padding.bottom = 0;
			self->control.view.padding.left = 0;
		}
	}
	
	return self;
}

/**
 * @brief ArrayEnumerator to remove TableRowViews from the table's contentView.
 */
static _Bool reloadData_removeRows(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj); return false;
}

static __thread TableView *_sortTableView;

/**
 * @brief Comparator for sorting TableViewRows.
 *
 * @remarks This function relies on thread-local-storage.
 */
static Order reloadData_sortRows(const ident a, const ident b) {

	const TableColumn *column = _sortTableView->sortColumn;
	if (column->comparator) {
		const Array *rows = (Array *) _sortTableView->rows;

		const int row1 = $(rows, indexOfObject, a);
		const int row2 = $(rows, indexOfObject, b);

		const ident value1 = _sortTableView->dataSource.valueForColumnAndRow(_sortTableView, column, row1);
		const ident value2 = _sortTableView->dataSource.valueForColumnAndRow(_sortTableView, column, row2);

		switch (column->order) {
			case OrderAscending:
				return column->comparator(value1, value2);
			case OrderSame:
				return OrderSame;
			case OrderDescending:
				return column->comparator(value2, value1);
		}
	}

	return OrderSame;
}

/**
 * @brief ArrayEnumerator to add TableRowViews to the table's contentView.
 */
static _Bool reloadData_addRows(const Array *array, ident obj, ident data) {
	$((View *) data, addSubview, (View *) obj); return false;
}

/**
 * @fn void TableView::reloadData(TableView *self)
 *
 * @memberof TableView
 */
static void reloadData(TableView *self) {

	assert(self->dataSource.numberOfRows);
	assert(self->delegate.cellForColumnAndRow);

	$((Array *) self->rows, enumerateObjects, reloadData_removeRows, self->contentView);
	$(self->rows, removeAllObjects);

	TableRowView *headerView = (TableRowView *) self->headerView;
	$(headerView, removeAllCells);

	const Array *columns = (Array *) self->columns;
	for (size_t i = 0; i < columns->count; i++) {

		const TableColumn *column = $(columns, objectAtIndex, i);
		$(headerView, addCell, (TableCellView *) column->headerCell);
	}

	const size_t numberOfRows = self->dataSource.numberOfRows(self);
	for (size_t i = 0; i < numberOfRows; i++) {

		TableRowView *row = $(alloc(TableRowView), initWithTableView, self);
		assert(row);

		$(self->rows, addObject, row);
		release(row);

		for (size_t j = 0; j < columns->count; j++) {
			const TableColumn *column = $(columns, objectAtIndex, j);

			TableCellView *cell = self->delegate.cellForColumnAndRow(self, column, i);
			assert(cell);

			$(row, addCell, cell);
			release(cell);
		}
	}

	if (self->sortColumn) {
		_sortTableView = self;

		MutableArray *rows = (MutableArray *) $((Object *) self->rows, copy);
		$(rows, sort, reloadData_sortRows);

		release(self->rows);
		self->rows = rows;

		_sortTableView = NULL;
	}

	$((Array *) self->rows, enumerateObjects, reloadData_addRows, self->contentView);

	self->selectedRow = -1;

	$((View *) self, layoutSubviews);
}

/**
 * @fn void TableView::removeColumn(TableView *self, TableColumn *column)
 *
 * @memberof TableView
 */
static void removeColumn(TableView *self, TableColumn *column) {

	assert(column);

	if (self->sortColumn == column) {
		self->sortColumn = NULL;
	}
	
	$(self->columns, removeObject, column);
}

/**
 * @fn int TableView::rowAtPoint(const TableView *self, const SDL_Point *point)
 *
 * @memberof TableView
 */
static int rowAtPoint(const TableView *self, const SDL_Point *point) {

	if (self->rowHeight) {
		const SDL_Rect contentFrame = $((View *) self->contentView, renderFrame);
		if (SDL_PointInRect(point, &contentFrame)) {
			return (point->y - contentFrame.y) / self->rowHeight;
		}
	}

	return -1;
}

/**
 * @fn void TableView::selectRowAtIndex(TableView *self, int index)
 *
 * @memberof TableView
 */
static void selectRowAtIndex(TableView *self, int index) {

	self->selectedRow = clamp(index, -1, (int) self->rows->array.count - 1);

	if (self->delegate.selectionDidChange) {
		self->delegate.selectionDidChange(self);
	}

	((View *) self)->needsLayout = true;
}

/**
 * @fn void TableView::setSortColumn(TableView *self, TableColumn *column)
 *
 * @memberof TableView
 */
static void setSortColumn(TableView *self, TableColumn *column) {

	if (self->sortColumn) {
		if (self->sortColumn == column) {
			self->sortColumn->order = -self->sortColumn->order;
			return;
		}

		self->sortColumn->order = OrderSame;
		self->sortColumn = NULL;
	}

	if (column) {

		assert($((Array *) self->columns, containsObject, column));

		self->sortColumn = column;
		self->sortColumn->order = OrderAscending;

		printf("Sorting by %s %d\n", self->sortColumn->identifier, self->sortColumn->order);
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

	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

	((TableViewInterface *) clazz->interface)->addColumn = addColumn;
	((TableViewInterface *) clazz->interface)->columnAtPoint = columnAtPoint;
	((TableViewInterface *) clazz->interface)->columnWithIdentifier = columnWithIdentifier;
	((TableViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((TableViewInterface *) clazz->interface)->reloadData = reloadData;
	((TableViewInterface *) clazz->interface)->removeColumn = removeColumn;
	((TableViewInterface *) clazz->interface)->rowAtPoint = rowAtPoint;
	((TableViewInterface *) clazz->interface)->selectRowAtIndex = selectRowAtIndex;
	((TableViewInterface *) clazz->interface)->setSortColumn = setSortColumn;
}

Class _TableView = {
	.name = "TableView",
	.superclass = &_Control,
	.instanceSize = sizeof(TableView),
	.interfaceOffset = offsetof(TableView, interface),
	.interfaceSize = sizeof(TableViewInterface),
	.initialize = initialize,
};

#undef _Class
