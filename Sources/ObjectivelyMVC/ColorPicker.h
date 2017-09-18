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
 * @brief RGBA color selector.
 */

typedef struct ColorPickerDelegate ColorPickerDelegate;

typedef struct ColorPicker ColorPicker;
typedef struct ColorPickerInterface ColorPickerInterface;

/**
 * @brief The ColorPicker delegate protocol.
 */
struct ColorPickerDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Called by the ColorPicker when color selection changes.
	 * @param colorPicker The ColorPicker.
	 * @param color The Color.
	 */
	void (*didPickColor)(ColorPicker *colorPicker, SDL_Color *color);
};

/**
 * @brief RGBA color selector.
 * @extends StackView
 * @ingroup Containers
 * @remarks ColorPickers are for selecting an RGB color and optionally alpha
 */
struct ColorPicker {

	/**
	 * @brief The superclass.
	 */
	StackView stackView;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ColorPickerInterface *interface;

	/**
	 * @brief The delegate.
	 */
	ColorPickerDelegate delegate;

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
};

/**
 * @brief The ColorPicker interface.
 */
struct ColorPickerInterface {

	/**
	 * @brief The superclass interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn ColorPicker *ColorPicker::initWithFrame(ColorPicker *self, const SDL_Rect *frame)
	 * @brief Initializes this ColorPicker with the specified frame.
	 * @param self The ColorPicker.
	 * @param frame The frame.
	 * @return The initialized ColorPicker, or `NULL` on error.
	 * @memberof ColorPicker
	 */
	ColorPicker *(*initWithFrame)(ColorPicker *self, const SDL_Rect *frame);

	/**
	 * @fn void ColorPicker::setColor(ColorPicker *self, const SDL_Color color)
	 * @brief Sets the color of the ColorPicker
	 * @param self The ColorPicker.
	 * @param color The color to change to
	 * @memberof ColorPicker
	 */
	void (*setColor)(ColorPicker *self, const SDL_Color color);
};

/**
 * @fn Class *ColorPicker::_ColorPicker(void)
 * @brief The ColorPicker archetype.
 * @return The ColorPicker Class.
 * @memberof ColorPicker
 */
OBJECTIVELYMVC_EXPORT Class *_ColorPicker(void);
