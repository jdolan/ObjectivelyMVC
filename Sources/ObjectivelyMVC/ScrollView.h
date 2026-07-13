/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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
#include <ObjectivelyMVC/ScrollBar.h>

/**
 * @file
 * @brief ScrollViews allow users to pan their internal contents.
 */

typedef struct ScrollViewDelegate ScrollViewDelegate;

typedef struct ScrollView ScrollView;
typedef struct ScrollViewInterface ScrollViewInterface;

/**
 * @brief The ScrollView delegate protocol.
 */
struct ScrollViewDelegate {

  /**
   * @brief The delegate self-reference.
   */
  ident self;

  /**
   * @brief Delegate callback for view contents scrolling.
   * @param scrollView The ScrollView.
   */
  void (*didScroll)(ScrollView *scrollView);
};

/**
 * @brief ScrollViews allow users to pan their internal contents.
 * @extends View
 * @ingroup Controls
 */
struct ScrollView {

  /**
   * @brief The superclass.
   */
  Control control;

  /**
   * @brief The interface.
   * @protected
   */
  ScrollViewInterface *interface;

  /**
   * @brief The content View offset.
   */
  SDL_Point contentOffset;

  /**
   * @brief The content View.
   */
  View *contentView;

  /**
   * @brief The scroll step, in pixels.
   */
  float step;

  /**
   * @brief The scrollbar. Always present; shown per `scrollBarVisibility`
   * (see the `scrollbar` style attribute / `setScrollBarVisibility`).
   */
  ScrollBar *scrollBar;

  /**
   * @brief Governs whether the scrollbar is shown. Attribute `scrollbar`.
   * @details `ScrollBarAuto` (the default) shows it only when the content
   * View's height exceeds this ScrollView's own.
   */
  ScrollBarVisibility scrollBarVisibility;
};

/**
 * @brief The ScrollView interface.
 */
struct ScrollViewInterface {

  /**
   * @brief The superclass interface.
   */
  ControlInterface controlInterface;

  /**
   * @fn ScrollView *ScrollView::initWithFrame(ScrollView *self, const SDL_Rect *frame)
   * @brief Initializes this ScrollView with the specified frame and style.
   * @param self The ScrollView.
   * @param frame The frame.
   * @return The initialized ScrollView, or `NULL` on error.
   * @memberof ScrollView
   */
  ScrollView *(*initWithFrame)(ScrollView *self, const SDL_Rect *frame);
 
  /**
   * @fn void ScrollView::scrollToOffset(ScrollView *self, const SDL_Point offset);
   * @brief Scrolls the content View to the specified offset.
   * @param self The ScrollView.
   * @param offset The offset.
   * @memberof ScrollView
   */
  void (*scrollToOffset)(ScrollView *self, const SDL_Point *offset);

  /**
   * @fn void ScrollView::setContentView(ScrollView *self, View *contentView)
   * @brief Sets the content view of this ScrollView.
   * @param self The ScrollView.
   * @param contentView The content view.
   * @memberof ScrollView
   */
  void (*setContentView)(ScrollView *self, View *contentView);

  /**
   * @fn void ScrollView::setScrollBarVisibility(ScrollView *self, ScrollBarVisibility visibility)
   * @brief Sets whether this ScrollView's ScrollBar is shown, hidden, or shown only as needed.
   * @param self The ScrollView.
   * @param visibility The ScrollBarVisibility.
   * @memberof ScrollView
   */
  void (*setScrollBarVisibility)(ScrollView *self, ScrollBarVisibility visibility);
};

/**
 * @fn Class *ScrollView::_ScrollView(void)
 * @brief The ScrollView archetype.
 * @return The ScrollView Class.
 * @memberof ScrollView
 */
OBJECTIVELYMVC_EXPORT Class *_ScrollView(void);
