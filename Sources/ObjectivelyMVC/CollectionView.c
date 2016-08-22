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

static void layoutSubviews(View *self) {

	CollectionView *this = (CollectionView *) self;

	const SDL_Rect bounds = $(self, bounds);

	int x = 0, y = 0;

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
					x = 0;
				}
				break;
			case CollectionViewAxisHorizontal:
				y += this->itemSize.h + this->itemSpacing.h;
				if (y + this->itemSize.h > bounds.h) {
					x += this->itemSize.w + this->itemSpacing.w;
					y = 0;
				}
				break;
		}
	}

	super(View, self, layoutSubviews);
}

#pragma mark - CollectionView

/**
 * @fn CollectionView *CollectionView::initWithFrame(CollectionView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof CollectionView
 */
static CollectionView *initWithFrame(CollectionView *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (CollectionView *) super(Control, self, initWithFrame, frame, style);
	if (self) {

		self->items = $$(MutableArray, array);

		self->contentView = $(alloc(View), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->autoresizingMask = ViewAutoresizingContain;

		self->scrollView = $(alloc(ScrollView), initWithFrame, NULL);
		assert(self->scrollView);

		self->scrollView->view.autoresizingMask = ViewAutoresizingFill;

		$(self->scrollView, setContentView, self->contentView);

		$((View *) self, addSubview, (View *) self->scrollView);

		if (self->control.style == ControlStyleDefault) {

			self->itemSize.w = DEFAULT_COLLECTION_VIEW_ITEM_SIZE;
			self->itemSize.h = DEFAULT_COLLECTION_VIEW_ITEM_SIZE;

			self->itemSpacing.w = DEFAULT_COLLECTION_VIEW_HORIZONTAL_SPACING;
			self->itemSpacing.h = DEFAULT_COLLECTION_VIEW_VERTICAL_SPACING;

			self->control.view.backgroundColor = Colors.Black;
			self->control.view.backgroundColor.a = 48;
		}
	}
	
	return self;
}

/**
 * @brief ArrayEnumerator to remove CollectionItemViews from the collection's contentView.
 */
static _Bool reloadData_removeItems(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj); return false;
}

/**
 * @fn void CollectionView::reloadData(CollectionView *self)
 *
 * @memberof CollectionView
 */
static void reloadData(CollectionView *self) {

	assert(self->dataSource.numberOfItems);
	assert(self->dataSource.objectForItemAtIndex);
	assert(self->delegate.itemForObjectAtIndex);

	$((Array *) self->items, enumerateObjects, reloadData_removeItems, self->contentView);
	$(self->items, removeAllObjects);

	const size_t numberOfItems = self->dataSource.numberOfItems(self);
	for (size_t i = 0; i < numberOfItems; i++) {

		CollectionItemView *item = self->delegate.itemForObjectAtIndex(self, i);
		item->object = self->dataSource.objectForItemAtIndex(self, i);

		$(self->items, addObject, item);
		$(self->contentView, addSubview, (View *) item);

		release(item);
	}

	self->selectedItem = -1;

	$((View *) self, layoutSubviews);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

	((CollectionViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((CollectionViewInterface *) clazz->interface)->reloadData = reloadData;
}

Class _CollectionView = {
	.name = "CollectionView",
	.superclass = &_Control,
	.instanceSize = sizeof(CollectionView),
	.interfaceOffset = offsetof(CollectionView, interface),
	.interfaceSize = sizeof(CollectionViewInterface),
	.initialize = initialize,
};

#undef _Class

