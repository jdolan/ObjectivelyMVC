/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>

#include <Objectively/Types.h>

#undef interface

#if defined(_MSC_VER)
 #include "WindowlyMVC.h"
#endif

#ifndef OBJECTIVELYMVC_EXPORT
 #define OBJECTIVELYMVC_EXPORT extern
#endif

/**
 * @file
 * @brief ObjectivelyMVC base types.
 */

typedef struct View View;
typedef struct ViewController ViewController;

/**
 * @brief A function type for View enumeration
 * @param view The View.
 * @param data User data.
 */
typedef void (*ViewEnumerator)(View *view, ident data);

/**
 * @brief View event relay codes.
 */
typedef enum {
  /**
   * @brief No event.
   */
  ViewEventNone,

  /**
   * @brief A View has received a key down event.
   */
  ViewEventKeyDown,

  /**
   * @brief A View has received a key up event.
   */
  ViewEventKeyUp,

  /**
   * @brief A View has received a mouse button down event.
   */
  ViewEventMouseButtonDown,

  /**
   * @brief A View has received a mouse button up event.
   */
  ViewEventMouseButtonUp,

  /**
   * @brief The mouse has entered a View's frame.
   */
  ViewEventMouseEnter,

  /**
   * @brief The mouse has left a View's frame.
   */
  ViewEventMouseLeave,

  /**
   * @brief The mouse has moved within a View's frame.
   */
  ViewEventMouseMotion,

  /**
   * @brief A Control has lost focus.
   */
  ViewEventBlur,

  /**
   * @brief A Control's input value has changed.
   */
  ViewEventChange,

  /**
   * @brief A Control received one or more click events.
   */
  ViewEventClick,

  /**
   * @brief A Control's state has focus.
   */
  ViewEventFocus,

  /**
   * @brief User-defined ViewEvents.
   */
  ViewEventUserDefined,
} ViewEvent;

/**
 * @brief The custom `SDL_event` type for View event relaying.
 * @details When a View responds to an input event, it may emit an output event. The application
 * may handle these events to e.g. play sound effects.
 */
OBJECTIVELYMVC_EXPORT Uint32 MVC_VIEW_EVENT;

/**
 * @brief The custom `SDL_event` type for notifications.
 */
OBJECTIVELYMVC_EXPORT Uint32 MVC_NOTIFICATION_EVENT;

typedef struct SDL_Size SDL_Size;

/**
 * @brief The SDL_Size type.
 */
struct SDL_Size {
  int w, h;
};

/**
 * @brief Creates an SDL_Point with the given coordinates.
 */
#define MakePoint(x, y) (SDL_Point) { (x), (y) }

/**
 * @brief Creates an SDL_Rect with the given origin and size.
 */
#define MakeRect(x, y, w, h) (SDL_Rect) { (x), (y), (w), (h) }

/**
 * @brief Creates an SDL_Size with the given dimensions.
 */
#define MakeSize(w, h) (SDL_Size) { (w), (h) }
