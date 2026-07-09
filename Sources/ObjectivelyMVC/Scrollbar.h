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

#include <ObjectivelyMVC/Button.h>
#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/StackView.h>

#include <ObjectivelyMVC/ScrollThumb.h>

/**
 * @file
 * @brief A visible, draggable vertical scrollbar for any scrolling StackView.
 * @details StackView can opt in to panning its own subviews (see the `scroll`
 * style attribute) but draws no indicator. Scrollbar adds one, bound directly
 * to the StackView it drives -- no wrapping ScrollView required. It is a
 * container View (the "bar base") that lays its three regions out vertically
 * by hand -- fixed caps top and bottom, the panel filling between them. All
 * interactivity lives in its children: the ScrollThumb drags, the adorners
 * step-scroll on click.
 *
 * Anatomy:
 *
 *   Scrollbar (bar base, vertical View)
 *   |-- topAdorner     step-up cap at the top, height = -adorner-size
 *   |-- scrollPanel    the middle track the thumb travels within
 *   |     `-- thumb    the ScrollThumb grabby-widget
 *   `-- bottomAdorner  step-down cap at the bottom, height = -adorner-size
 *
 * The thumb's size tracks the visible/content ratio and its position tracks
 * the StackView's contentOffset; dragging the thumb scrolls the content,
 * clicking an adorner steps it, and the bar re-syncs the thumb whenever the
 * content's offset changes (StackView fires no didScroll callback). The whole
 * bar may sit on the LEFT or RIGHT of the content (see -orientation).
 *
 * The bound StackView must opt in to scrolling itself -- set its `scroll`
 * style attribute (or `scrolls` field) to `true` -- Scrollbar only drives the
 * offset, it does not force scrolling on.
 *
 * Styling (all set in CSS, read in applyStyle):
 *   width          the bar thickness (standard View attribute)
 *   -adorner-size  the height of the top and bottom caps
 *   -adorner-shade per-channel brighten of -bgcolor for the caps (derive amount)
 *   -orientation   left | right -- which side of the content the bar occupies
 *   -bgcolor       the bar-base color; the adorners derive a calculated shade
 *                  of this so the caps read as part of the bar
 *   -fgcolor       the thumb (grabby control) color
 *   -thumb-min     the minimum thumb height in pixels
 *   -step          the pixels scrolled per adorner-cap click
 */

/**
 * @brief Which side of the content a Scrollbar occupies.
 */
typedef enum {
  ScrollbarOrientationRight,
  ScrollbarOrientationLeft
} ScrollbarOrientation;

/**
 * @brief String <-> ScrollbarOrientation mapping for the `-orientation` attribute.
 */
OBJECTIVELYMVC_EXPORT const EnumName ScrollbarOrientationNames[];

typedef struct Scrollbar Scrollbar;
typedef struct ScrollbarInterface ScrollbarInterface;

/**
 * @brief A draggable scrollbar bound to a scrolling StackView.
 * @extends View
 */
struct Scrollbar {

  /**
   * @brief The superclass.
   * @private
   */
  View view;

  /**
   * @brief The interface.
   * @private
   */
  ScrollbarInterface *interface;

  /**
   * @brief The StackView this bar drives. Weak reference (not retained).
   */
  StackView *stackView;

  /**
   * @brief The step-up cap at the top of the bar (.adorner). A click steps up.
   */
  Button *topAdorner;

  /**
   * @brief The middle track within which the thumb travels (.scrollPanel).
   */
  View *scrollPanel;

  /**
   * @brief The step-down cap at the bottom of the bar (.adorner). Click steps down.
   */
  Button *bottomAdorner;

  /**
   * @brief The grabby-widget: the draggable thumb (.thumb).
   */
  ScrollThumb *thumb;

  /**
   * @brief The contentOffset the thumb was last synced to (for change detection).
   * @private
   */
  SDL_Point syncedOffset;

  /**
   * @brief Which side of the content the bar sits on. Attribute `-orientation`.
   */
  ScrollbarOrientation orientation;

  /**
   * @brief The height of the top and bottom caps. Attribute `-adorner-size`.
   */
  int adornerSize;

  /**
   * @brief The bar-base color. Attribute `-bgcolor`. Adorners derive a shade.
   */
  SDL_Color bgColor;

  /**
   * @brief The thumb color. Attribute `-fgcolor`.
   */
  SDL_Color fgColor;

  /**
   * @brief Per-channel brighten applied to `bgColor` for the adorner caps, so
   * they read as part of the bar. Attribute `-adorner-shade`.
   */
  int adornerShade;

  /**
   * @brief Minimum thumb height in pixels. Attribute `-thumb-min`.
   */
  int thumbMin;

  /**
   * @brief Pixels scrolled per adorner-cap click. Attribute `-step`.
   */
  int step;
};

/**
 * @brief The Scrollbar interface.
 */
struct ScrollbarInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn Scrollbar *Scrollbar::initWithStackView(Scrollbar *self, StackView *stackView)
   * @brief Initializes this Scrollbar bound to the given StackView.
   * @param self The Scrollbar.
   * @param stackView The StackView to drive, or `NULL` (set later).
   * @return The initialized Scrollbar, or `NULL` on error.
   * @memberof Scrollbar
   */
  Scrollbar *(*initWithStackView)(Scrollbar *self, StackView *stackView);

  /**
   * @fn void Scrollbar::setStackView(Scrollbar *self, StackView *stackView)
   * @brief Binds this Scrollbar to the given StackView and syncs the thumb.
   * @details Does NOT itself enable scrolling -- the StackView must opt in on
   * its own (`scroll: true` in CSS, or `scrolls = true` in code). This keeps
   * opt-in single-sourced in the StackView's own configuration.
   * @param self The Scrollbar.
   * @param stackView The StackView to drive.
   * @memberof Scrollbar
   */
  void (*setStackView)(Scrollbar *self, StackView *stackView);

  /**
   * @fn void Scrollbar::update(Scrollbar *self)
   * @brief Resizes and repositions the thumb from the StackView's geometry.
   * @param self The Scrollbar.
   * @memberof Scrollbar
   */
  void (*update)(Scrollbar *self);
};

/**
 * @fn Class *Scrollbar::_Scrollbar(void)
 * @brief The Scrollbar archetype.
 * @return The Scrollbar Class.
 * @memberof Scrollbar
 */
OBJECTIVELYMVC_EXPORT Class *_Scrollbar(void);
