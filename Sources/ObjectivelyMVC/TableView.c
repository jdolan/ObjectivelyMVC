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
#include <string.h>

#include <Objectively/String.h>

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
 * @brief ArrayEnumerator for awaking TableColumns.
 */
static void awakeWithDictionary_columns(const Array *array, ident obj, ident data) {

	const String *identifier = $((Dictionary *) obj, objectForKeyPath, "identifier");
	assert(identifier);

	TableColumn *column = $(alloc(TableColumn), initWithIdentifier, identifier->chars);
	assert(column);

	const Inlet inlets[] = MakeInlets(
		MakeInlet("cellAlignment", InletTypeEnum, &column->cellAlignment, (ident) ViewAlignmentNames),
		MakeInlet("width", InletTypeInteger, &column->width, NULL)
	);

	$((View *) data, bind, inlets, obj);
	$((TableView *) data, addColumn, column);

	release(column);
}

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	TableView *this = (TableView *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("alternateBackgroundColor", InletTypeColor, &this->alternateBackgroundColor, NULL),
		MakeInlet("cellSpacing", InletTypeInteger, &this->cellSpacing, NULL),
		MakeInlet("rowHeight", InletTypeInteger, &this->rowHeight, NULL),
		MakeInlet("usesAlternateBackgroundColor", InletTypeBool, &this->usesAlternateBackgroundColor, NULL)
	);

	$(self, bind, inlets, dictionary);

	const Array *columns = $(dictionary, objectForKeyPath, "columns");
	if (columns) {
		$(columns, enumerateObjects, awakeWithDictionary_columns, self);
	}
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((TableView *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	TableView *this = (TableView *) self;

	View *scrollView = (View *) this->scrollView;

	scrollView->frame = $(this, scrollableArea);
	scrollView->needsLayout = true;

	const Array *rows = (Array *) this->rows;
	for (size_t i = 0; i < rows->count; i++) {

		TableRowView *row = (TableRowView *) $(rows, objectAtIndex, i);
		row->stackView.view.frame.h = this->rowHeight;

		if (this->usesAlternateBackgroundColor && (i & 1)) {
			row->assignedBackgroundColor = this->alternateBackgroundColor;
		} else {
			row->assignedBackgroundColor = Colors.Clear;
		}

		row->stackView.view.backgroundColor = row->assignedBackgroundColor;
	}

	super(View, self, layoutSubviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	const TableView *this = (TableView *) self;

	const SDL_Size headerSize = $((View *) this->headerView, sizeThatFits);
	const SDL_Size contentSize = $((View *) this->contentView, sizeThatFits);

	SDL_Size size = MakeSize(max(headerSize.w, contentSize.w), headerSize.h + contentSize.h);

	size.w += self->padding.left + self->padding.right;
	size.h += self->padding.top + self->padding.bottom;

	return size;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	TableView *this = (TableView *) self;

	if (event->type == SDL_MOUSEBUTTONUP) {

		if ($((View *) this->headerView, didReceiveEvent, event)) {

			const SDL_Point point = {
				.x = event->button.x,
				.y = event->button.y
			};

			TableColumn *column = $(this, columnAtPoint, &point);
			if (column) {
				$(this, setSortColumn, column);
			}

			return true;
		}

		if ($((Control *) this->scrollView, highlighted) == false) {
			if ($((View *) this->contentView, didReceiveEvent, event)) {

				const SDL_Point point = {
					.x = event->button.x,
					.y = event->button.y
				};

				const ssize_t index = $(this, rowAtPoint, &point);

				const Array *rows = (Array *) this->rows;
				if (index > -1 && index < rows->count) {

					TableRowView *row = $(rows, objectAtIndex, index);

					switch (this->control.selection) {
						case ControlSelectionNone:
							break;
						case ControlSelectionSingle:
							if (row->isSelected == false) {
								$(this, deselectAll);
								$(this, selectRowAtIndex, index);
							}
							break;
						case ControlSelectionMultiple:
							if (SDL_GetModState() & (KMOD_CTRL | KMOD_GUI)) {
								if (row->isSelected) {
									$(this, deselectRowAtIndex, index);
								} else {
									$(this, selectRowAtIndex, index);
								}
							} else {
								$(this, deselectAll);
								$(this, selectRowAtIndex, index);
							}
							break;
					}

					if (this->delegate.didSelectRowsAtIndexes) {
						IndexSet *selectedRowIndexes = $(this, selectedRowIndexes);

						this->delegate.didSelectRowsAtIndexes(this, selectedRowIndexes);

						release(selectedRowIndexes);
					}
				}

				return true;
			}
		}
	}

	return super(Control, self, captureEvent, event);
}


#pragma mark - TableView

/**
 * @fn void TableView::addColumn(TableView *self, TableColumn *column)
 * @memberof TableView
 */
static void addColumn(TableView *self, TableColumn *column) {

	assert(column);

	$(self->columns, addObject, column);

	$((TableRowView *) self->headerView, addCell, (TableCellView *) column->headerCell);
}

/**
 * @fn TableColumn *TableView::columnAtPoint(const TableView *self, const SDL_Point *point)
 * @memberof TableView
 */
static TableColumn *columnAtPoint(const TableView *self, const SDL_Point *point) {

	const SDL_Rect frame = $((View *) self, renderFrame);
	if (SDL_PointInRect(point, &frame)) {

		const Array *cells = (Array *) self->headerView->tableRowView.cells;
		const Array *columns = (Array *) self->columns;

		assert(cells->count == columns->count);

		for (size_t i = 0; i < cells->count; i++) {

			const View *cell = $(cells, objectAtIndex, i);
			const SDL_Rect renderFrame = $(cell, renderFrame);

			if (renderFrame.x + renderFrame.w >= point->x) {
				return $(columns, objectAtIndex, i);
			}
		}
	}

	return NULL;
}

/**
 * @fn TableColumn *TableView::columnWithIdentifier(const TableView *self, const char *identifier)
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
 * @brief ArrayEnumerator for all Row deselection.
 */
static void deselectAll_enumerate(const Array *array, ident obj, ident data) {
	$((TableRowView *) obj, setSelected, false);
}

/**
 * @fn void TableView::deselectAll(TableView *self)
 * @memberof TableView
 */
static void deselectAll(TableView *self) {
	$((Array *) self->rows, enumerateObjects, deselectAll_enumerate, NULL);
}

/**
 * @fn void TableView::deselectRowAtIndex(TableView *self, size_t index)
 * @memberof TableView
 */
static void deselectRowAtIndex(TableView *self, size_t index) {

	const Array *rows = (Array *) self->rows;
	if (index < rows->count) {

		TableRowView *row = $(rows, objectAtIndex, index);
		$(row, setSelected, false);
	}
}

/**
 * @fn void TableView::deselectRowsAtIndexes(TableView *self, const IndexSet *indexes)
 * @memberof TableView
 */
static void deselectRowsAtIndexes(TableView *self, const IndexSet *indexes) {

	if (indexes) {
		for (size_t i = 0; i < indexes->count; i++) {
			$(self, deselectRowAtIndex, indexes->indexes[i]);
		}
	}
}

/**
 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style)
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

		self->contentView->view.autoresizingMask |= ViewAutoresizingWidth;

		self->scrollView = $(alloc(ScrollView), initWithFrame, NULL, style);
		assert(self->scrollView);

		self->scrollView->control.view.autoresizingMask |= ViewAutoresizingWidth;

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
static void reloadData_removeRows(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj);
}

/**
 * @brief ArrayEnumerator to add TableRowViews to the table's contentView.
 */
static void reloadData_addRows(const Array *array, ident obj, ident data) {
	$((View *) data, addSubview, (View *) obj);
}

/**
 * @fn void TableView::reloadData(TableView *self)
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

	$((Array *) self->rows, enumerateObjects, reloadData_addRows, self->contentView);

	self->control.view.needsLayout = true;
}

/**
 * @fn void TableView::removeColumn(TableView *self, TableColumn *column)
 * @memberof TableView
 */
static void removeColumn(TableView *self, TableColumn *column) {

	assert(column);

	if (self->sortColumn == column) {
		self->sortColumn->order = OrderSame;
		self->sortColumn = NULL;
	}

	$(self->columns, removeObject, column);

	$((TableRowView *) self->headerView, removeCell, (TableCellView *) column->headerCell);
}

/**
 * @fn ssize_t TableView::rowAtPoint(const TableView *self, const SDL_Point *point)
 * @memberof TableView
 */
static ssize_t rowAtPoint(const TableView *self, const SDL_Point *point) {

	if (self->rowHeight) {
		const SDL_Rect contentFrame = $((View *) self->contentView, renderFrame);
		if (SDL_PointInRect(point, &contentFrame)) {
			return (point->y - contentFrame.y) / self->rowHeight;
		}
	}

	return -1;
}

/**
 * @fn SDL_Rect TableView::scrollableArea(const TableView *self)
 * @memberof TableView
 */
static SDL_Rect scrollableArea(const TableView *self) {

	const View *headerView = (View *) self->headerView;

	SDL_Rect frame = $((View *) self, bounds);

	if (headerView->hidden == false) {
		frame.y = headerView->frame.h;
		frame.h -= headerView->frame.h;
	}

	return frame;
}

/**
 * @brief ArrayEnumerator for all row selection.
 */
static void selectAll_enumerate(const Array *array, ident obj, ident data) {
	$((TableRowView *) obj, setSelected, true);
}

/**
 * @fn void TableView::selectAll(TableView *self)
 * @memberof TableView
 */
static void selectAll(TableView *self) {
	$((Array *) self->rows, enumerateObjects, selectAll_enumerate, NULL);
}

/**
 * @fn IndexSet *TableView::selectedRowIndexes(const TableView *self)
 * @memberof TableView
 */
static IndexSet *selectedRowIndexes(const TableView *self) {

	size_t indexes[self->rows->array.count];
	size_t count = 0;

	const Array *rows = (Array *) self->rows;
	for (size_t i = 0; i < rows->count; i++) {

		const TableRowView *row = $(rows, objectAtIndex, i);
		if (row->isSelected) {
			indexes[count++] = i;
		}
	}

	return $(alloc(IndexSet), initWithIndexes, indexes, count);
}

/**
 * @fn void TableView::selectRowAtIndex(TableView *self, size_t index)
 * @memberof TableView
 */
static void selectRowAtIndex(TableView *self, size_t index) {

	const Array *rows = (Array *) self->rows;
	if (index < rows->count) {

		TableRowView *row = $(rows, objectAtIndex, index);
		$(row, setSelected, true);
	}
}

/**
 * @fn void TableView::selectRowsAtIndexes(TableView *self, const IndexSet *indexes)
 * @memberof TableView
 */
static void selectRowsAtIndexes(TableView *self, const IndexSet *indexes) {

	if (indexes) {
		for (size_t i = 0; i < indexes->count; i++) {
			$(self, selectRowAtIndex, indexes->indexes[i]);
		}
	}
}

/**
 * @fn void TableView::setSortColumn(TableView *self, TableColumn *column)
 * @memberof TableView
 */
static void setSortColumn(TableView *self, TableColumn *column) {

	if (self->sortColumn != column) {

		if (self->sortColumn) {
			self->sortColumn->order = OrderSame;
			self->sortColumn = NULL;
		}

		if (column) {
			assert($((Array *) self->columns, containsObject, column));

			self->sortColumn = column;
			self->sortColumn->order = OrderAscending;
		}
	} else {
		if (self->sortColumn) {
			self->sortColumn->order = -self->sortColumn->order;
		}
	}

	if (self->delegate.didSetSortColumn) {
		self->delegate.didSetSortColumn(self);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;

	((TableViewInterface *) clazz->def->interface)->addColumn = addColumn;
	((TableViewInterface *) clazz->def->interface)->columnAtPoint = columnAtPoint;
	((TableViewInterface *) clazz->def->interface)->columnWithIdentifier = columnWithIdentifier;
	((TableViewInterface *) clazz->def->interface)->deselectAll = deselectAll;
	((TableViewInterface *) clazz->def->interface)->deselectRowAtIndex = deselectRowAtIndex;
	((TableViewInterface *) clazz->def->interface)->deselectRowsAtIndexes = deselectRowsAtIndexes;
	((TableViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((TableViewInterface *) clazz->def->interface)->reloadData = reloadData;
	((TableViewInterface *) clazz->def->interface)->removeColumn = removeColumn;
	((TableViewInterface *) clazz->def->interface)->rowAtPoint = rowAtPoint;
	((TableViewInterface *) clazz->def->interface)->scrollableArea = scrollableArea;
	((TableViewInterface *) clazz->def->interface)->selectedRowIndexes = selectedRowIndexes;
	((TableViewInterface *) clazz->def->interface)->selectAll = selectAll;
	((TableViewInterface *) clazz->def->interface)->selectRowAtIndex = selectRowAtIndex;
	((TableViewInterface *) clazz->def->interface)->selectRowsAtIndexes = selectRowsAtIndexes;
	((TableViewInterface *) clazz->def->interface)->setSortColumn = setSortColumn;
}

/**
 * @fn Class *TableView::_TableView(void)
 * @memberof TableView
 */
Class *_TableView(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "TableView";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(TableView);
		clazz.interfaceOffset = offsetof(TableView, interface);
		clazz.interfaceSize = sizeof(TableViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
