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
#include <ObjectivelyMVC/View.h>

#include <ObjectivelyMVC/ScrollHandle.h>

/**
 * @brief Forward declaration; ScrollView.h includes this header to embed a
 * ScrollBar, so it can't be included here without a cycle.
 */
typedef struct ScrollView ScrollView;

/**
 * @file
 * @brief A visible, draggable vertical scrollbar, owned by a ScrollView.
 * @details ScrollBar is a hideable subview a ScrollView owns internally (see
 * `ScrollView::setShowsScrollBar` / the `scrollbar` style attribute) -- it is
 * not meant to be instantiated directly by consumers. No arrows (matches
 * modern conventions, e.g. macOS/Xcode's own overlay scrollbars): ScrollBar
 * itself IS the track (clips its own subviews, fills the whole bar), and
 * clicking it above or below the handle pages the content by one viewport's
 * worth in that direction.
 *
 * Anatomy (AppKit-inspired: NSScroller's knob / knob slot):
 *
 *   ScrollBar (bar base + track, vertical View)
 *   `-- handle    the ScrollHandle grabby-widget
 *
 * The handle's size tracks the visible/content ratio and its position tracks
 * the ScrollView's contentOffset; dragging the handle scrolls the content.
 * Anything that changes either just flags `needsLayout` -- ScrollBar's own
 * `layoutSubviews` is what actually repositions/resizes the handle.
 *
 * Styling is plain View attributes, set in the stylesheet like any other
 * View -- no ScrollBar-specific CSS at all: `width`/`background-color` on
 * `ScrollBar` for the bar/track, and `background-color`/`min-height` on
 * `ScrollBar > .handle` for the handle and its minimum size.
 */

/**
 * @brief Governs whether a ScrollView's ScrollBar is shown.
 */
typedef enum {
  ScrollBarHide,
  ScrollBarShow,
  ScrollBarAuto
} ScrollBarVisibility;

/**
 * @brief String <-> ScrollBarVisibility mapping for the `scrollbar` attribute.
 */
OBJECTIVELYMVC_EXPORT const EnumName ScrollBarVisibilityNames[];

typedef struct ScrollBar ScrollBar;
typedef struct ScrollBarInterface ScrollBarInterface;

/**
 * @brief A draggable scrollbar, owned by a ScrollView.
 * @extends View
 */
struct ScrollBar {

  /**
   * @brief The superclass.
   * @private
   */
  View view;

  /**
   * @brief The interface.
   * @private
   */
  ScrollBarInterface *interface;

  /**
   * @brief The ScrollView this bar drives. Weak reference (not retained).
   */
  ScrollView *scrollView;

  /**
   * @brief The grabby-widget: the draggable handle (.handle). Its own
   * `min-height` (a standard View attribute) floors how small it can shrink.
   */
  ScrollHandle *handle;
};

/**
 * @brief The ScrollBar interface.
 */
struct ScrollBarInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn ScrollBar *ScrollBar::initWithScrollView(ScrollBar *self, ScrollView *scrollView)
   * @brief Initializes this ScrollBar bound to the given ScrollView.
   * @param self The ScrollBar.
   * @param scrollView The ScrollView to drive, or `NULL` (set later).
   * @return The initialized ScrollBar, or `NULL` on error.
   * @memberof ScrollBar
   */
  ScrollBar *(*initWithScrollView)(ScrollBar *self, ScrollView *scrollView);

  /**
   * @fn void ScrollBar::setScrollView(ScrollBar *self, ScrollView *scrollView)
   * @brief Binds this ScrollBar to the given ScrollView and syncs the handle.
   * @param self The ScrollBar.
   * @param scrollView The ScrollView to drive.
   * @memberof ScrollBar
   */
  void (*setScrollView)(ScrollBar *self, ScrollView *scrollView);
};

/**
 * @fn Class *ScrollBar::_ScrollBar(void)
 * @brief The ScrollBar archetype.
 * @return The ScrollBar Class.
 * @memberof ScrollBar
 */
OBJECTIVELYMVC_EXPORT Class *_ScrollBar(void);
