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

#include <Objectively/MutableString.h>

#include <ObjectivelyMVC/Control.h>

/**
 * @file
 * 
 * @brief TextViews provide editable text field Controls.
 */

#define DEFAULT_TEXTVIEW_PADDING 10
#define DEFAULT_TEXTVIEW_WIDTH 150

typedef struct TextViewDelegate TextViewDelegate;

typedef struct TextView TextView;
typedef struct TextViewInterface TextViewInterface;

/**
 * @brief The TextView delegate protocol.
 */
struct TextViewDelegate {

	/**
	 * @brief Delegate callback for initiating text editing.
	 */
	void (*didBeginEditing)(TextView *textView);
	
	/**
	 * @brief Delegate callback for text input events.
	 */
	void (*didEdit)(TextView *textView);
	
	/**
	 * @brief Delegate callback for finalizing text editing.
	 */
	void (*didEndEditing)(TextView *textView);
};

/**
 * @brief The TextView type.
 * @extends Control
 * @ingroup Controls
 */
struct TextView {

	/**
	 * @brief The parent.
	 * @private
	 */
	Control control;

	/**
	 * @brief The typed interface.
	 * @private
	 */
	TextViewInterface *interface;
	
	/**
	 * @brief The user-provided text.
	 */
	MutableString *attributedText;

	/**
	 * @brief The default text, displayed when no user-provided text is available.
	 */
	char *defaultText;
	
	/**
	 * @brief The delegate.
	 */
	TextViewDelegate delegate;
	
	/**
	 * @brief True if this TextView supports editing, false otherwise.
	 */
	_Bool isEditable;
	
	/**
	 * The editing position.
	 */
	size_t position;
	
	/**
	 * @brief The text.
	 */
	Text *text;
};

/**
 * @brief The TextView interface.
 */
struct TextViewInterface {

	/**
	 * @brief The parent interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this TextView with the specified frame.
	 * @param self The TextView.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The initialized TextView, or `NULL` on error.
	 * @memberof TextView
	 */
	TextView *(*initWithFrame)(TextView *self, const SDL_Rect *frame, ControlStyle style);
};

/**
 * @brief The TextView Class.
 */
extern Class _TextView;
