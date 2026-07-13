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
 * @brief The draggable handle of a ScrollBar.
 * @details ScrollHandle is the only interactive part of a ScrollBar: a Control
 * that, while grabbed, reports each increment of vertical mouse motion to its
 * delegate. Its owner (ScrollBar) maps that motion onto the scrolled content's
 * offset. Kept generic (no knowledge of what it scrolls).
 */

typedef struct ScrollHandle ScrollHandle;
typedef struct ScrollHandleInterface ScrollHandleInterface;

/**
 * @brief The ScrollHandle delegate protocol.
 */
typedef struct {

  /**
   * @brief The delegate self pointer (the ScrollBar).
   */
  ident self;

  /**
   * @brief Called for each mouse-motion increment while the handle is grabbed.
   * @param handle The ScrollHandle.
   * @param delta The vertical motion this event, in window pixels.
   */
  void (*didDrag)(ScrollHandle *handle, float delta);

} ScrollHandleDelegate;

/**
 * @brief A draggable scrollbar handle.
 * @extends Control
 */
struct ScrollHandle {

  /**
   * @brief The superclass.
   * @private
   */
  Control control;

  /**
   * @brief The interface.
   * @private
   */
  ScrollHandleInterface *interface;

  /**
   * @brief The delegate.
   */
  ScrollHandleDelegate delegate;
};

/**
 * @brief The ScrollHandle interface.
 */
struct ScrollHandleInterface {

  /**
   * @brief The superclass interface.
   */
  ControlInterface controlInterface;

  /**
   * @fn ScrollHandle *ScrollHandle::initWithFrame(ScrollHandle *self, const SDL_Rect *frame)
   * @brief Initializes this ScrollHandle with the given frame.
   * @param self The ScrollHandle.
   * @param frame The frame, or `NULL`.
   * @return The initialized ScrollHandle, or `NULL` on error.
   * @memberof ScrollHandle
   */
  ScrollHandle *(*initWithFrame)(ScrollHandle *self, const SDL_Rect *frame);
};

/**
 * @fn Class *ScrollHandle::_ScrollHandle(void)
 * @brief The ScrollHandle archetype.
 * @return The ScrollHandle Class.
 * @memberof ScrollHandle
 */
OBJECTIVELYMVC_EXPORT Class *_ScrollHandle(void);
