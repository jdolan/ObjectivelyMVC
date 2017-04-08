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

#include <ObjectivelyMVC/Slider.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief RGBA color selector.
 */

typedef struct ColorSelectDelegate ColorSelectDelegate;

typedef struct ColorSelect ColorSelect;
typedef struct ColorSelectInterface ColorSelectInterface;

/**
 * @brief The ColorSelect delegate protocol.
 */
struct ColorSelectDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Delegate callback for ColorSelect color modification.
	 * @param colorSelect The ColorSelect.
	 */
	void (*didSetColor)(ColorSelect *colorSelect);
};
/**
 * @brief RGBA color selector.
 * @extends StackView
 * @ingroup Containers
 * @remarks ColorSelects are for selecting an RGB color and optionally alpha
 */
struct ColorSelect {

	/**
	 * @brief The superclass.
	 */
	StackView stackView;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ColorSelectInterface *interface;

	/**
	 * @brief The delegate.
	 */
	ColorSelectDelegate delegate;

	/**
	 * @brief The red slider.
	 */
	Slider *sliderR;

	/**
	 * @brief The green slider.
	 */
	Slider *sliderG;

	/**
	 * @brief The blue slider.
	 */
	Slider *sliderB;

	/**
	 * @brief The alpha slider.
	 */
	Slider *sliderA;

	/**
	 * @brief The color preview.
	 */
	View *colorView;

	/**
	 * @brief If the alpha selector is enabled.
	 */
	_Bool useAlpha;

	/**
	 * @brief The color.
	 */
	SDL_Color color;
};

/**
 * @brief The ColorSelect interface.
 */
struct ColorSelectInterface {

	/**
	 * @brief The superclass interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn ColorSelect *ColorSelect::initWithFrame(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha)
	 * @brief Initializes this ColorSelect with the specified frame.
	 * @param self The ColorSelect.
	 * @param frame The frame.
	 * @param useAlpha If the alpha slider is shown.
	 * @return The initialized ColorSelect, or `NULL` on error.
	 * @memberof ColorSelect
	 */
	ColorSelect *(*initWithFrame)(ColorSelect *self, const SDL_Rect *frame, _Bool useAlpha);

	/**
	 * @fn void ColorSelect::setColor(ColorSelect *self, const SDL_Color color)
	 * @brief Sets the current color of the ColorSelect
	 * @param self The ColorSelect.
	 * @param color The color to change to
	 * @memberof ColorSelect
	 */
	void (*setColor)(ColorSelect *self, const SDL_Color color);
};

/**
 * @fn Class *ColorSelect::_ColorSelect(void)
 * @brief The ColorSelect archetype.
 * @return The ColorSelect Class.
 * @memberof ColorSelect
 */
OBJECTIVELYMVC_EXPORT Class *_ColorSelect(void);
