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

/**
 * @file
 * @brief The draggable "grabby-widget" of a Scrollbar.
 * @details ScrollThumb is the only interactive part of a Scrollbar: a Control
 * that, while grabbed, reports each increment of vertical mouse motion to its
 * delegate. Its owner (Scrollbar) maps that motion onto the scrolled view's
 * content offset. Kept generic (no knowledge of what it scrolls) and
 * deliberately not named like Slider's handle to avoid any control-name
 * collision.
 */

typedef struct ScrollThumb ScrollThumb;
typedef struct ScrollThumbInterface ScrollThumbInterface;

/**
 * @brief The ScrollThumb delegate protocol.
 */
typedef struct {

  /**
   * @brief The delegate self pointer (the Scrollbar).
   */
  ident self;

  /**
   * @brief Called for each mouse-motion increment while the thumb is grabbed.
   * @param thumb The ScrollThumb.
   * @param delta The vertical motion this event, in window pixels.
   */
  void (*didDrag)(ScrollThumb *thumb, int delta);

} ScrollThumbDelegate;

/**
 * @brief A draggable scrollbar thumb.
 * @extends Control
 */
struct ScrollThumb {

  /**
   * @brief The superclass.
   * @private
   */
  Control control;

  /**
   * @brief The interface.
   * @private
   */
  ScrollThumbInterface *interface;

  /**
   * @brief The delegate.
   */
  ScrollThumbDelegate delegate;
};

/**
 * @brief The ScrollThumb interface.
 */
struct ScrollThumbInterface {

  /**
   * @brief The superclass interface.
   */
  ControlInterface controlInterface;

  /**
   * @fn ScrollThumb *ScrollThumb::initWithFrame(ScrollThumb *self, const SDL_Rect *frame)
   * @brief Initializes this ScrollThumb with the given frame.
   * @param self The ScrollThumb.
   * @param frame The frame, or `NULL`.
   * @return The initialized ScrollThumb, or `NULL` on error.
   * @memberof ScrollThumb
   */
  ScrollThumb *(*initWithFrame)(ScrollThumb *self, const SDL_Rect *frame);
};

/**
 * @fn Class *ScrollThumb::_ScrollThumb(void)
 * @brief The ScrollThumb archetype.
 * @return The ScrollThumb Class.
 * @memberof ScrollThumb
 */
OBJECTIVELYMVC_EXPORT Class *_ScrollThumb(void);
