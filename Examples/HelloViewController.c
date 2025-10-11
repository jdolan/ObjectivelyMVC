/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#pragma mark - Delegates

/**
 * @brief ButtonDelegate.
 */
static void didClickButton(Button *button) {
  printf("%s: %s\n", __func__, ((View *) button)->identifier);
}

/**
 * @brief CheckboxDelegate.
 */
static void didToggleCheckbox(Checkbox *checkbox) {
  printf("%s: %s\n", __func__, $((Control *) checkbox, isSelected) ? "checked": "unchecked");
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
static void didSetValue(Slider *slider, double value) {
  printf("%s: %.1f\n", __func__, value);
}

#pragma mark - TableViewDataSource

static int tableData[25][3] = {
  { 1, 1, 1 },
  { 2, 2, 2 },
  { 3, 3, 3 },
  { 4, 4, 4 },
  { 5, 5, 5 },
  { 6, 6, 6 },
  { 7, 7, 7 },
  { 8, 8, 8 },
  { 9, 9, 9 },
  { 10, 10, 10 },
  { 11, 11, 11 },
  { 12, 12, 12 },
  { 13, 13, 13 },
  { 14, 14, 14 },
  { 15, 15, 15 },
  { 16, 16, 16 },
  { 17, 17, 17 },
  { 18, 18, 18 },
  { 19, 19, 19 },
  { 20, 20, 20 },
  { 21, 21, 21 },
  { 22, 22, 22 },
  { 23, 23, 23 },
  { 24, 24, 24 },
  { 25, 25, 25 }
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

  char text[8];
  snprintf(text, sizeof(text), "%d%c", tableData[row][col], 'A' + (int) col);

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
  return 48;
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

#pragma mark - RGBColorPickerDelegate

/**
 * @see RGBColorPickerDelegate::didPickColor
 */
static void didPickRGBColor(RGBColorPicker *colorPicker, SDL_Color *color) {
  printf("%02x%02x%02x%02x\n", color->r, color->g, color->b, color->a);
}

#pragma mark - HSVColorPickerDelegate

static void didPickHSVColor(HSVColorPicker *colorPicker, double hue, double saturation, double value) {
  const SDL_Color color = $(colorPicker, rgbColor);
  printf("%02x%02x%02x\n", color.r, color.g, color.b);
}

#pragma mark - HueColorPickerDelegate

static void didPickHueColor(HueColorPicker *colorPicker, double hue, double saturation, double value) {
  const SDL_Color color = $(colorPicker, rgbColor);
  printf("%02x%02x%02x\n", color.r, color.g, color.b);
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
    MakeOutlet("collectionView", &this->collectionView),
    MakeOutlet("rgbColorPicker", &this->rgbColorPicker),
    MakeOutlet("hsvColorPicker", &this->hsvColorPicker),
    MakeOutlet("hueColorPicker", &this->hueColorPicker)
  );

  this->panel = (Panel *) $$(View, viewWithResourceName, "HelloViewController.json", outlets);

  $(self->view, addSubview, (View *) this->panel);

  this->cancel->delegate.didClick = didClickButton;
  this->apply->delegate.didClick = didClickButton;
  this->button->delegate.didClick = didClickButton;
  this->checkbox->delegate.didToggle = didToggleCheckbox;
  this->textView->delegate.didEndEditing = didEndEditing;

  $(this->select, addOption, "This is a select", (ident) 1);
  $(this->select, addOption, "This is an option", (ident) 2);
  $(this->select, addOption, "This is another", (ident) 3);
  $(this->select, addOption, "This is yet another", (ident) 4);
  this->select->delegate.didSelectOption = didSelectOption;

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

  this->rgbColorPicker->delegate.didPickColor = didPickRGBColor;
  this->hsvColorPicker->delegate.didPickColor = didPickHSVColor;
  this->hueColorPicker->delegate.didPickColor = didPickHueColor;
}

#pragma mark - HelloViewController

/**
 * @fn HelloViewController *HelloViewController::init(HelloViewController *self)
 * @memberof HelloViewController
 */
static HelloViewController *init(HelloViewController *self) {
  return (HelloViewController *) super(ViewController, self, init);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ViewControllerInterface *) clazz->interface)->loadView = loadView;

  ((HelloViewControllerInterface *) clazz->interface)->init = init;
}

/**
 * @fn Class *HelloViewController::_HelloViewController(void)
 * @memberof HelloViewController
 */
Class *_HelloViewController(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "HelloViewController",
      .superclass = _ViewController(),
      .instanceSize = sizeof(HelloViewController),
      .interfaceOffset = offsetof(HelloViewController, interface),
      .interfaceSize = sizeof(HelloViewControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
