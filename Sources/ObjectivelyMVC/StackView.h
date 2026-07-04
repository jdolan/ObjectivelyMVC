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

#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief StackViews are containers that manage the arrangement of their subviews.
 */

/**
 * @defgroup Containers Containers
 * @brief Containers allow for grouping and positioning of related Views.
 * @remarks All containers have a default `autoResizingMask` of ViewAutoResizingContain.
 */

/**
 * @brief Axis constants for specifying a StackView's primary layout direction.
 */
typedef enum {
  StackViewAxisVertical,
  StackViewAxisHorizontal
} StackViewAxis;

OBJECTIVELYMVC_EXPORT const EnumName StackViewAxisNames[];

/**
 * @brief Distribution models for filling a StackView's frame.
 */
typedef enum {
  StackViewDistributionDefault,
  StackViewDistributionFill,
  StackViewDistributionFillEqually,
} StackViewDistribution;

OBJECTIVELYMVC_EXPORT const EnumName StackViewDistributionNames[];

typedef struct StackView StackView;
typedef struct StackViewInterface StackViewInterface;

/**
 * @brief StackViews are containers that manage the arrangement of their subviews.
 * @extends View
 * @ingroup Containers
 * @remarks A StackView can optionally pan its own stacked subviews when its
 * content overflows its frame. This is strictly opt-in: the `scroll` flag
 * defaults to `false`, so legacy layouts behave exactly as before unless the
 * `scroll` style attribute (or code) enables it. Scrolling lives directly on
 * StackView (it remains a View, not a Control/ScrollView) to avoid inheriting
 * framework defaults that would blanket every StackView in the UI.
 */
struct StackView {

  /**
   * @brief The superclass.
   */
  View view;

  /**
   * @brief The interface.
   * @protected
   */
  StackViewInterface *interface;

  /**
   * @brief The axis.
   */
  StackViewAxis axis;

  /**
   * @brief The distribution.
   */
  StackViewDistribution distribution;

  /**
   * @brief The subview spacing.
   */
  int spacing;

  /**
   * @brief If `true`, this StackView pans its subviews along its axis when its
   * content overflows its frame. Opt-in via the `scroll` style attribute.
   */
  bool scrolls;

  /**
   * @brief The scroll offset, in pixels. Negative along the primary axis as the
   * content is panned (the first subview moves off the leading edge). Only used
   * when `scrolls` is `true`.
   */
  SDL_Point contentOffset;

  /**
   * @brief The scroll step, in pixels, applied per mouse-wheel notch.
   */
  int step;
};

/**
 * @brief The StackView interface.
 */
struct StackViewInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn StackView *StackView::initWithFrame(StackView *self, const SDL_Rect *frame)
   * @brief Initializes this StackView with the specified frame.
   * @param self The StackView.
   * @param frame The frame.
   * @return The initialized StackView, or `NULL` on error.
   * @memberof StackView
   */
  StackView *(*initWithFrame)(StackView *self, const SDL_Rect *frame);

  /**
   * @fn void StackView::scrollToOffset(StackView *self, const SDL_Point *offset)
   * @brief Pans the stacked content to the given offset, clamped to the
   * scrollable range. No-op unless `scrolls` is `true`.
   * @param self The StackView.
   * @param offset The desired content offset (negative along the axis to advance).
   * @memberof StackView
   */
  void (*scrollToOffset)(StackView *self, const SDL_Point *offset);
};

/**
 * @fn Class *StackView::_StackView(void)
 * @brief The StackView archetype.
 * @return The StackView Class.
 * @memberof StackView
 */
OBJECTIVELYMVC_EXPORT Class *_StackView(void);
