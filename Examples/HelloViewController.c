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

#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

#define _Class _HelloViewController

#pragma mark - Actions

/**
 * @brief ActionFunction for Button.
 */
static void buttonAction(Control *control, const SDL_Event *event, ident sender, ident data) {
	printf("%s: %s\n", __func__, ((View *) control)->identifier);
}

/**
 * @brief ActionFunction for Checkbox.
 */
static void checkboxAction(Control *control, const SDL_Event *event, ident sender, ident data) {
	printf("%s: %s\n", __func__, $(control, selected) ? "checked": "unchecked");
}

#pragma mark - TextViewDelegate

/**
 * @see TextViewDelegate::didEndEditing
 */
static void didEndEditing(TextView *textView) {
	printf("%s: %s\n", __func__, textView->attributedText->string.chars);
}

#pragma mark - SelectDelegate

/**
 * @see SelectDelegate::didSelectOption
 */
static void didSelectOption(Select *select, Option *option) {
	printf("%s: %s\n", __func__, option->title->text);
}

#pragma mark - SliderDelegate

/**
 * @see SliderDelegate::didSetValue
 */
static void didSetValue(Slider *slider) {
	printf("%s: %.1f\n", __func__, slider->value);
}

#pragma mark - TableViewDataSource

static int tableData[7][3] = {
	{ 1, 1, 1 },
	{ 2, 2, 2 },
	{ 3, 3, 3 },
	{ 4, 4, 4 },
	{ 5, 5, 5 },
	{ 6, 6, 6 },
	{ 7, 7, 7 }
};

/**
 * @see TableViewDataSource::numberOfRows
 */
static size_t numberOfRows(const TableView *tableView) {
	return lengthof(tableData);
}

/**
 * @see TableViewDataSource::valueForColumnAndRow
 */
static ident valueForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	const ssize_t col = $((Array *) tableView->columns, indexOfObject, (ident) column);

	return (ident) (intptr_t) tableData[row][col];
}

#pragma mark - TableViewDelegate

/**
 * @see TableViewDelegate::cellForColumnAndRow
 */
static TableCellView *cellForColumnAndRow(const TableView *tableView, const TableColumn *column, size_t row) {

	TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);

	const ssize_t col = $((Array *) tableView->columns, indexOfObject, (ident) column);

	char text[16];
	snprintf(text, sizeof(text), "Cell %d%c", tableData[row][col], 'A' + (int) col);

	$(cell->text, setText, text);
	return cell;
}

/**
 * @see TableViewDelegate::didSelectRowsAtIndexes
 */
static void didSelectRowsAtIndexes(TableView *tableView, const IndexSet *indexes) {

	String *string = $((Object *) indexes, description);
	printf("%s %s\n", __func__, string->chars);
	release(string);
}

static TableView *_tableView;

/**
 * @brief Comparator for table sorting.
 */
static int comparator(const void *a, const void *b) {

	const ssize_t col = $((Array *) _tableView->columns, indexOfObject, _tableView->sortColumn);

	switch (_tableView->sortColumn->order) {
		case OrderAscending:
			return *(int *)(a + col) - *(int *)(b + col);
		case OrderDescending:
			return *(int *)(b + col) - *(int *)(a + col);
		case OrderSame:
			return 0;
	}
}

/**
 * @see TableViewDelegate::didSetSortColumn
 */
static void didSetSortColumn(TableView *tableView) {

	_tableView = tableView;
	if (_tableView->sortColumn) {

		qsort(tableData, lengthof(tableData), sizeof(tableData[0]), comparator);

		$(tableView, reloadData);
	}
}

#pragma mark - CollectionViewDataSource

/**
 * @see CollectionViewDataSource::numberOfItems
 */
static size_t numberOfItems(const CollectionView *collectionView) {
	return 24;
}

/**
 * @see CollectionViewDataSource::objectForItemAtIndexPath
 */
