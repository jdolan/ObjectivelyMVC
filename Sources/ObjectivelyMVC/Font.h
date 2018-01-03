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

#include <Objectively/Enum.h>
#include <Objectively/Array.h>
#include <Objectively/Data.h>

#include <ObjectivelyMVC/Types.h>

#if defined(__APPLE__)
#define DEFAULT_FONT_FAMILY "Helvetica Neue"
#elif defined(_WIN32)
#define DEFAULT_FONT_FAMILY "Verdana"
#else
#define DEFAULT_FONT_FAMILY "DejaVu Sans"
#endif

#define DEFAULT_FONT_SIZE 16

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

/**
 * @brief Font categories.
 */
typedef enum {
	FontCategoryDefault,
	FontCategoryPrimaryLabel,
	FontCategorySecondaryLabel,
	FontCategoryPrimaryControl,
	FontCategorySecondaryControl,
	FontCategoryPrimaryResponder,
	FontCategorySecondaryResponder,
	FontCategoryUser,
	FontCategoryMax = 16
} FontCategory;

OBJECTIVELYMVC_EXPORT const EnumName FontCategoryNames[];

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
	 * @brief The backing font.
	 */
	ident font;

	/**
	 * @brief The index in the backing font file.
	 */
	int index;

	/**
	 * @brief The render size, which is dependent on the display resolution.
	 */
	int renderSize;

	/**
	 * @brief The point size.
	 */
	int size;
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
	 * @fn Array *Font::allFonts(void)
	 * @return An Array of all Font names known to Fontconfig.
	 * @memberof Font
	 */
	Array *(*allFonts)(void);

	/**
	 * @static
	 * @fn Font *Font::defaultFont(FontCategory category)
	 * @param category The FontCategory.
	 * @return The default Font for the given category.
	 * @memberof Font
	 */
	Font *(*defaultFont)(FontCategory category);

	/**
	 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int size, int style)
	 * @brief Initializes this Font with the given attributes via Fontconfig.
	 * @param self The Font.
	 * @param family The font family.
	 * @param size The point size.
	 * @param style The style.
	 * @return The initialized Font, or `NULL` on error.
	 * @memberof Font
	 */
	Font *(*initWithAttributes)(Font *self, const char *family, int size, int style);

	/**
	 * @fn Font *Font::initWithData(Font *self, Data *data, int size, int index)
	 * @brief Initializes this Font with the given TrueType font file.
	 * @param self The Font.
	 * @param data The Data containing the TrueType font file.
	 * @param size The point size.
	 * @param index The index of the desired font face within the font file.
	 * @return The initialized Font, or `NULL` on error.
	 * @memberof Font
	 */
	Font *(*initWithData)(Font *self, Data *data, int size, int index);

	/**
	 * @fn Font *Font::initWithName(Font *self, const char *name)
	 * @brief Initializes this Font with the given Fontconfig name.
	 * @param self The Font.
	 * @param name The Fontconfig name.
	 * @return The initialized Font, or `NULL` on error.
	 * @memberof Font
	 */
	Font *(*initWithName)(Font *self, const char *name);

	/**
	 * @fn Font *Font::initWithPattern(Font *self, FcPattern *pattern)
	 * @brief Initializes this Font with the given Fontconfig pattern.
	 * @param self The Font.
	 * @param name The Fontconfig pattern.
	 * @return The initialized Font, or `NULL` on error.
	 * @memberof Font
	 * @private
	 */
	Font *(*initWithPattern)(Font *self, ident pattern);

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
	 * @static
	 * @fn void Font::setDefaultFont(FontCategory category, Font *font)
	 * @brief Sets the default Font for the given category.
	 * @param category The FontCategory.
	 * @param font The Font.
	 * @memberof Font
	 */
	void (*setDefaultFont)(FontCategory category, Font *font);

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
