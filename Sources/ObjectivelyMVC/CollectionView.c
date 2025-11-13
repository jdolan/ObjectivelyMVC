/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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
#include <math.h>

#include "CollectionView.h"

const EnumName CollectionViewAxisNames[] = MakeEnumNames(
  MakeEnumAlias(CollectionViewAxisHorizontal, horizontal),
  MakeEnumAlias(CollectionViewAxisVertical, vertical)
);

#define _Class _CollectionView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  CollectionView *this = (CollectionView *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  release(this->contentView);
  release(this->items);
  release(this->scrollView);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  CollectionView *this = (CollectionView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("axis", InletTypeEnum, &this->axis, (ident) CollectionViewAxisNames),
    MakeInlet("item-size", InletTypeSize, &this->itemSize, NULL),
    MakeInlet("item-spacing", InletTypeSize, &this->itemSpacing, NULL)
  );

  $(self, bind, inlets, (Dictionary *) style->attributes);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((CollectionView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  CollectionView *this = (CollectionView *) self;

  super(View, self, layoutSubviews);

  const SDL_Rect bounds = $((View *) this->scrollView, bounds);

  int x = bounds.x, y = bounds.y;

  const Array *items = (Array *) this->items;
  for (size_t i = 0; i < items->count; i++) {

    CollectionItemView *item = (CollectionItemView *) $(items, objectAtIndex, i);

    $((View *) item, resize, &this->itemSize);
    $((View *) item, layoutIfNeeded);

    item->view.frame.x = x;
    item->view.frame.y = y;

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
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {

    CollectionView *this = (CollectionView *) self;

    if ($((Control *) this->scrollView, isHighlighted) == false) {
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
              if (SDL_GetModState() & (SDL_KMOD_CTRL | SDL_KMOD_GUI)) {
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
      $(self->contentView, invalidateStyle);
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

    const int rows = max(1, frame.h / itemHeight);
    const int cols = max(1, frame.w / itemWidth);

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

    if (index < (int) self->items->array.count) {
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
 * @fn CollectionView *CollectionView::initWithFrame(CollectionView *self, const SDL_Rect *frame)
 * @memberof CollectionView
 */
static CollectionView *initWithFrame(CollectionView *self, const SDL_Rect *frame) {

  self = (CollectionView *) super(Control, self, initWithFrame, frame);
  if (self) {
    self->items = $$(MutableArray, array);
    assert(self->items);

    self->contentView = $(alloc(View), initWithFrame, NULL);
    assert(self->contentView);

    $(self->contentView, addClassName, "contentView");
    $(self->contentView, addClassName, "container");

    self->scrollView = $(alloc(ScrollView), initWithFrame, NULL);
    assert(self->scrollView);

    $(self->scrollView, setContentView, self->contentView);
    $((View *) self, addSubview, (View *) self->scrollView);
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
 * @fn SDL_Size CollectionView::naturalSize(const CollectionView *self)
 * @memberof CollectionView
 */
static SDL_Size naturalSize(const CollectionView *self) {

  ViewPadding padding = MakePadding(0, 0, 0, 0);
  padding = AddPadding(padding, ((View *) self)->padding);
  padding = AddPadding(padding, ((View *) self->scrollView)->padding);
  padding = AddPadding(padding, ((View *) self->contentView)->padding);

  SDL_Size size = MakeSize(padding.left + padding.right, padding.top + padding.bottom);

  View *scrollView = (View *) self->scrollView;
  SDL_Size scrollViewSize;
  if (scrollView->autoresizingMask & ViewAutoresizingContain) {
    scrollViewSize = $(scrollView, sizeThatContains);
  } else if (scrollView->autoresizingMask & ViewAutoresizingFit) {
    scrollViewSize = $(scrollView, sizeThatFits);
  } else {
    scrollViewSize = $(scrollView, size);
  }

  switch (self->axis) {
    case CollectionViewAxisVertical: {
      int itemsPerRow = 1;
      int w = scrollViewSize.w;
      while (w > 0) {
        w -= self->itemSize.w;
        itemsPerRow++;
        if (w - self->itemSpacing.w < 0) {
          break;
        }
        w -= self->itemSpacing.w;
      }
      const int rows = ceilf(self->items->array.count / (float) itemsPerRow);
      size.w += max(self->itemSize.w, scrollViewSize.w);
      size.h += rows * (self->itemSize.h + self->itemSpacing.h);
    }
      break;
    case CollectionViewAxisHorizontal:{
      int itemsPerCol = 1;
      int h = scrollViewSize.h;
      while (h > 0) {
        h -= self->itemSize.h;
        itemsPerCol++;
        if (h - self->itemSpacing.h < 0) {
          break;
        }
        h -= self->itemSpacing.h;
      }
      const int cols = ceilf(self->items->array.count / (float) itemsPerCol);
      size.w += cols * (self->itemSize.w + self->itemSpacing.w);
      size.h += max(self->itemSize.h, scrollViewSize.h);
    }
      break;
  }

  return size;
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

  ((View *) self)->needsLayout = true;
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
      $(self->contentView, invalidateStyle);
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

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((CollectionViewInterface *) clazz->interface)->deselectAll = deselectAll;
  ((CollectionViewInterface *) clazz->interface)->deselectItemAtIndexPath = deselectItemAtIndexPath;
  ((CollectionViewInterface *) clazz->interface)->deselectItemsAtIndexPaths = deselectItemsAtIndexPaths;
  ((CollectionViewInterface *) clazz->interface)->indexPathForItem = indexPathForItem;
  ((CollectionViewInterface *) clazz->interface)->indexPathForItemAtPoint = indexPathForItemAtPoint;
  ((CollectionViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((CollectionViewInterface *) clazz->interface)->itemAtIndexPath = itemAtIndexPath;
  ((CollectionViewInterface *) clazz->interface)->naturalSize = naturalSize;
  ((CollectionViewInterface *) clazz->interface)->reloadData = reloadData;
  ((CollectionViewInterface *) clazz->interface)->selectAll = selectAll;
  ((CollectionViewInterface *) clazz->interface)->selectionIndexPaths = selectionIndexPaths;
  ((CollectionViewInterface *) clazz->interface)->selectItemAtIndexPath = selectItemAtIndexPath;
  ((CollectionViewInterface *) clazz->interface)->selectItemsAtIndexPaths = selectItemsAtIndexPaths;
}

/**
 * @fn Class *CollectionView::_CollectionView(void)
 * @memberof CollectionView
 */
Class *_CollectionView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "CollectionView",
      .superclass = _Control(),
      .instanceSize = sizeof(CollectionView),
      .interfaceOffset = offsetof(CollectionView, interface),
      .interfaceSize = sizeof(CollectionViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

