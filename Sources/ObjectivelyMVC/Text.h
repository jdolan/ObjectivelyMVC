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

#include <SDL3/SDL_gpu.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/View.h>

typedef struct BitmapFont BitmapFont;

/**
 * @file
 * @brief Text rendered with TrueType fonts.
 */

/**
 * @brief Color palette for text escape sequences (^0-^7).
 *
 * Defines the colors used when rendering Text with @c colorEscapes enabled.
 * Embedding applications can customize this array at runtime before rendering.
 *
 * Default mapping:
 * - ^0 = Black
 * - ^1 = Red
 * - ^2 = Green
 * - ^3 = Yellow
 * - ^4 = Blue
 * - ^5 = Magenta
 * - ^6 = Cyan
 * - ^7 = White
 *
 * @warning Do not modify the array size (must remain exactly 8 elements).
 * @warning Modifications should be made early in initialization, before rendering text.
 *
 * Example customization:
 * @code
 * TextEscapeColors[8] = (SDL_Color) { 0xFF, 0x80, 0x00, 0xFF };  // ^8 = Orange
 * @endcode
 */
OBJECTIVELYMVC_EXPORT SDL_Color TextEscapeColors[10];

/**
 * @brief Strips color escape sequences (`^0` through `^9`) from the given text.
 * @param text The text.
 * @return A newly allocated copy of `text` without escape sequences. The caller must free it.
 */
OBJECTIVELYMVC_EXPORT char *MVC_StripColorEscapes(const char *text);

typedef struct Text Text;
typedef struct TextInterface TextInterface;

/**
 * @brief Text rendered with TrueType fonts.
 * @extends View
 */
struct Text {

  /**
   * @brief The superclass.
   */
  View view;

  /**
   * @brief The interface type.
   * @protected
   */
  TextInterface *interface[0];

  /**
   * @brief The BitmapFont, or `NULL` to render with `font`.
   * @details When set, text is drawn as one quad per glyph from the BitmapFont's atlas, and
   * `font` is not used for rasterization or measurement; `color` and color escapes apply per
   * vertex, for free.
   * @remarks Do not set this property directly.
   * @see Text::setBitmapFont(Text *, BitmapFont *)
   */
  BitmapFont *bitmapFont;

  /**
   * @brief The text color.
   */
  SDL_Color color;

  /**
   * @brief The Font.
   * @remarks Do not set this property directly.
   * @see Text::setFont(Text *, Font *)
   */
  Font *font;

  /**
   * @brief If true, render text with color escape sequence support (^0-^7).
   * @remarks Enables per-character colorization via Font::renderCharactersWithColors.
   *   Only enable for Text that uses color escape sequences, as the code path is
   *   significantly more expensive than plain rendering.
   * @see FontEscapeColors
   */
  bool colorEscapes;

  /**
   * @brief If true, wrap text along word boundaries to fit this Text's width.
   */
  bool lineWrap;

  /**
   * @brief The cached Text::naturalSize, valid while `isValid` is set and `pixelDensity`
   * and `colorEscapes` match the Font's pixel density and this Text's `colorEscapes` --
   * the latter because it is a public, setter-less field that changes the measurement path.
   * @private
   */
  struct {
    SDL_Size size;
    float pixelDensity;
    bool colorEscapes;
    bool isValid;
  } naturalSizeCache;

  /**
   * @brief The text.
   * @remarks Do not set this property directly.
   * @see Text::setText(Text *, const char *)
   */
  char *text;

  /**
   * @brief The rendered GPU texture.
   * @protected
   */
  Texture *texture;

  /**
   * @brief The logical draw dimensions of the texture (surface size / pixel density).
   * @remarks Stored when the texture is created; used for pixel-perfect rendering.
   * @protected
   */
  SDL_Size textureSize;
};

/**
 * @brief The Text interface.
 */
struct TextInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn Text *Text::initWithText(Text *self, const char *text, Font *font)
   * @brief Initializes this Text with the given text and Font.
   * @param self The Text.
   * @param text The text.
   * @param font The Font (optional).
   * @return The initialized Text, or `NULL` on error.
   * @memberof Text
   */
  Text *(*initWithText)(Text *self, const char *text, Font *font);

  /**
   * @fn SDL_Size Text::naturalSize(const Text *self)
   * @brief Resolves the rendered size of this Text.
   * @param self The Text.
   * @memberof Text
   */
  SDL_Size (*naturalSize)(const Text *self);

  /**
   * @fn void Text::setBitmapFont(Text *self, BitmapFont *bitmapFont)
   * @brief Sets this Text's BitmapFont.
   * @details A BitmapFont is bound to the pixel density of the Font it was baked from. On a
   * density change the owner MUST set a new one, since Text cannot re-bake it the way it
   * re-resolves a Font.
   * @param self The Text.
   * @param bitmapFont The BitmapFont, or `NULL` to render with `font`.
   * @memberof Text
   */
  void (*setBitmapFont)(Text *self, BitmapFont *bitmapFont);

  /**
   * @fn void Text::setFont(Text *self, Font *font)
   * @brief Sets this Text's font.
   * @param self The Text.
   * @param font The Font to set.
   * @memberof Text
   */
  void (*setFont)(Text *self, Font *font);

  /**
   * @fn void Text::setText(Text *self, const char *text)
   * @brief Sets this Text's text.
   * @param self The Text.
   * @param text The text to set.
   * @memberof Text
   */
  void (*setText)(Text *self, const char *text);

  /**
   * @fn void Text::setTextWithFormat(Text *self, const char *fmt, ...)
   * @brief Sets this Text's text with the given format string.
   * @param self The Text.
   * @param fmt The format string.
   * @memberof Text
   */
  void (*setTextWithFormat)(Text *self, const char *fmt, ...);
};

OBJECTIVELYMVC_EXPORT Class *_Text(void);
