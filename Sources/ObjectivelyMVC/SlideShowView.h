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

#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief SlideShowView cycles through a collection of Images with crossfade transitions.
 */

typedef struct SlideShowView SlideShowView;
typedef struct SlideShowViewInterface SlideShowViewInterface;

/**
 * @brief SlideShowView cycles through a collection of Images with crossfade transitions.
 * @details Add Image instances via `addImage`. The view starts displaying the first image
 * immediately upon arrival and advances to the next every `cycleInterval` milliseconds,
 * blending between images over `fadeDuration` milliseconds.
 * @extends View
 */
struct SlideShowView {

  /**
   * @brief The superclass.
   */
  View view;

  /**
   * @brief The interface.
   * @protected
   */
  SlideShowViewInterface *interface;

  /**
   * @brief The images to display.
   * @protected
   */
  MutableArray *images;

  /**
   * @brief The index of the currently displayed image.
   * @protected
   */
  size_t index;

  /**
   * @brief The duration of each slide in milliseconds, excluding the fade transition.
   */
  Uint32 slideDuration;

  /**
   * @brief The duration of the fade transition in milliseconds.
   */
  Uint32 fadeDuration;

  /**
   * @brief The bottom ImageView, showing the current slide.
   * @protected
   */
  ImageView *current;

  /**
   * @brief The top ImageView, fading in the next slide.
   * @protected
   */
  ImageView *next;

  /**
   * @brief The ticks at which the current fade began (0 = not fading).
   * @protected
   */
  Uint64 fadeStartedAt;

  /**
   * @brief The ticks at which the next advance should begin (0 = not scheduled).
   * @protected
   */
  Uint64 fadeEndedAt;
};

/**
 * @brief The SlideShowView interface.
 */
struct SlideShowViewInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn void SlideShowView::addImage(SlideShowView *self, Image *image)
   * @brief Appends an Image to the slideshow.
   * @param self The SlideShowView.
   * @param image The Image to add.
   * @remarks Thread-safe: may be called from a background thread before images arrive.
   * @memberof SlideShowView
   */
  void (*addImage)(SlideShowView *self, Image *image);

  /**
   * @fn SlideShowView *SlideShowView::initWithFrame(SlideShowView *self, const SDL_Rect *frame)
   * @brief Initializes this SlideShowView with the specified frame.
   * @param self The SlideShowView.
   * @param frame The frame, or `NULL` for the default frame.
   * @return The initialized SlideShowView, or `NULL` on error.
   * @memberof SlideShowView
   */
  SlideShowView *(*initWithFrame)(SlideShowView *self, const SDL_Rect *frame);
};

OBJECTIVELYMVC_EXPORT Class *_SlideShowView(void);
