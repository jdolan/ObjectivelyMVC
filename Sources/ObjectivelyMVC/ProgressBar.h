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

#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/Label.h>

/**
 * @file
 * @brief A Progress bar.
 */

typedef struct ProgressBarDelegate ProgressBarDelegate;

typedef struct ProgressBar ProgressBar;
typedef struct ProgressBarInterface ProgressBarInterface;

/**
 * @brief The ProgressBar delegate protocol.
 */
struct ProgressBarDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Delegate callback for ProgressBar value modification.
	 * @param progressBar The ProgressBar.
	 * @param value The value.
	 */
	void (*didSetValue)(ProgressBar *progressBar, double value);
};

/**
 * @brief The ProgressBar type.
 * @extends View
 */
struct ProgressBar {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ProgressBarInterface *interface;

	/**
	 * @brief The background ImageView.
	 * @remarks Set a background color if you do not wish to provide an ImageView.
	 */
	ImageView *background;

	/**
	 * @brief The delegate.
	 */
	ProgressBarDelegate delegate;

	/**
	 * @brief The foreground ImageView.
	 * @remarks Set a background color if you do not wish to provide an ImageView.
	 */
	ImageView *foreground;

	/**
	 * @brief The progress Text.
	 */
	Text *label;

	/**
	 * @brief The Label format, e.g. `"%0.0lf"`.
	 */
	char *labelFormat;

	/**
	 * @brief The progress bounds.
	 */
	double min, max;

	/**
	 * @brief The progress value.
	 */
	double value;
};

/**
 * @brief The ProgressBar interface.
 */
struct ProgressBarInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn void ProgressBar::formatLabel(ProgressBar *self)
	 * @brief Forces an update on this ProgressBar's label.
	 * @param self The ProgressBar.
	 * @memberof ProgressBar
	 */
	void (*formatLabel)(ProgressBar *self);

	/**
	 * @fn ProgressBar *ProgressBar::init(ProgressBar *self, const SDL_Rect *frame)
	 * @brief Initializes this ProgressBar with the specified frame.
	 * @param self The ProgressBar.
	 * @param frame The frame.
	 * @return The initialized ProgressBar, or `NULL` on error.
	 * @memberof ProgressBar
	 */
	ProgressBar *(*initWithFrame)(ProgressBar *self, const SDL_Rect *frame);

	/**
	 * @fn int ProgressBar::progress(const ProgressBar *self)
	 * @param self The ProgressBar.
	 * @return The progress as a percentage value.
	 * @memberof ProgressBar
	 */
	double (*progress)(const ProgressBar *self);

	/**
	 * @fn void ProgressBar::setLabelFormat(ProgressBar *self, const char *labelFormat)
	 * @brief Changes this ProgressBar's label format and calls appropriate update functions
	 * @param self The ProgressBar.
	 * @param labelFormat The new label format.
	 * @memberof ProgressBar
	 */
	void (*setLabelFormat)(ProgressBar *self, const char *labelFormat);

	/**
	 * @fn void ProgressBar::setValue(const ProgressBar *self, double value)
	 * @brief Sets the value, which is clamped to `min` and `max`.
	 * @param self The ProgressBar.
	 * @param value The value.
	 * @memberof ProgressBar
	 */
	void (*setValue)(ProgressBar *self, double value);
};

/**
 * @fn Class *ProgressBar::_ProgressBar(void)
 * @brief The ProgressBar archetype.
 * @return The ProgressBar Class.
 * @memberof ProgressBar
 */
OBJECTIVELYMVC_EXPORT Class *_ProgressBar(void);
