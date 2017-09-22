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

#pragma once

#include <ObjectivelyMVC/Input.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/Slider.h>

/**
 * @file
 * @brief HSV color picker.
 */

typedef struct HSVColorPickerDelegate HSVColorPickerDelegate;

typedef struct HSVColorPicker HSVColorPicker;
typedef struct HSVColorPickerInterface HSVColorPickerInterface;

/**
 * @brief The HSVColorPicker delegate protocol.
 */
struct HSVColorPickerDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called by the HSVColorPicker when color selection changes.
	 * @param hsvColorPicker The HSVColorPicker.
	 * @param hue The hue (0.0 - 360.0).
	 * @param saturation The saturation (0.0 - 1.0).
	 * @param value The brightness value (0.0 - 1.0).
	 */
	void (*didPickColor)(HSVColorPicker *hsvColorPicker, double hue, double saturation, double value);
};

/**
 * @brief The HSVColorPicker type.
 * @extends Control
 * @ingroup Controls
 */
struct HSVColorPicker {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	HSVColorPickerInterface *interface;

	/**
	 * @brief The color preview.
	 */
	View *colorView;

	/**
	 * @brief The delegate.
	 */
	HSVColorPickerDelegate delegate;

	/**
	 * @brief The color components.
	 */
	double hue, saturation, value;

	/**
	 * @brief The Sliders.
	 */
	Slider *hueSlider, *saturationSlider, *valueSlider;

	/**
	 * @brief The Inputs containing the Sliders.
	 */
	Input *hueInput, *saturationInput, *valueInput;

	/**
	 * @brief The StackView.
	 */
	StackView *stackView;
};

/**
 * @brief The HSVColorPicker interface.
 */
struct HSVColorPickerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn HSVColorPicker *HSVColorPicker::initWithFrame(HSVColorPicker *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this HSVColorPicker with the specified frame and style.
	 * @param self The HSVColorPicker.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized HSVColorPicker, or `NULL` on error.
	 * @memberof HSVColorPicker
	 */
	HSVColorPicker *(*initWithFrame)(HSVColorPicker *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn SDL_Color HSVColorPicker::rgbColor(const HSVColorPicker *self);
	 * @param self The HSVColorPicker.
	 * @return The RGB color.
	 * @memberof HSVColorPicker
	 */
	SDL_Color (*rgbColor)(const HSVColorPicker *self);

	/**
	 * @fn void HSVColorPicker::setColor(HSVColorPicker *self, double hue, double saturation, double value)
	 * @brief Sets the color of the HSVColorPicker.
	 * @param self The HSVColorPicker.
	 * @param hue The hue.
	 * @param saturation The saturation.
	 * @param value The brightness value.
	 * @memberof HSVColorPicker
	 */
	void (*setColor)(HSVColorPicker *self, double hue, double saturation, double value);
};

/**
 * @fn Class *HSVColorPicker::_HSVColorPicker(void)
 * @brief The HSVColorPicker archetype.
 * @return The HSVColorPicker Class.
 * @memberof HSVColorPicker
 */
OBJECTIVELYMVC_EXPORT Class *_HSVColorPicker(void);
