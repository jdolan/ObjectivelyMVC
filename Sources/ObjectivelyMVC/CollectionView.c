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

#include <ObjectivelyMVC/CollectionView.h>

const EnumName CollectionViewAxisNames[] = MakeEnumNames(
	MakeEnumName(CollectionViewAxisHorizontal),
	MakeEnumName(CollectionViewAxisVertical)
);

#define _Class _CollectionView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	CollectionView *this = (CollectionView *) self;

	release(this->contentView);
	release(this->items);
	release(this->scrollView);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	CollectionView *this = (CollectionView *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("axis", InletTypeEnum, &this->axis, (ident) CollectionViewAxisNames),
		MakeInlet("itemSize", InletTypeSize, &this->itemSize, NULL),
		MakeInlet("itemSpacing", InletTypeSize, &this->itemSpacing, NULL)
	);

	$(self, bind, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((CollectionView *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	CollectionView *this = (CollectionView *) self;

	const SDL_Rect bounds = $(self, bounds);

	int x = bounds.x, y = bounds.y;

	const Array *items = (Array *) this->items;
	for (size_t i = 0; i < items->count; i++) {

		CollectionItemView *item = (CollectionItemView *) $(items, objectAtIndex, i);

		item->view.frame.x = x;
		item->view.frame.y = y;
		item->view.frame.w = this->itemSize.w;
		item->view.frame.h = this->itemSize.h;

		switch (this->axis) {
			case CollectionViewAxisVertical:
				x += this->itemSize.w + this->itemSpacing.w;
				if (x + this->itemSize.w > bounds.w) {
					y += this->itemSize.h + this->itemSpacing.h;
					x = bounds.x;
				}
				break;
			case CollectionViewAxisHorizontal:
				y += this->itemSize.h + this->itemSpacing.h;
				if (y + this->itemSize.h > bounds.h) {
					x += this->itemSize.w + this->itemSpacing.w;
					y = bounds.y;
				}
				break;
		}
	}

	super(View, self, layoutSubviews);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	if (event->type == SDL_MOUSEBUTTONUP) {

		CollectionView *this = (CollectionView *) self;

		if ($((Control *) this->scrollView, highlighted) == false) {
			if ($((View *) this->contentView, didReceiveEvent, event)) {

				const SDL_Point point = {
					.x = event->button.x,
					.y = event->button.y
				};

				IndexPath *indexPath = $(this, indexPathForItemAtPoint, &point);

				const CollectionItemView *item = $(this, itemAtIndexPath, indexPath);
				if (item) {

					switch (self->selection) {
						case ControlSelectionNone:
							break;
						case ControlSelectionSingle:
							if (item->isSelected == false) {
								$(this, deselectAll);
								$(this, selectItemAtIndexPath, indexPath);
							}
							break;
						case ControlSelectionMultiple:
							if (SDL_GetModState() & (KMOD_CTRL | KMOD_GUI)) {
								if (item->isSelected) {
									$(this, deselectItemAtIndexPath, indexPath);
								} else {
									$(this, selectItemAtIndexPath, indexPath);
								}
							} else {
								$(this, deselectAll);
								$(this, selectItemAtIndexPath, indexPath);
							}
							break;
					}

					if (this->delegate.didModifySelection) {
						Array *selectionIndexPaths = $(this, selectionIndexPaths);

						this->delegate.didModifySelection(this, selectionIndexPaths);

						release(selectionIndexPaths);
					}
				}

				release(indexPath);
				return true;
			}
		}
	}

	return super(Control, self, captureEvent, event);
}

#pragma mark - CollectionView

/**
 * @brief ArrayEnumerator for all item deselection.
 */
static void deselectAll_enumerate(const Array *array, ident obj, ident data) {
	$((CollectionItemView *) obj, setSelected, false);
}

/**
 * @fn void CollectionView::deselectAll(CollectionView *self)
 * @memberof CollectionView
 */
static void deselectAll(CollectionView *self) {
	$((Array *) self->items, enumerateObjects, deselectAll_enumerate, NULL);
}

/**
 * @fn void CollectionView::deselectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath)
 * @memberof CollectionView
 */
static void deselectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath) {

	if (indexPath) {
		CollectionItemView *item = $(self, itemAtIndexPath, indexPath);
		if (item) {
			$(item, setSelected, false);
		}
	}
}

/**
 * @brief ArrayEnumerator for item deselection.
 */
static void deselectItemsAtIndexPaths_enumerate(const Array *array, ident obj, ident data) {
	$((CollectionItemView *) obj, setSelected, false);
}

/**
 * @fn void CollectionView::deselectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths)
 * @memberof CollectionView
 */
static void deselectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths) {

	if (indexPaths) {
		$(indexPaths, enumerateObjects, deselectItemsAtIndexPaths_enumerate, self);
	}
}

