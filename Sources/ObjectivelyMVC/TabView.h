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

#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/PageView.h>
#include <ObjectivelyMVC/TabViewItem.h>

/**
 * @file
 * @brief Tabbed pages within a single View.
 */

#define DEFAULT_TAB_VIEW_ITEM_SPACING 4
#define DEFAULT_TAB_VIEW_SPACING 4

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
	 * @param tab The TabViewItem.
	 */
	void (*didAddTab)(TabView *tabView, TabViewItem *tab);

	/**
	 * @brief Called when a tab is selected.
	 * @param tabView The TabView.
	 * @param tab The selected TabViewItem.
	 */
	void (*didSelectTab)(TabView *tabView, TabViewItem *tab);

	/**
	 * @brief Called when a tab is to be removed.
	 * @param tabView The TabView.
	 * @param tab The TabViewItem.
	 */
	void (*willRemoveTab)(TabView *tabView, TabViewItem *tab);
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
	StackView stackView;

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
	TabViewItem *selectedTab;

	/**
	 * @brief The tab content container.
	 */
	PageView *tabPageView;

	/**
	 * @brief The tab selection container.
	 */
	StackView *tabSelectionView;

	/**
	 * @brief The TabViewItems.
	 */
	MutableArray *tabs;
};

/**
 * @brief The TabView interface.
 */
struct TabViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn void TabView::addTab(TabView *self, TabViewItem *tab)
	 * @brief Adds the specified TabViewItem to this TabView.
	 * @param self The TabView.
	 * @param tab The TabViewItem.
	 * @memberof TabView
	 */
	void (*addTab)(TabView *self, TabViewItem *tab);

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
	 * @fn void TabView::selectTab(TabView *self, TabViewItem *tab)
	 * @brief Selects the specified TabViewItem.
	 * @param self The TabView.
	 * @param tab The TabViewItem.
	 * @memberof TabView
	 */
	void (*selectTab)(TabView *self, TabViewItem *tab);

	/**
	 * @fn void TabView::removeTab(TabView *self, TabViewItem *tab)
	 * @brief Removes the specified TabViewItem from this TabView.
	 * @param self The TabView.
	 * @param tab The TabViewItem.
	 * @memberof TabView
	 */
	void (*removeTab)(TabView *self, TabViewItem *tab);

	/**
	 * @fn TabViewItem *TabView::tabWithIdentifier(const TabView *self, const char *identifier)
	 * @param self The TabView.
	 * @param identifier The identifier of the desired TabViewItem.
	 * @return The TabViewItem with the specified identifier.
	 * @memberof TabView
	 */
	TabViewItem *(*tabWithIdentifier)(const TabView *self, const char *identifier);
};

/**
 * @fn Class *TabView::_TabView(void)
 * @brief The TabView archetype.
 * @return The TabView Class.
 * @memberof TabView
 */
OBJECTIVELYMVC_EXPORT Class *_TabView(void);
