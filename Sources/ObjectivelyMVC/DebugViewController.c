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

#include <ObjectivelyMVC/DebugViewController.h>

#include <debug.json.h>

#define _Class _DebugViewController

#pragma mark - Selectors

/**
 * @see TableViewDataSource::numberOfRows(const TableView *)
 */
static size_t selectors_numberOfRows(const TableView *tableView) {

	const DebugViewController *this = tableView->dataSource.self;
	if (this->debug) {
		return this->debug->computedStyle->selectors->count;
	} else {
		return 0;
	}
}

/**
 * @see TableViewDataSource::valueForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static ident selectors_valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	const DebugViewController *this = tableView->dataSource.self;
	if (this->debug) {
		return $(this->debug->computedStyle->selectors, objectAtIndex, row);
	} else {
		return NULL;
	}
}

/**
 * @see TableViewDelegate::cellForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static TableCellView *selectors_cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	const Selector *selector = tableView->dataSource.valueForColumnAndRow(tableView, column, row);
	assert(selector);

	TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
	$(cell->text, setText, selector->rule);

	return cell;
}

#pragma mark - Computed Style

/**
 * @see TableViewDataSource::numberOfRows(const TableView *)
 */
static size_t computedStyle_numberOfRows(const TableView *tableView) {

	const DebugViewController *this = tableView->dataSource.self;
	if (this->debug) {
		return this->debug->computedStyle->attributes->count;
	} else {
		return 0;
	}
}

/**
 * @see TableViewDataSource::valueForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static ident computedStyle_valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	ident value = NULL;

	const DebugViewController *this = tableView->dataSource.self;
	if (this->debug) {

		Array *array;
		if (strcmp("Attribute", column->identifier) == 0) {
			array = $(this->debug->computedStyle->attributes, allKeys);
		} else {
			array = $(this->debug->computedStyle->attributes, allObjects);
		}

		value = $(array, objectAtIndex, row);
		release(array);
	}

	return value;
}

/**
 * @see TableViewDelegate::cellForColumnAndRow(const TableView *, const TableColumn *, size_t)
 */
static TableCellView *computedStyle_cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	ident value = tableView->dataSource.valueForColumnAndRow(tableView, column, row);
	assert(value);

	TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
	String *description = $((Object *) value, description);
	$(cell->text, setText, description->chars);
	release(description);

	return cell;
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);

	DebugViewController *this = (DebugViewController *) self;

	Outlet outlets[] = MakeOutlets(
		MakeOutlet("description", &this->description),
		MakeOutlet("selectors", &this->selectors),
		MakeOutlet("computedStyle", &this->computedStyle)
	);

	Data *data = $$(Data, dataWithBytes, debug_json, debug_json_len);

	this->stackView = (StackView *) $$(View, viewWithData, data, outlets);

	release(data);

	$(self->view, addSubview, (View *) this->stackView);

	this->selectors->dataSource.self = self;
	this->selectors->dataSource.numberOfRows = selectors_numberOfRows;
	this->selectors->dataSource.valueForColumnAndRow = selectors_valueForColumnAndRow;
	this->selectors->delegate.self = self;
	this->selectors->delegate.cellForColumnAndRow = selectors_cellForColumnAndRow;

	this->computedStyle->dataSource.self = self;
	this->computedStyle->dataSource.numberOfRows = computedStyle_numberOfRows;
	this->computedStyle->dataSource.valueForColumnAndRow = computedStyle_valueForColumnAndRow;
	this->computedStyle->delegate.self = self;
	this->computedStyle->delegate.cellForColumnAndRow = computedStyle_cellForColumnAndRow;
}

#pragma mark - DebugViewController


/**
 * @fn void DebugViewController::debug(DebugViewController *self, const View *view)
 * @memberof DebugViewController
 */
static void debug(DebugViewController *self, const View *view) {

	if (view != self->debug) {
		self->debug = view;

		String *description = $((Object *) view, description);
		$(self->description, setText, description->chars);
		release(description);

		$(self->selectors, reloadData);
		$((View *) self->selectors, sizeToFit);

		$(self->computedStyle, reloadData);
		$((View *) self->computedStyle, sizeToFit);

		self->stackView->view.needsLayout = true;
	}
}

/**
 * @fn DebugViewController *DebugViewController::init(DebugViewController *self)
 * @memberof DebugViewController
 */
static DebugViewController *init(DebugViewController *self) {
	return (DebugViewController *) super(ViewController, self, init);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewControllerInterface *) clazz->def->interface)->loadView = loadView;

	((DebugViewControllerInterface *) clazz->def->interface)->debug = debug;
	((DebugViewControllerInterface *) clazz->def->interface)->init = init;
}

/**
 * @fn Class *DebugViewController::_DebugViewController(void)
 * @memberof DebugViewController
 */
Class *_DebugViewController(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "DebugViewController";
		clazz.superclass = _ViewController();
		clazz.instanceSize = sizeof(DebugViewController);
		clazz.interfaceOffset = offsetof(DebugViewController, interface);
		clazz.interfaceSize = sizeof(DebugViewControllerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class