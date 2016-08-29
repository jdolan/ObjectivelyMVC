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

#pragma once

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/CollectionItemView.h>
#include <ObjectivelyMVC/ScrollView.h>

/**
 * @file
 *
 * @brief The CollectionView type.
 */

/**
 * @brief Axis constants for specifying a CollectionView's primary layout direction.
 */
typedef enum {
	CollectionViewAxisVertical,
	CollectionViewAxisHorizontal
} CollectionViewAxis;

typedef struct CollectionViewDataSource CollectionViewDataSource;
typedef struct CollectionViewDelegate CollectionViewDelegate;

typedef struct CollectionView CollectionView;
typedef struct CollectionViewInterface CollectionViewInterface;

/**
 * @brief The CollectionView data source protocol.
 */
struct CollectionViewDataSource {

	/**
	 * @return The number of items in the Collection.
	 */
	size_t (*numberOfItems)(const CollectionView *collectionView);

	/**
	 * @brief Called by the CollectionView for the associated object of an item.
	 *
	 * @param CollectionView The collection.
	 * @param index The item index.
	 *
	 * @return The object for the item at the given index.
	 */
	ident (*objectForItemAtIndex)(const CollectionView *collectionView, int index);
};

/**
 * @brief The CollectionView delegate protocol.
 */
struct CollectionViewDelegate {

	/**
	 * @brief Called by the CollectionView to instantiate items.
	 *
	 * @param CollectionView The collection.
	 * @param index The index.
	 *
	 * @return The item for the index.
	 */
	CollectionItemView *(*itemForObjectAtIndex)(const CollectionView *collectionView, int index);

	/**
	 * @brief Called by the CollectionView when the selection changes.
	 *
	 * @param CollectionView The collection.
	 *
	 * @remarks This function is optional.
	 */
	void (*selectionDidChange)(CollectionView *collectionView);
};

#define DEAFULT_COLLECTION_VIEW_PADDING 10
#define DEFAULT_COLLECTION_VIEW_HORIZONTAL_SPACING 10
#define DEFAULT_COLLECTION_VIEW_VERTICAL_SPACING 10
#define DEFAULT_COLLECTION_VIEW_ITEM_SIZE 48

/**
 * @brief CollectionViews display items in a grid.
 *
 * @extends View
 *
 * @ingroup CollectionViews
 */
struct CollectionView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	CollectionViewInterface *interface;

	/**
	 * @ The layout axis.
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

	/**
	 * @brief The selected item index, or `-1` when no item is selected.
	 */
	int selectedItem;
};

/**
 * @brief The CollectionView interface.
 */
struct CollectionViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;
	
	/**
	 * @fn CollectionView *CollectionView::init(CollectionView *self, const SDL_Rect *frame, ControlStyle style)
	 *
	 * @brief Initializes this CollectionView with the specified frame and style.
	 *
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 *
	 * @return The initialized CollectionView, or `NULL` on error.
	 *
	 * @memberof CollectionView
	 */
	CollectionView *(*initWithFrame)(CollectionView *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn int CollectionView::itemAtPoint(const CollectionView *self, const SDL_Point *point)
	 *
	 * @param point A point in window coordinate space.
	 *
	 * @return The item index at the specified point, or `-1` if none.
	 *
	 * @memberof CollectionView
	 */
	int (*itemAtPoint)(const CollectionView *self, const SDL_Point *point);

	/**
	 * @fn void CollectionView::reloadData(CollectionView *self)
	 *
	 * @brief Reloads this CollectionView's visible rows.
	 *
	 * @remarks This method must be called after changes to the data source or delegate. Failure to 
	 * call this method after such changes leads to undefined behavior.
	 *
	 * @memberof CollectionView
	 */
	void (*reloadData)(CollectionView *self);

	/**
	 * @fn void CollectionView::selectItemAtIndex(CollectionView *self, int index)
	 *
	 * @brief Selects the item at the given index.
	 *
	 * @param index The index of the item to select, or `-1` to clear the selection.
	 *
	 * @memberof CollectionView
	 */
	void (*selectItemAtIndex)(CollectionView *self, int index);
};

/**
 * @brief The CollectionView Class.
 */
extern Class _CollectionView;

