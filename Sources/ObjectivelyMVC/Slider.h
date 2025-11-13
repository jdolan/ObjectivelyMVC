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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/Text.h>

/**
 * @file
 * @brief A Control allowing users to drag a handle to select a numeric value.
 */

typedef struct SliderDelegate SliderDelegate;

typedef struct Slider Slider;
typedef struct SliderInterface SliderInterface;

/**
 * @brief The Slider delegate protocol.
 */
struct SliderDelegate {

  /**
   * @brief The delegate self-reference.
   */
  ident self;

  /**
   * @brief Delegate callback for Slider value modification.
   * @param slider The Slider.
   * @param value The value.
   */
  void (*didSetValue)(Slider *slider, double value);
};

/**
 * @brief A Control allowing users to drag a handle to select a numeric value.
 * @extends Control
 * @ingroup Controls
 */
struct Slider {

  /**
   * @brief The superclass.
   */
  Control control;

  /**
   * @brief The interface.
   * @protected
   */
  SliderInterface *interface;

  /**
   * @brief The slider bar.
   */
  View *bar;

  /**
   * @brief The delegate.
   */
  SliderDelegate delegate;

  /**
   * @brief The slider handle.
   * @private
   */
  View *handle;

  /**
   * @brief The Text displaying the current value.
   */
  Text *label;

  /**
   * @brief The label format, e.g. `"%0.01f"`.
   */
  char *labelFormat;

  /**
   * @brief The slider bounds.
   */
  double min, max;

  /**
   * @brief The step to increment on key events.
   */
  double step;

  /**
   * @brief Enable snapping the slider handle to the step
   */
  bool snapToStep;

  /**
   * @brief The slider value.
   */
  double value;
};

/**
 * @brief The Slider interface.
 */
struct SliderInterface {

  /**
   * @brief The superclass interface.
   */
  ControlInterface controlInterface;

  /**
   * @fn void Slider::formatLabel(Slider *self)
   * @brief Forces an update on this Slider's label.
   * @param self The Slider.
   * @memberof Slider
   */
  void (*formatLabel)(Slider *self);

  /**
   * @fn Slider *Slider::init(Slider *self)
   * @brief Initializes this Slider.
   * @param self The Slider.
   * @return The initialized Slider, or `NULL` on error.
   * @memberof Slider
   */
  Slider *(*initWithFrame)(Slider *self, const SDL_Rect *frame);

  /**
   * @fn void Slider::setLabelFormat(Slider *self, const char *labelFormat)
   * @brief Changes this Slider's label format and calls appropriate update functions
   * @param self The Slider.
   * @param labelFormat The new label format.
   * @memberof Slider
   */
  void (*setLabelFormat)(Slider *self, const char *labelFormat);

  /**
   * @fn void Slider::setValue(Slider *self, double value)
   * @brief Sets this Slider's value, invalidating its layout and notifying the delegate.
   * @param self The Slider.
   * @param value The value.
   * @memberof Slider
   */
  void (*setValue)(Slider *self, double value);
};

/**
 * @fn Class *Slider::_Slider(void)
 * @brief The Slider archetype.
 * @return The Slider Class.
 * @memberof Slider
 */
OBJECTIVELYMVC_EXPORT Class *_Slider(void);
