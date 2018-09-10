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

#include <SDL_ttf.h>

#include <Objectively/Enum.h>
#include <Objectively/Array.h>
#include <Objectively/Data.h>

#include <ObjectivelyMVC/Types.h>

#define DEFAULT_FONT_FAMILY "Coda"
#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FONT_STYLE FontStyleRegular

/**
 * @file
 * @brief TrueType fonts.
 */

/**
 * @brief Font styles.
 */
typedef enum {
	FontStyleRegular = TTF_STYLE_NORMAL,
	FontStyleBold = TTF_STYLE_BOLD,
	FontStyleItalic = TTF_STYLE_ITALIC,
	FontStyleUnderline = TTF_STYLE_UNDERLINE,
	FontStyleStrikeThrough = TTF_STYLE_STRIKETHROUGH
} FontStyle;

OBJECTIVELYMVC_EXPORT const EnumName FontStyleNames[];

typedef struct Font Font;
typedef struct FontInterface FontInterface;

/**
 * @brief TrueType fonts.
 * @extends Object
 */
struct Font {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	FontInterface *interface;

	/**
	 * @brief The raw font data.
	 */
	Data *data;

	/**
	 * @brief The family name.
	 */
	char *family;

	/**
	 * @brief The backing font.
	 */
	ident font;

	/**
	 * @brief The render size, adjusted for display density.
	 */
	int renderSize;

	/**
	 * @brief The point size.
	 */
	int size;

	/**
	 * @brief The style.
	 */
	int style;
};

/**
 * @brief The Font interface.
 */
struct FontInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Font *Font::cachedFont(const char *family, int size, int style)
	 * @brief Resolves the cached Font with the given attributes.
	 * @param family The family.
	 * @param size The size.
	 * @param style The style.
	 * @return The cached Font, or the default Font if not found.
	 * @memberof Font
	 */
	Font *(*cachedFont)(const char *family, int size, int style);

	/**
	 * @static
	 * @brief Caches the specified font Data.
	 * @param data The TTF Data.
	 * @param family The family.
	 * @memberof Font
	 */
	void (*cacheFont)(Data *data, const char *family);

	/**
	 * @static
	 * @fn void Font::clearCache(void)
	 * @brief Clears the Font cache.
	 * @memberof Font
	 */
	void (*clearCache)(void);

	/**
	 * @static
	 * @fn Font *Font::defaultFont(void)
	 * @return The default Font.
	 * @memberof Font
	 */
	Font *(*defaultFont)(void);

	/**
	 * @fn Font *Font::initWithData(Font *self, Data *data, const char *family, int size, int style)
	 * @brief Initializes this Font with the given TTF Data and attributes.
	 * @param self The Font.
	 * @param data The Data.
	 * @param family The family.
	 * @param size The size.
	 * @param style The style.
	 * @return The initialized Font, or `NULL` on error.
	 * @memberof Font
	 */
	Font *(*initWithData)(Font *self, Data *data, const char *family, int size, int style);

	/**
	 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color)
	 * @brief Renders the given characters in this Font.
	 * @param self The Font.
	 * @param chars The null-terminated UTF-8 encoded C string to render.
	 * @param color The color.
	 * @return The rendered surface, or `NULL` on error.
	 * @memberof Font
	 */
	SDL_Surface *(*renderCharacters)(const Font *self, const char *chars, SDL_Color color);

	/**
	 * @fn void Font::renderDeviceDidReset(Font *self)
	 * @brief This method should be invoked when the render context is invalidated.
	 * @param self The Font.
	 * @memberof Font
	 */
	void (*renderDeviceDidReset)(Font *self);

	/**
	 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
	 * @param self The Font.
	 * @param chars The null-terminated UTF-8 encoded C string to size.
	 * @param w The width to return.
	 * @param h The height to return.
	 * @return The size of the rendered characters in pixels.
	 * @memberof Font
	 */
	void (*sizeCharacters)(const Font *self, const char *chars, int *w, int *h);
};

/**
 * @fn Class *Font::_Font(void)
 * @brief The Font archetype.
 * @return The Font Class.
 * @memberof Font
 */
OBJECTIVELYMVC_EXPORT Class *_Font(void);
