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

#pragma once

#include <Objectively/IndexPath.h>

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/CollectionItemView.h>
#include <ObjectivelyMVC/ScrollView.h>

/**
 * @file
 * @brief The CollectionView type.
 */

/**
 * @defgroup CollectionView Collection Views
 * @brief CollectionViews provide grid and gallery presentations of data.
 */

/**
 * @brief Axis constants for specifying a CollectionView's primary layout direction.
 * @ingroup CollectionView
 */
typedef enum {
	CollectionViewAxisVertical,
	CollectionViewAxisHorizontal
} CollectionViewAxis;

OBJECTIVELYMVC_EXPORT const EnumName CollectionViewAxisNames[];

typedef struct CollectionViewDataSource CollectionViewDataSource;
typedef struct CollectionViewDelegate CollectionViewDelegate;

typedef struct CollectionView CollectionView;
typedef struct CollectionViewInterface CollectionViewInterface;

/**
 * @brief The CollectionView data source protocol.
 * @ingroup CollectionView
 */
struct CollectionViewDataSource {

	/**
	 * @brief The data source self-reference.
	 */
	ident self;

	/**
	 * @param collectionView The CollectionView.
	 * @return The number of items in the CollectionView.
	 */
	size_t (*numberOfItems)(const CollectionView *collectionView);

	/**
	 * @brief Called by the CollectionView for the associated object of an item.
	 * @param collectionView The CollectionView.
	 * @param indexPath The index path.
	 * @return The object for the item at the given index path.
	 */
	ident (*objectForItemAtIndexPath)(const CollectionView *collectionView, const IndexPath *indexPath);
};

/**
 * @brief The CollectionView delegate protocol.
 * @ingroup CollectionView
 */
struct CollectionViewDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called by the CollectionView when items are selected or deselected.
	 * @param collectionView The CollectionView.
	 * @param selectionIndexPaths The index paths of the current selection.
	 * @remarks This function is optional.
	 */
	void (*didModifySelection)(CollectionView *collectionView, const Array *selectionIndexPaths);

	/**
	 * @brief Called by the CollectionView to instantiate items.
	 * @param collectionView The CollectionView.
	 * @param indexPath The index path.
	 * @return The item for the index path.
	 */
	CollectionItemView *(*itemForObjectAtIndexPath)(const CollectionView *collectionView, const IndexPath *indexPath);
};

/**
 * @brief CollectionViews display items in a grid.
 * @extends Control
 * @ingroup CollectionView Containers Controls
 */
struct CollectionView {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	CollectionViewInterface *interface;

	/**
	 * @brief The layout axis.
	 */
	CollectionViewAxis axis;

	/**
	 * @brief The content view.
	 */
	View *contentView;

	/**
	 * @brief The data source.
	 */
	CollectionViewDataSource dataSource;

	/**
	 * @brief The delegate.
	 */
	CollectionViewDelegate delegate;

	/**
	 * @brief The items.
	 */
	MutableArray *items;

	/**
	 * @brief The item size.
	 */
	SDL_Size itemSize;

	/**
	 * @brief The item spacing.
	 */
	SDL_Size itemSpacing;

	/**
	 * @brief The scroll view.
	 */
	ScrollView *scrollView;
};

/**
 * @brief The CollectionView interface.
 */
