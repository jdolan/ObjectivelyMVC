/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

#include <SDL3_ttf/SDL_ttf.h>

#include <Objectively/Enum.h>
#include <Objectively/Array.h>
#include <Objectively/Data.h>
#include <Objectively/String.h>

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
   * @brief The interface type.
   * @protected
   */
  FontInterface *interface[0];

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
  TTF_Font *font;

  /**
   * @brief The display pixel density scale, greater than 1.0 on high-density displays.
   */
  float pixelDensity;

  /**
   * @brief The render size, adjusted for pixel density.
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
   * @return The default Font, at a pixel density of `1.0`.
   * @remarks This is the fallback for Views that are not yet attached to a window; on
   * attachment, style application re-resolves Fonts through the window's cache.
   * @memberof Font
   */
  Font *(*defaultFont)(void);

  /**
   * @static
   * @fn Font *Font::fontWithName(const char *name, float pixelDensity)
   * @brief Resolves a Font from a name in the format produced by Font::name.
   * @details The name is parsed from the right, so families MAY contain `-`: the final
   * token is the style, the preceding token the point size, and the remainder the family.
   * @param name The name, e.g. `"Coda-16-regular"`.
   * @param pixelDensity The pixel density.
   * @return The Font, or the default Font if `name` is not parseable or not registered.
   * The caller owns the returned Font, and MUST release it.
   * @memberof Font
   */
  Font *(*fontWithName)(const char *name, float pixelDensity);

  /**
   * @static
   * @fn Font *Font::fontWithAttributes(const char *family, int size, int style, float pixelDensity)
   * @brief Resolves a new Font with the given attributes from the cached font Data.
   * @details "Cached" refers to the font Data registered via Font::cacheFont; the returned
   * instance itself is newly created. Instance caching is provided per-window by
   * WindowController::font, which callers with a window SHOULD prefer, since it
   * supplies the window's pixel density implicitly. Attributes are normalized to their
   * defaults exactly as Font::nameWithAttributes normalizes them.
   * @param family The family, or `NULL` for the default.
   * @param size The point size, or `0` for the default.
   * @param style The FontStyle, or `-1` for the default.
   * @param pixelDensity The pixel density.
   * @return The Font, or the default Font if `family` is not registered. The caller owns
   * the returned Font, and MUST release it.
   * @memberof Font
   */
  Font *(*fontWithAttributes)(const char *family, int size, int style, float pixelDensity);

  /**
   * @fn Font *Font::initWithData(Font *self, Data *data, const char *family, int size, int style, float pixelDensity)
   * @brief Initializes this Font with the given TTF Data and attributes.
   * @details Fonts are immutable: the backing TTF_Font is opened here at
   * `size * pixelDensity` and never re-opened. A pixel density change MUST be handled by
   * resolving a new instance (see WindowController::font).
   * @param self The Font.
   * @param data The Data.
   * @param family The family.
   * @param size The point size.
   * @param style The style.
   * @param pixelDensity The pixel density.
   * @return The initialized Font, or `NULL` on error.
   * @memberof Font
   */
  Font *(*initWithData)(Font *self, Data *data, const char *family, int size, int style, float pixelDensity);

  /**
   * @fn String *Font::name(const Font *self)
   * @return This Font's name, in the format `family-size-style`, e.g. `"Coda-16-regular"`.
   * @remarks The format is stable and MAY be parsed by Font::fontWithName; it is also the
   * key under which WindowController caches Font instances. Pixel density is deliberately
   * absent: it is implied by the window whose cache is consulted.
   * @memberof Font
   */
  String *(*name)(const Font *self);

  /**
   * @static
   * @fn String *Font::nameWithAttributes(const char *family, int size, int style)
   * @return The Font name for the given attributes, in the format of Font::name.
   * @memberof Font
   */
  String *(*nameWithAttributes)(const char *family, int size, int style);

  /**
   * @fn SDL_Surface *Font::renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth)
   * @brief Renders the given characters in this Font.
   * @param self The Font.
   * @param chars The null-terminated UTF-8 encoded C string to render.
   * @param color The color.
   * @param wrapWidth The maximum line width, in logical pixels, where wrapping should occur.
   * @return The rendered surface, or `NULL` on error.
   * @memberof Font
   */
  SDL_Surface *(*renderCharacters)(const Font *self, const char *chars, SDL_Color color, int wrapWidth);

  /**
   * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
   * @brief Measures the given characters in this Font.
   * @param self The Font.
   * @param chars The null-terminated UTF-8 encoded C string to size.
   * @param w The width to return.
   * @param h The height to return.
   * @return The size of the rendered characters in logical pixels.
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
