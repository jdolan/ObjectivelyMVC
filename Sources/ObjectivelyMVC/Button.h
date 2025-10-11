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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/ImageView.h>

/**
 * @file
 * @brief Buttons are Controls that respond to click events.
 */

typedef struct ButtonDelegate ButtonDelegate;

typedef struct Button Button;
typedef struct ButtonInterface ButtonInterface;

typedef void (*ButtonDelegateFunction)(Button *button);

/**
 * @brief The Button delegate protocol.
 */
struct ButtonDelegate {

  /**
   * @brief The delegate self-reference.
   */
  ident self;

  /**
   * @brief The user data.
   */
  ident data;

  /**
   * @brief Called when the Button is clicked.
   * @param button The Button.
   */
  void (*didClick)(Button *);
};

/**
 * @brief Buttons are Controls that respond to click events.
 * @extends Control
 * @ingroup Controls
 */
struct Button {

  /**
   * @brief The superclass.
   */
  Control control;

  /**
   * @brief The interface.
   * @protected
   */
  ButtonInterface *interface;

  /**
   * @brief The ButtonDelegate.
   */
  ButtonDelegate delegate;

  /**
   * @brief The image.
   */
  ImageView *image;

  /**
   * @brief The title.
   */
  Text *title;
};

/**
 * @brief The Button interface.
 */
struct ButtonInterface {

  /**
   * @brief The superclass interface.
   */
  ControlInterface controlInterface;

  /**
   * @fn Button *Button::initWithFrame(Button *self, const SDL_Rect *frame)
   * @brief Initializes this Button with the specified frame and style.
   * @param self The Button.
   * @param frame The frame.
   * @return The initialized Button, or `NULL` on error.
   * @memberof Button
   */
  Button *(*initWithFrame)(Button *self, const SDL_Rect *frame);

  /**
   * @fn Button *Button::initWithImage(Button *self, Image *image)
   * @brief Initializes this Button with the sopecified Image.
   * @param self The Button.
   * @param image The Image.
   * @return The initialized Button, or `NULL` on error.
   * @memberof Button
   */
  Button *(*initWithImage)(Button *self, Image *image);

  /**
   * @fn Button *Button::initWithTitle(Button *self, const char *title)
   * @brief Initializes this Button with the specified title.
   * @param self The Button.
   * @param title The title text.
   * @return The initialized Button, or `NULL` on error.
   * @memberof Button
   */
  Button *(*initWithTitle)(Button *self, const char *title);
};

/**
 * @fn Class *Button::_Button(void)
 * @brief The Button archetype.
 * @return The Button Class.
 * @memberof Button
 */
OBJECTIVELYMVC_EXPORT Class *_Button(void);
