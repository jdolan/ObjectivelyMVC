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

#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief PageViews manage their subviews as pages in a book.
 */

typedef struct PageViewDelegate PageViewDelegate;

typedef struct PageView PageView;
typedef struct PageViewInterface PageViewInterface;

/**
 * @brief The PageView delegate protocol.
 */
struct PageViewDelegate {

  /**
   * @brief The delegate self-reference.
   */
  ident self;

  /**
   * @brief Called when the current page is set.
   * @param pageView The PageView.
   */
  void (*didSetCurrentPage)(PageView *pageView);
};

/**
 * @brief PageViews manage their subviews as pages in a book.
 * @extends View
 * @ingroup Containers
 */
struct PageView {

  /**
   * @brief The superclass.
   */
  View view;

  /**
   * @brief The interface.
   * @protected
   */
  PageViewInterface *interface;

  /**
   * @brief The index of the current page.
   */
  View *currentPage;

  /**
   * @brief The delegate.
   */
  PageViewDelegate delegate;
};

/**
 * @brief The PageView interface.
 */
struct PageViewInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn PageView *PageView::initWithFrame(PageView *self, const SDL_Rect *frame)
   * @brief Initializes this PageView with the specified frame.
   * @param self The PageView.
   * @param frame The frame.
   * @return The initialized PageView, or `NULL` on error.
   * @memberof PageView
   */
  PageView *(*initWithFrame)(PageView *self, const SDL_Rect *frame);

  /**
   * @fn void PageView::setCurrentPage(PageView *self, View *currentPage)
   * @brief Presents the specified subview as the current page of this PageView.
   * @param self The PageView.
   * @param currentPage The subview to present.
   * @memberof PageView
   */
  void (*setCurrentPage)(PageView *self, View *currentPage);
};

/**
 * @fn Class *PageView::_PageView(void)
 * @brief The PageView archetype.
 * @return The PageView Class.
 * @memberof PageView
 */
OBJECTIVELYMVC_EXPORT Class *_PageView(void);
