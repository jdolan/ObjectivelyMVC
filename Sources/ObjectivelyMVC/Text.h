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

#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/Font.h>

/**
 * @file
 *
 * @brief Texts rendered with TrueType fonts.
 */

typedef struct Text Text;
typedef struct TextInterface TextInterface;

/**
 * @brief The Text type.
 *
 * @extends View
 */
struct Text {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	View view;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	TextInterface *interface;
	
	/**
	 * @brief The text color.
	 */
	SDL_Color color;

	/**
	 * @brief The Font.
	 *
	 * @remarks Do not set this property directly.
	 *
	 * @see Text::setFont(Text *, Font *)
	 */
	Font *font;

	/**
	 * @brief The text.
	 *
	 * @remarks Do not set this property directly.
	 *
	 * @see Text::setText(Text *, const char *)
	 */
	char *text;

	/**
	 * @brief The rendered texture.
	 *
	 * @private
	 */
	SDL_Texture *texture;
};

/**
 * @brief The Text interface.
 */
struct TextInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn Text *Text::initWithText(Text *self, const char *text, Font *font)
	 *
	 * @brief Initializes this Text with the given text and Font.
	 *
	 * @param text The text.
	 * @param font The Font.
	 *
	 * @return The initialized Text, or `NULL` on error.
	 *
	 * @memberof Text
	 */
	Text *(*initWithText)(Text *self, const char *text, Font *font);
	
	/**
	 * @fn void Text::naturalSize(const Text *self, int *width, int *height)
	 *
	 * @brief Resolves the rendered size of this Text.
	 *
	 * @param width The rendered width.
	 * @param height The rendered height.
	 *
	 * @memberof Text
	 */
	void (*naturalSize)(const Text *self, int *width, int *height);

	/**
	 * @fn void Text::setFont(Text *self, Font *font)
	 *
	 * @brief Sets this Text's font.
	 *
	 * @param font The Font to set.
	 *
	 * @memberof Text
	 */
	void (*setFont)(Text *self, Font *font);
	
	/**
	 * @fn void Text::setText(Text *self, const char *text)
	 *
	 * @brief Sets this Text's text.
	 *
	 * @param text The text to set.
	 *
	 * @memberof Text
	 */
	void (*setText)(Text *self, const char *text);
};

extern Class _Text;
