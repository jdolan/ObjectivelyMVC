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
#include <ObjectivelyMVC/Slider.h>
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 * @brief RGB(A) color picker.
 */

typedef struct RGBColorPickerDelegate RGBColorPickerDelegate;

typedef struct RGBColorPicker RGBColorPicker;
typedef struct RGBColorPickerInterface RGBColorPickerInterface;

/**
 * @brief The RGBColorPicker delegate protocol.
 */
struct RGBColorPickerDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called by the RGBColorPicker when color selection changes.
	 * @param rgbColorPicker The RGBColorPicker.
	 * @param color The Color.
	 */
	void (*didPickColor)(RGBColorPicker *rgbColorPicker, SDL_Color *color);
};

/**
 * @brief RGB(A) color picker.
 * @extends Control
 * @ingroup Controls
 */
struct RGBColorPicker {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	RGBColorPickerInterface *interface;

	/**
	 * @brief The delegate.
	 */
	RGBColorPickerDelegate delegate;

	/**
	 * @brief The color.
	 */
	SDL_Color color;

	/**
	 * @brief The color preview.
	 */
	View *colorView;

	/**
	 * @brief The Sliders.
	 */
	Slider *redSlider, *greenSlider, *blueSlider, *alphaSlider;

	/**
	 * @brief The Inputs containing the Sliders.
	 */
	Input *redInput, *greenInput, *blueInput, *alphaInput;

	/**
	 * @brief The StackView.
	 */
	StackView *stackView;
};

/**
 * @brief The RGBColorPicker interface.
 */
struct RGBColorPickerInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn RGBColorPicker *RGBColorPicker::initWithFrame(RGBColorPicker *self, const SDL_Rect *frame)
	 * @brief Initializes this RGBColorPicker with the specified frame and style.
	 * @param self The RGBColorPicker.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized RGBColorPicker, or `NULL` on error.
	 * @memberof RGBColorPicker
	 */
	RGBColorPicker *(*initWithFrame)(RGBColorPicker *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn void RGBColorPicker::setColor(RGBColorPicker *self, const SDL_Color color)
	 * @brief Sets the color of the RGBColorPicker
	 * @param self The RGBColorPicker.
	 * @param color The color to change to
	 * @memberof RGBColorPicker
	 */
	void (*setColor)(RGBColorPicker *self, const SDL_Color color);
};

/**
 * @fn Class *RGBColorPicker::_RGBColorPicker(void)
 * @brief The RGBColorPicker archetype.
 * @return The RGBColorPicker Class.
 * @memberof RGBColorPicker
 */
OBJECTIVELYMVC_EXPORT Class *_RGBColorPicker(void);