/**
 * @fn IndexPath *CollectionView::indexPathForItemAtPoint(const CollectionView *self, const SDL_Point *point)
 * @memberof CollectionView
 */
static IndexPath *indexPathForItemAtPoint(const CollectionView *self, const SDL_Point *point) {

	if (self->itemSize.w && self->itemSize.h) {

		const SDL_Rect frame = $(self->contentView, renderFrame);

		const int itemWidth = self->itemSize.w + self->itemSpacing.w;
		const int itemHeight = self->itemSize.h + self->itemSpacing.h;

		const int rows = frame.h / itemHeight;
		const int cols = frame.w / itemWidth;

		const int x = point->x - frame.x;
		const int y = point->y - frame.y;

		const int row = y / itemHeight;
		const int col = x / itemWidth;

		int index;
		switch (self->axis) {
			case CollectionViewAxisVertical:
				index = row * cols + col;
				break;
			case CollectionViewAxisHorizontal:
				index = col * rows + row;
				break;
		}

		if (index < self->items->array.count) {
			return $(alloc(IndexPath), initWithIndex, index);
		}
	}

	return NULL;
}

/**
 * @fn IndexPath *CollectionView::indexPathForItem(const CollectionView *self, const CollectionItemView *item)
 * @memberof CollectionView
 */
static IndexPath *indexPathForItem(const CollectionView *self, const CollectionItemView *item) {

	const ssize_t index = $((Array *) self->items, indexOfObject, (ident) item);
	if (index > -1) {
		return $(alloc(IndexPath), initWithIndex, index);
	}

	return NULL;
}

/**
 * @fn CollectionView *CollectionView::initWithFrame(CollectionView *self, const SDL_Rect *frame, ControlStyle style)
 * @memberof CollectionView
 */
static CollectionView *initWithFrame(CollectionView *self, const SDL_Rect *frame, ControlStyle style) {

	self = (CollectionView *) super(Control, self, initWithFrame, frame, style);
	if (self) {

		self->items = $$(MutableArray, array);

		self->contentView = $(alloc(View), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->autoresizingMask = ViewAutoresizingContain;

		self->scrollView = $(alloc(ScrollView), initWithFrame, NULL, style);
		assert(self->scrollView);

		self->scrollView->control.view.autoresizingMask = ViewAutoresizingFill;

		$(self->scrollView, setContentView, self->contentView);

		$((View *) self, addSubview, (View *) self->scrollView);

		if (self->control.style == ControlStyleDefault) {

			self->itemSize.w = DEFAULT_COLLECTION_VIEW_ITEM_SIZE;
			self->itemSize.h = DEFAULT_COLLECTION_VIEW_ITEM_SIZE;

			self->itemSpacing.w = DEFAULT_COLLECTION_VIEW_HORIZONTAL_SPACING;
			self->itemSpacing.h = DEFAULT_COLLECTION_VIEW_VERTICAL_SPACING;

			self->control.view.backgroundColor = Colors.Black;
			self->control.view.backgroundColor.a = 48;

			self->control.view.padding.top = 0;
			self->control.view.padding.right = 0;
			self->control.view.padding.bottom = 0;
			self->control.view.padding.left = 0;

			self->contentView->padding.top = DEFAULT_COLLECTION_VIEW_VERTICAL_SPACING;
			self->contentView->padding.right = DEFAULT_COLLECTION_VIEW_HORIZONTAL_SPACING;
			self->contentView->padding.bottom = DEFAULT_COLLECTION_VIEW_VERTICAL_SPACING;
			self->contentView->padding.left = DEFAULT_COLLECTION_VIEW_HORIZONTAL_SPACING;
		}
	}
	
	return self;
}

/**
 * @fn CollectionItemView *CollectionView::itemAtIndexPath(const CollectionView *self, const IndexPath *indexPath)
 * @memberof CollectionView
 */
static CollectionItemView *itemAtIndexPath(const CollectionView *self, const IndexPath *indexPath) {

	if (indexPath) {
		const Array *items = (Array *) self->items;
		const size_t index = $(indexPath, indexAtPosition, 0);

		if (index < items->count) {
			return $(items, objectAtIndex, index);
		}
	}

	return NULL;
}

/**
 * @brief ArrayEnumerator to remove CollectionItemViews from the collection's contentView.
 */
static void reloadData_removeItems(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj);
}

