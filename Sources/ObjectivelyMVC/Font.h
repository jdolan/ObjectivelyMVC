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

#include <ObjectivelyMVC/Font+Bitmap.h>
#include <ObjectivelyMVC/Types.h>

#define DEFAULT_FONT_FAMILY "Coda"
#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FONT_STYLE FontStyleRegular

#define DEFAULT_MONOSPACE_FONT_FAMILY "Share Tech Mono"

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
 * @brief The family, size and style attributes that identify a Font, independent of the pixel
 * density at which it is opened.
 */
typedef struct {

  /**
   * @brief The family, or `NULL` for the default.
   */
  const char *family;

  /**
   * @brief The point size, or `0` for the default.
   */
  int size;

  /**
   * @brief The FontStyle, or `-1` for the default.
   */
  int style;
} FontAttributes;

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
   * @brief The glyphs of a fixed-width Font, baked into one sheet; `bitmap.surface` is `NULL`
   * for a proportional Font.
   * @details Monospaced Fonts are bitmap fonts: Text draws a Font with a bitmap as one quad
   * per glyph from the sheet, with no opt-in.
   */
  FontBitmap bitmap;

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
   * @fn Font *Font::defaultMonospaceFont(void)
   * @return The default monospaced Font, at a pixel density of `1.0`.
   * @remarks Bundled so that a bitmap font is always available; this is the fallback for
   * Views that are not yet attached to a window, mirroring Font::defaultFont.
   * @memberof Font
   */
  Font *(*defaultMonospaceFont)(void);

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
   * @fn Font *Font::fontWithAttributes(const FontAttributes *attributes, float pixelDensity)
   * @brief Resolves a new Font with the given attributes from the cached font Data.
   * @details "Cached" refers to the font Data registered via Font::cacheFont; the returned
   * instance itself is newly created. Instance caching is provided by Theme::font, which
   * callers with a Theme SHOULD prefer, since it supplies the pixel density implicitly and
   * avoids re-opening the same Font repeatedly. Attributes are normalized to their
   * defaults exactly as Font::nameWithAttributes normalizes them.
   * @param attributes The FontAttributes.
   * @param pixelDensity The pixel density.
   * @return The Font, or the default Font if `attributes->family` is not registered. The
   * caller owns the returned Font, and MUST release it.
   * @memberof Font
   */
  Font *(*fontWithAttributes)(const FontAttributes *attributes, float pixelDensity);

  /**
   * @fn Font *Font::initWithData(Font *self, Data *data, const FontAttributes *attributes, float pixelDensity)
   * @brief Initializes this Font with the given TTF Data and attributes.
   * @details Fonts are immutable: the backing TTF_Font is opened here at
   * `size * pixelDensity` and never re-opened. A pixel density change MUST be handled by
   * resolving a new instance (see Theme::font).
   * @param self The Font.
   * @param data The Data.
   * @param attributes The FontAttributes.
   * @param pixelDensity The pixel density.
   * @return The initialized Font, or `NULL` on error.
   * @memberof Font
   */
  Font *(*initWithData)(Font *self, Data *data, const FontAttributes *attributes, float pixelDensity);

  /**
   * @fn String *Font::name(const Font *self)
   * @return This Font's name, in the format `family-size-style`, e.g. `"Coda-16-regular"`.
   * @remarks The format is stable and MAY be parsed by Font::fontWithName; it is also the
   * key under which Theme caches Font instances. Pixel density is deliberately
   * absent: it is implied by the window whose cache is consulted.
   * @memberof Font
   */
  String *(*name)(const Font *self);

  /**
   * @static
   * @fn String *Font::nameWithAttributes(const FontAttributes *attributes)
   * @return The Font name for the given attributes, in the format of Font::name.
   * @memberof Font
   */
  String *(*nameWithAttributes)(const FontAttributes *attributes);

  /**
   * @fn void Font::renderBitmapCharacters(Font *self, const Renderer *renderer, const char *chars, SDL_Color color, int wrapWidth, const SDL_Point *origin)
   * @brief Records one quad per glyph of the given characters from this Font's bitmap.
   * @param self The Font, which MUST have a bitmap.
   * @param renderer The Renderer.
   * @param chars The null-terminated UTF-8 encoded C string to render.
   * @param color The color; `^0` through `^9` in `chars` select from `TextEscapeColors`, and
   * `^^` is a literal `^`.
   * @param wrapWidth The maximum line width, in logical pixels, or `0` for no wrapping.
   * @param origin The top-left corner of the first line, in logical pixels.
   * @memberof Font
   */
  void (*renderBitmapCharacters)(Font *self, const Renderer *renderer, const char *chars, SDL_Color color, int wrapWidth, const SDL_Point *origin);

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
   * @fn void Font::renderDeviceWillReset(Font *self)
   * @brief Releases this Font's bitmap Texture, if any, ahead of a render device reset. It is
   * recreated on the next draw.
   * @param self The Font.
   * @memberof Font
   */
  void (*renderDeviceWillReset)(Font *self);

  /**
   * @fn void Font::sizeBitmapCharacters(const Font *self, const char *chars, int wrapWidth, int *w, int *h)
   * @brief Measures the given characters against this Font's bitmap.
   * @param self The Font, which MUST have a bitmap.
   * @param chars The null-terminated UTF-8 encoded C string to size.
   * @param wrapWidth The maximum line width, in logical pixels, or `0` for no wrapping.
   * @param w The width to return, in logical pixels.
   * @param h The height to return, in logical pixels.
   * @memberof Font
   */
  void (*sizeBitmapCharacters)(const Font *self, const char *chars, int wrapWidth, int *w, int *h);

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
