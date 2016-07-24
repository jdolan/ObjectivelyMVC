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

#include <SDL2/SDL_ttf.h>

#include <Objectively/Array.h>

#if defined(__APPLE__)
#define DEFAULT_FONT_FAMILY "Helvetica Neue"
#elif defined(__MINGW32__)
#define DEFAULT_FONT_FAMILY "Verdana"
#else
#define DEFAULT_FONT_FAMILY "Open Sans"
#endif

/**
 * @file
 * 
 * @brief TrueType fonts.
 */

/**
 * @brief Font categories.
 */
typedef enum {
	FontCategoryDefault,
	FontCategoryPrimaryLabel,
	FontCategorySecondaryLabel,
	FontCategoryPrimaryControl,
	FontCategorySecondaryControl,
} FontCategory;

typedef struct Font Font;
typedef struct FontInterface FontInterface;

/**
 * @brief The Font type.
 *
 * @extends Object
 */
struct Font {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	FontInterface *interface;
	
	/**
	 * @brief The backing TTF_Font.
	 */
	TTF_Font *font;
	
	/**
	 * @brief The TrueType font name, according to Fontconfig.
	 */
	char *name;
};

/**
 * @brief The Font interface.
 */
struct FontInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @static
	 *
	 * @fn Array *Font::allFonts(void)
	 *
	 * @return An Array of all available Font names.
	 *
	 * @memberof Font
	 */
	Array *(*allFonts)(void);

	/**
	 * @static
	 *
	 * @fn Font *Font::defaultFont(FontCategory category)
	 *
	 * @param category The FontCategory.
	 *
	 * @return The default Font for the given category.
	 *
	 * @memberof Font
	 */
	Font *(*defaultFont)(FontCategory category);

	/**
	 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int ptsize, int style)
	 *
	 * @brief Initializes this Font with the given attributes.
	 *
	 * @param family The font family.
	 * @param ptsize The point size.
	 * @param style The style (e.g. `TTF_STYLE_BOLD`).
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	Font *(*initWithAttributes)(Font *self, const char *family, int ptsize, int style);

	/**
	 * @fn Font *Font::initWithName(Font *self, const char *name)
	 *
	 * @brief Initializes this Font with the given Fontconfig name.
	 *
	 * @param name The Fontconfig name.
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	Font *(*initWithName)(Font *self, const char *name);

	/**
	 * @fn Font *Font::initWithPattern(Font *self, FcPattern *pattern)
	 *
	 * @brief Initializes this Font with the given Fontconfig pattern.
	 *
	 * @param name The Fontconfig pattern.
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 *
	 * @private
	 */
	Font *(*initWithPattern)(Font *self, ident pattern);
	
	/**
	 * @fn void Font::renderCharacters(const Font *self, const char *text, SDL_Color color)
	 *
	 * @brief Renders the given characters in this Font.
	 *
	 * @param chars The null-terminated UTF-8 encoded C string to render.
	 * @param color The color.
	 *
	 * @return The rendered surface, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	SDL_Surface *(*renderCharacters)(const Font *self, const char *chars, SDL_Color color);
	
	/**
	 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
	 *
	 * @param chars The null-terminated UTF-8 encoded C string to size.
	 * @param w The width to return.
	 * @param h The height to return.
	 *
	 * @return The size of the rendered characters in pixels.
	 *
	 * @memberof Font
	 */
	void (*sizeCharacters)(const Font *self, const char *chars, int *w, int *h);
};

/**
 * @brief The Font Class.
 */
extern Class _Font;
