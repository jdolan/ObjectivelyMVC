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
	printf("%s:\n", __func__);
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

/**
 * @see TableViewDataSource::numberOfRows
 */
static size_t numberOfRows(const TableView *tableView) {
	return 3;
}

/**
 * @see TableViewDataSource::valueForColumnAndRow
 */
static ident valueForColumnAndRow(const TableView *tableView, const TableColumn *column, int row) {

	const Array *columns = (Array *) tableView->columns;
	const int col = $(columns, indexOfObject, (ident) column);

	return (ident) (intptr_t) (columns->count * row + col);
}

#pragma mark - TableViewDelegate

/**
 * @see TableViewDelegate::cellForColumnAndRow
 */
static TableCellView *cellForColumnAndRow(const TableView *tableView, const TableColumn *column, int row) {

	TableCellView *cell = $(alloc(TableCellView), initWithFrame, NULL);
	const intptr_t value = (intptr_t) valueForColumnAndRow(tableView, column, row);

	char text[8];
	snprintf(text, sizeof(text), "%zd", value);

	$(cell->text, setText, text);
	return cell;
}

/**
 * @brief Comparator for table sorting.
 */
static Order comparator(const ident a, const ident b) {

	const intptr_t p1 = (intptr_t) a;
	const intptr_t p2 = (intptr_t) b;

	return p1 - p2;
}

/**
 * @see TableViewDelegate::didSelectRowsAtIndexes
 */
static void didSelectRowsAtIndexes(TableView *tableView, const IndexSet *indexes) {

	String *string = $((Object *) indexes, description);
	printf("%s %s\n", __func__, string->chars);
	release(string);
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
	snprintf(text, sizeof(text), "%d", $(indexPath, indexAtPosition, 0));

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

	$(this->tableView, columnWithIdentifier, "One")->comparator = comparator;
	$(this->tableView, columnWithIdentifier, "Two")->comparator = comparator;
	$(this->tableView, columnWithIdentifier, "Three")->comparator = comparator;

	$(this->tableView, reloadData);
	$((View *) this->tableView, sizeToFit);

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

Class _HelloViewController = {
	.name = "HelloViewController",
	.superclass = &_ViewController,
	.instanceSize = sizeof(HelloViewController),
	.interfaceOffset = offsetof(HelloViewController, interface),
	.interfaceSize = sizeof(HelloViewControllerInterface),
	.initialize = initialize,
};

#undef _Class