static ident objectForItemAtIndexPath(const CollectionView *collectionView, const IndexPath *indexPath) {
	return (ident) (intptr_t) $(indexPath, indexAtPosition, 0);
}

#pragma mark - CollectionViewDelegate

/**
 * @see CollectionViewDelegate::itemForObjectAtIndexPath
 */
static CollectionItemView *itemForObjectAtIndexPath(const CollectionView *collectionView, const IndexPath *indexPath) {

	CollectionItemView *item = $(alloc(CollectionItemView), initWithFrame, NULL);

	char text[64];
	snprintf(text, sizeof(text), "%zd", $(indexPath, indexAtPosition, 0));

	$(item->text, setText, text);
	return item;
}

/**
 * @see CollectionViewDelegate::didModifySelection
 */
static void didModifySelection(CollectionView *collectionView, const Array *selectionIndexPaths) {

	String *string = $(selectionIndexPaths, componentsJoinedByCharacters, ", ");
	printf("%s: %s\n", __func__, string->chars);
	release(string);
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);

	HelloViewController *this = (HelloViewController *) self;

	Outlet outlets[] = MakeOutlets(
		MakeOutlet("cancel", &this->cancel),
		MakeOutlet("apply", &this->apply),
		MakeOutlet("button", &this->button),
		MakeOutlet("checkbox", &this->checkbox),
		MakeOutlet("textView", &this->textView),
		MakeOutlet("select", &this->select),
		MakeOutlet("slider", &this->slider),
		MakeOutlet("tableView", &this->tableView),
		MakeOutlet("collectionView", &this->collectionView)
	);

	#ifndef EXAMPLES
	# define EXAMPLES "."
	#endif

	this->panel = (Panel *) $$(View, viewWithContentsOfFile, EXAMPLES"/HelloViewController.json", outlets);

	$(self->view, addSubview, (View *) this->panel);

	$((Control *) this->cancel, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, self, NULL);
	$((Control *) this->apply, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, self, NULL);
	$((Control *) this->button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, self, NULL);
	$((Control *) this->checkbox, addActionForEventType, SDL_MOUSEBUTTONUP, checkboxAction, self, NULL);

	this->textView->delegate.didEndEditing = didEndEditing;

	$(this->select, addOption, "This is a select", (ident) 1);
	$(this->select, addOption, "This is an option", (ident) 2);
	$(this->select, addOption, "This is another", (ident) 3);
	this->select->delegate.didSelectOption = didSelectOption;
	$((View *) this->select, sizeToFit);

	this->slider->delegate.didSetValue = didSetValue;

	this->tableView->dataSource.numberOfRows = numberOfRows;
	this->tableView->dataSource.valueForColumnAndRow = valueForColumnAndRow;
	this->tableView->delegate.cellForColumnAndRow = cellForColumnAndRow;
	this->tableView->delegate.didSelectRowsAtIndexes = didSelectRowsAtIndexes;
	this->tableView->delegate.didSetSortColumn = didSetSortColumn;

	$(this->tableView, reloadData);
	
	this->collectionView->dataSource.numberOfItems = numberOfItems;
	this->collectionView->dataSource.objectForItemAtIndexPath = objectForItemAtIndexPath;

	this->collectionView->delegate.itemForObjectAtIndexPath = itemForObjectAtIndexPath;
	this->collectionView->delegate.didModifySelection = didModifySelection;

	$(this->collectionView, reloadData);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewControllerInterface *) clazz->def->interface)->loadView = loadView;
}

/**
 * @fn Class *HelloViewController::_HelloViewController(void)
 * @memberof HelloViewController
 */
Class *_HelloViewController(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "HelloViewController";
		clazz.superclass = _ViewController();
		clazz.instanceSize = sizeof(HelloViewController);
		clazz.interfaceOffset = offsetof(HelloViewController, interface);
		clazz.interfaceSize = sizeof(HelloViewControllerInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
