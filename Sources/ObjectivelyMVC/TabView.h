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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/TabViewItem.h>

/**
 * @file
 * @brief Tabbed pages within a single View.
 */

#define DEFAULT_TAB_VIEW_PADDING 12

typedef struct TabViewDelegate TabViewDelegate;

typedef struct TabView TabView;
typedef struct TabViewInterface TabViewInterface;

/**
 * @brief The TabViewDelegate.
 */
struct TabViewDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called when a tab is added.
	 * @param tabView The TabView.
	 * @param tabViewItem The TabViewItem.
	 */
	void (*didAddTabViewItem)(TabView *tabView, TabViewItem *tabViewItem);

	/**
	 * @brief Called when a tab is selected.
	 * @param tabView The TabView.
	 * @param tabViewItem The selected TabViewItem.
	 */
	void (*didSelectTabViewItem)(TabView *tabView, TabViewItem *tabViewItem);

	/**
	 * @brief Called when a tab is to be removed.
	 * @param tabView The TabView.
	 * @param tabViewItem The TabViewItem.
	 */
	void (*willRemoveTabViewItem)(TabView *tabView, TabViewItem *tabViewItem);
};

/**
 * @brief TabViews allow for the display of multiple pages of information within a single view.
 * @extends View
 * @ingroup Containers
 */
struct TabView {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TabViewInterface *interface;

	/**
	 * @brief The TabViewDelegate.
	 */
	TabViewDelegate delegate;

	/**
	 * @brief The selected TabViewItem.
	 */
	TabViewItem *selectedTabViewItem;

	/**
	 * @brief The TabViewItems.
	 */
	MutableArray *tabViewItems;
};

/**
 * @brief The TabView interface.
 */
struct TabViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn void TabView::addTabViewItem(TabView *self, TabViewItem *tabViewItem)
	 * @brief Adds the specified TabViewItem to this TabView.
	 * @param self The TabView.
	 * @param tabViewItem The TabViewItem.
	 * @memberof TabView
	 */
	void (*addTabViewItem)(TabView *self, TabViewItem *tabViewItem);

	/**
	 * @fn TabView *TabView::initWithFrame(TabView *self, const SDL_Rect *frame)
	 * @brief Initializes this TabView with the specified frame.
	 * @param self The TabView.
	 * @param frame The frame.
	 * @return The initialized TabView, or `NULL` on error.
	 * @memberof TabView
	 */
	TabView *(*initWithFrame)(TabView *self, const SDL_Rect *frame);

	/**
	 * @fn void TabView::selectTabViewItem(TabView *self, TabViewItem *tabViewItem)
	 * @brief Selects the specified TabViewItem.
	 * @param self The TabView.
	 * @param tabViewItem The TabViewItem.
	 * @memberof TabView
	 */
	void (*selectTabViewItem)(TabView *self, TabViewItem *tabViewItem);

	/**
	 * @fn void TabView::removeTabViewItem(TabView *self, TabViewItem *tabViewItem)
	 * @brief Removes the specified TabViewItem from this TabView.
	 * @param self The TabView.
	 * @param tabViewItem The TabViewItem.
	 * @memberof TabView
	 */
	void (*removeTabViewItem)(TabView *self, TabViewItem *tabViewItem);
};

/**
 * @fn Class *TabView::_TabView(void)
 * @brief The TabView archetype.
 * @return The TabView Class.
 * @memberof TabView
 */
OBJECTIVELY_EXPORT Class *_TabView(void);
