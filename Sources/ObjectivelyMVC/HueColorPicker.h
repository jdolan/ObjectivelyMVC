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
 * @brief Hue color picker.
 */

typedef struct HueColorPickerDelegate HueColorPickerDelegate;

typedef struct HueColorPicker HueColorPicker;
typedef struct HueColorPickerInterface HueColorPickerInterface;

/**
 * @brief The HueColorPicker delegate protocol.
 */
struct HueColorPickerDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called by the HueColorPicker when hue selection changes.
	 * @param hueColorPicker The HueColorPicker.
	 * @param hue The hue (0.0 - 360.0).
	 * @param saturation The saturation (0.0 - 1.0).
	 * @param value The brightness value (0.0 - 1.0).
	 */
	void (*didPickColor)(HueColorPicker *hueColorPicker, double hue, double saturation, double value);
};

/**
 * @brief The HueColorPicker type.
 * @extends Control
 * @ingroup Controls
 */
struct HueColorPicker {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	HueColorPickerInterface *interface;

	/**
	 * @brief The hue preview.
	 */
	View *colorView;

	/**
	 * @brief The delegate.
	 */
	HueColorPickerDelegate delegate;

	/**
	 * @brief The Input.
	 */
	Input *hueInput;

	/**
	 * @brief The Slider.
	 */
	Slider *hueSlider;

	/**
	 * @brief The color componenets.
	 * @remarks Only `hue` is mutable via the interface.
	 */
	double hue, saturation, value;

	/**
	 * @brief The StackView.
	 */
	StackView *stackView;
};

/**
 * @brief The HueColorPicker interface.
 */
struct HueColorPickerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn HueColorPicker *HueColorPicker::initWithFrame(HueColorPicker *self, const SDL_Rect *frame)
	 * @brief Initializes this HueColorPicker with the specified frame.
	 * @param self The HueColorPicker.
	 * @param frame The frame.
	 * @return The initialized HueColorPicker, or `NULL` on error.
	 * @memberof HueColorPicker
	 */
	HueColorPicker *(*initWithFrame)(HueColorPicker *self, const SDL_Rect *frame);

	/**
	 * @fn SDL_Color HueColorPicker::rgbColor(const HueColorPicker *self);
	 * @param self The HueColorPicker.
	 * @return The RGB color.
	 * @memberof HueColorPicker
	 */
	SDL_Color (*rgbColor)(const HueColorPicker *self);

	/**
	 * @fn void HueColorPicker::setColor(HueColorPicker *self, double hue, double saturation, double value)
	 * @brief Sets the color of the HueColorPicker.
	 * @param self The HueColorPicker.
	 * @param hue The hue.
	 * @param saturation The saturation.
	 * @param value The brightness value.
	 * @memberof HueColorPicker
	 */
	void (*setColor)(HueColorPicker *self, double hue, double saturation, double value);

	/**
	 * @fn void HueColorPicker::setRGBColor(HueColorPicker *self, const SDL_Color *color)
	 * @brief Sets the color of the HueColorPicker.
	 * @param self The HueColorPicker.
	 * @param color The RGB color.
	 * @memberof HueColorPicker
	 */
	void (*setRGBColor)(HueColorPicker *self, const SDL_Color *color);
};

/**
 * @fn Class *HueColorPicker::_HueColorPicker(void)
 * @brief The HueColorPicker archetype.
 * @return The HueColorPicker Class.
 * @memberof HueColorPicker
 */
OBJECTIVELYMVC_EXPORT Class *_HueColorPicker(void);