struct CollectionViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn void CollectionView::deselectAll(CollectionView *self)
	 * @brief Deselects all items in this CollectionView.
	 * @param self The CollectionView.
	 * @memberof CollectionView
	 */
	void (*deselectAll)(CollectionView *self);

	/**
	 * @fn void CollectionView::deselectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath)
	 * @brief Deselects the item at the given index path.
	 * @param self The CollectionView.
	 * @param indexPath The index path of the item to deselect.
	 * @memberof CollectionView
	 */
	void (*deselectItemAtIndexPath)(CollectionView *self, const IndexPath *indexPath);

	/**
	 * @fn void CollectionView::deselectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths)
	 * @brief Deselects the items at the given index paths.
	 * @param self The CollectionView.
	 * @param indexPaths The index paths of the items to deselect.
	 * @memberof CollectionView
	 */
	void (*deselectItemsAtIndexPaths)(CollectionView *self, const Array *indexPaths);

	/**
	 * @fn CollectionView *CollectionView::init(CollectionView *self, const SDL_Rect *frame)
	 * @brief Initializes this CollectionView with the specified frame and style.
	 * @param self The CollectionView.
	 * @param frame The frame.
	 * @return The initialized CollectionView, or `NULL` on error.
	 * @memberof CollectionView
	 */
	CollectionView *(*initWithFrame)(CollectionView *self, const SDL_Rect *frame);

	/**
	 * @fn IndexPath *CollectionView::indexPathForItem(const CollectionView *self, const CollectionItemView *item)
	 * @param self The CollectionView.
	 * @param item The item.
	 * @return The index path of `item`, or `NULL` if not found.
	 * @memberof CollectionView
	 */
	IndexPath *(*indexPathForItem)(const CollectionView *self, const CollectionItemView *item);

	/**
	 * @fn IndexPath CollectionView::indexPathForItemAtPoint(const CollectionView *self, const SDL_Point *point)
	 * @param self The CollectionView.
	 * @param point A point in window coordinate space.
	 * @return The index path of the item at the specified point, or `NULL`.
	 * @memberof CollectionView
	 */
	IndexPath *(*indexPathForItemAtPoint)(const CollectionView *self, const SDL_Point *point);

	/**
	 * @fn CollectionItemView *CollectionView::itemAtIndexPath(const CollectionView *self, const IndexPath *indexPath)
	 * @param self The CollectionView.
	 * @param indexPath An index path.
	 * @return The item at the specified index path, or `NULL`.
	 * @memberof CollectionView
	 */
	CollectionItemView *(*itemAtIndexPath)(const CollectionView *self, const IndexPath *indexPath);

	/**
	 * @fn SDL_Size CollectionView::naturalSize(const CollectionView *self)
	 * @param self The CollectionView.
	 * @return The natural size of this CollectionView, placing all items in a single column or row.
	 * @memberof CollectionView
	 */
	SDL_Size (*naturalSize)(const CollectionView *self);

	/**
	 * @fn void CollectionView::reloadData(CollectionView *self)
	 * @brief Reloads this CollectionView's visible items.
	 * @param self The CollectionView.
	 * @remarks This method must be called after changes to the data source or delegate. Failure to
	 * call this method after such changes leads to undefined behavior.
	 * @memberof CollectionView
	 */
	void (*reloadData)(CollectionView *self);

	/**
	 * @fn void CollectionView::selectAll(CollectionView *self)
	 * @brief Selects all items in this CollectionView.
	 * @param self The CollectionView.
	 * @memberof CollectionView
	 */
	void (*selectAll)(CollectionView *self);

	/**
	 * @fn Array *CollectionView::selectionIndexPaths(const CollectionView *self)
	 * @param self The CollectionView.
	 * @return An Array containing the index paths of all selected items.
	 * @memberof CollectionView
	 */
	Array *(*selectionIndexPaths)(const CollectionView *self);

	/**
	 * @fn void CollectionView::selectItemAtIndexPath(CollectionView *self, const IndexPath *indexPath)
	 * @brief Selects the item at the given index path.
	 * @param self The CollectionView.
	 * @param indexPath The index path of the item to select.
	 * @memberof CollectionView
	 */
	void (*selectItemAtIndexPath)(CollectionView *self, const IndexPath *indexPath);

	/**
	 * @fn void CollectionView::selectItemsAtIndexPaths(CollectionView *self, const Array *indexPaths)
	 * @brief Selects the items at the given index paths.
	 * @param self The CollectionView.
	 * @param indexPaths The index paths of the items to select.
	 * @memberof CollectionView
	 */
	void (*selectItemsAtIndexPaths)(CollectionView *self, const Array *indexPaths);
};

/**
 * @fn Class *CollectionView::_CollectionView(void)
 * @brief The CollectionView archetype.
 * @return The CollectionView Class.
 * @memberof CollectionView
 */
OBJECTIVELYMVC_EXPORT Class *_CollectionView(void);

