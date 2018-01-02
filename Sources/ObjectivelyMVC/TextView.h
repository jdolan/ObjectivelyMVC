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
 * @brief A Control for presenting and capturing user-provied Text.
 */

typedef struct TextViewDelegate TextViewDelegate;

typedef struct TextView TextView;
typedef struct TextViewInterface TextViewInterface;

/**
 * @brief The TextView delegate protocol.
 */
struct TextViewDelegate {

	/**
	 * @brief The delegate self-reference.
	 */
	ident self;

	/**
	 * @brief Delegate callback for initiating text editing.
	 * @param textView The TextView.
	 */
	void (*didBeginEditing)(TextView *textView);

	/**
	 * @brief Delegate callback for text input events.
	 * @param textView The TextView.
	 */
	void (*didEdit)(TextView *textView);

	/**
	 * @brief Delegate callback for finalizing text editing.
	 * @param textView The TextView.
	 */
	void (*didEndEditing)(TextView *textView);
};

/**
 * @brief A Control for presenting and capturing user-provied Text.
 * @extends Control
 * @ingroup Controls
 */
struct TextView {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
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
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame)
	 * @brief Initializes this TextView with the specified frame and style.
	 * @param self The TextView.
	 * @param frame The frame.
	 * @return The initialized TextView, or `NULL` on error.
	 * @memberof TextView
	 */
	TextView *(*initWithFrame)(TextView *self, const SDL_Rect *frame);
};

/**
 * @fn Class *TextView::_TextView(void)
 * @brief The TextView archetype.
 * @return The TextView Class.
 * @memberof TextView
 */
OBJECTIVELYMVC_EXPORT Class *_TextView(void);
