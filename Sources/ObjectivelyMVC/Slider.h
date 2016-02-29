/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#ifndef _ObjectivelyMVC_Slider_h_
#define _ObjectivelyMVC_Slider_h_

#include <ObjectivelyMVC/Control.h>

/**
 * @file
 *
 * @brief Sliders.
 */

#define DEFAULT_SLIDER_HANDLE_WIDTH 12
#define DEFAULT_SLIDER_HANDLE_HEIGHT 18

#define DEFAULT_SLIDER_WIDTH 100

typedef struct Slider Slider;
typedef struct SliderInterface SliderInterface;

/**
 * @brief The Slider type.
 *
 * @extends Control
 *
 * @ingroup Controls
 */
struct Slider {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Control control;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	SliderInterface *interface;
	
	/**
	 * @brief The slider handle.
	 *
	 * @private
	 */
	Control *handle;
	
	/**
	 * @brief The slider bounds.
	 */
	double min, max;
	
	/**
	 * @brief The step to increment on key events.
	 */
	double step;
	
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
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;
	
	/**
	 * @fn Slider *Slider::init(Slider *self)
	 *
	 * @brief Initializes this Slider.
	 *
	 * @return The initialized Slider, or `NULL` on error.
	 *
	 * @memberof Slider
	 */
	Slider *(*initWithFrame)(Slider *self, const SDL_Rect *frame, ControlStyle style);
	
	// ..
};

/**
 * @brief The Slider Class.
 */
extern Class _Slider;

#endif /* _ObjectivelyMVC_Slider_h_ */