/**
 * @fn void CollectionView::reloadData(CollectionView *self)
 * @memberof CollectionView
 */
static void reloadData(CollectionView *self) {

	assert(self->dataSource.numberOfItems);
	assert(self->delegate.itemForObjectAtIndexPath);

	$((Array *) self->items, enumerateObjects, reloadData_removeItems, self->contentView);
	$(self->items, removeAllObjects);

	const size_t numberOfItems = self->dataSource.numberOfItems(self);
	for (size_t i = 0; i < numberOfItems; i++) {

		IndexPath *indexPath = $(alloc(IndexPath), initWithIndex, i);

		CollectionItemView *item = self->delegate.itemForObjectAtIndexPath(self, indexPath);
		assert(item);
		
		$(self->items, addObject, item);
		$(self->contentView, addSubview, (View *) item);

		release(item);
		release(indexPath);
	}

	self->control.view.needsLayout = true;
}

/**
 * @brief ArrayEnumerator for all item selection.
 */
static void selectAll_enumerate(const Array *array, ident obj, ident data) {
	$((CollectionItemView *) obj, setSelected, true);
}

/**
 * @fn void CollectionView::selectAll(CollectionView *self)
 * @memberof CollectionView
 */
static void selectAll(CollectionView *self) {
	$((Array *) self->items, enumerateObjects, selectAll_enumerate, NULL);
}

/**
 * @fn Array *CollectionView::selectionIndexPaths(const CollectionView *self)
 * @memberof CollectionView
 */
static Array *selectionIndexPaths(const CollectionView *self) {

	MutableArray *array = $$(MutableArray, array);

	const Array *items = (Array *) self->items;
	for (size_t i = 0; i < items->count; i++) {

		const CollectionItemView *item = $(items, objectAtIndex, i);
		if (item->isSelected) {

			IndexPath *indexPath = $(self, indexPathForItem, item);
			$(array, addObject, indexPath);

			release(indexPath);
		}
	}

	return (Array *) array;
}

/**
 * @fn void CollectionView::selectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath)
 * @memberof CollectionView
 */
static void selectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath) {

	if (indexPath) {
		CollectionItemView *item = $(self, itemAtIndexPath, indexPath);
		if (item) {
			$(item, setSelected, true);
		}
	}
}

/**
 * @brief ArrayEnumerator for item selection.
 */
static void selectItemsAtIndexPaths_enumerate(const Array *array, ident obj, ident data) {
	$((CollectionView *) data, selectItemAtIndexPath, (IndexPath *) obj);
}

/**
 * @fn void CollectionView::selectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths)
 * @memberof CollectionView
 */
static void selectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths) {

	if (indexPaths) {
		$(indexPaths, enumerateObjects, selectItemsAtIndexPaths_enumerate, self);
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

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;

	((CollectionViewInterface *) clazz->def->interface)->deselectAll = deselectAll;
	((CollectionViewInterface *) clazz->def->interface)->deselectItemAtIndexPath = deselectItemAtIndexPath;
	((CollectionViewInterface *) clazz->def->interface)->deselectItemsAtIndexPaths = deselectItemsAtIndexPaths;
	((CollectionViewInterface *) clazz->def->interface)->indexPathForItem = indexPathForItem;
	((CollectionViewInterface *) clazz->def->interface)->indexPathForItemAtPoint = indexPathForItemAtPoint;
	((CollectionViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((CollectionViewInterface *) clazz->def->interface)->itemAtIndexPath = itemAtIndexPath;
	((CollectionViewInterface *) clazz->def->interface)->reloadData = reloadData;
	((CollectionViewInterface *) clazz->def->interface)->selectAll = selectAll;
	((CollectionViewInterface *) clazz->def->interface)->selectionIndexPaths = selectionIndexPaths;
	((CollectionViewInterface *) clazz->def->interface)->selectItemAtIndexPath = selectItemAtIndexPath;
	((CollectionViewInterface *) clazz->def->interface)->selectItemsAtIndexPaths = selectItemsAtIndexPaths;
}

/**
 * @fn Class *CollectionView::_CollectionView(void)
 * @memberof CollectionView
 */
Class *_CollectionView(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "CollectionView";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(CollectionView);
		clazz.interfaceOffset = offsetof(CollectionView, interface);
		clazz.interfaceSize = sizeof(CollectionViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

