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
#include <ObjectivelyMVC/ImageAtlas.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief Text rendered with TrueType fonts.
 */

/**
 * @brief Color palette for text escape sequences (`^0` through `^9`).
 * @details Escapes are always interpreted, on every Text: `^N` selects `TextEscapeColors[N]`
 * for the characters that follow, and `^^` renders a literal `^`. Embedding applications MAY
 * customize this array at runtime, before rendering.
 *
 * Default mapping: ^0 Black, ^1 Red, ^2 Green, ^3 Yellow, ^4 Blue, ^5 Magenta, ^6 Cyan,
 * ^7 White, ^8 Orange, ^9 Grey.
 */
OBJECTIVELYMVC_EXPORT SDL_Color TextEscapeColors[10];

/**
 * @param text The text, or `NULL`.
 * @return True if `text` contains a color escape (`^0` through `^9`, or `^^`).
 */
OBJECTIVELYMVC_EXPORT bool MVC_HasColorEscapes(const char *text);

/**
 * @brief Strips color escape sequences from the given text: `^0` through `^9` are removed, and
 * `^^` collapses to `^`.
 * @param text The text.
 * @return A newly allocated copy of `text` without escape sequences. The caller must free it.
 */
OBJECTIVELYMVC_EXPORT char *MVC_StripColorEscapes(const char *text);

/**
 * @brief Parses an icon escape at the start of `chars`: `:name:`, where `name` is one or more of
 * `[A-Za-z0-9_-]` and under 64 bytes, and is registered in `icons`. An unregistered name is not
 * an escape, so `12:30:45` stays literal unless someone registers an icon called `30`.
 * @param chars The text, positioned at a candidate `:`.
 * @param icons The icon ImageAtlas, or `NULL` for none.
 * @param image If not `NULL`, receives the icon, or `NULL`.
 * @return The byte length of the escape, or `0` if `chars` does not start with one.
 */
OBJECTIVELYMVC_EXPORT size_t MVC_IconEscapeLength(const char *chars, const ImageAtlas *icons, AtlasImage **image);

/**
 * @brief One span of a laid-out string, in bytes of the layout string MVC_LayoutText returns:
 * either the characters drawn in `color`, or a placeholder standing in for `icon`.
 */
typedef struct {
  int offset;
  int length;
  SDL_Color color;
  AtlasImage *icon;
} TextSpan;

/**
 * @brief Resolves the escapes in `text` into the string to lay out and the spans to draw it as.
 * @details `^N` selects a color for what follows, `^^` becomes `^`, and a registered `:icon:`
 * becomes a run of placeholder characters at least a line height wide, for the icon to be drawn
 * over. The placeholder is never a line-break opportunity, so an icon stays with its word. With
 * no `icons`, and `spans` `NULL`, the result is that of MVC_StripColorEscapes.
 * @param font The Font the text will be laid out in.
 * @param text The text.
 * @param color The color in effect at the start of `text`.
 * @param icons The icon ImageAtlas, or `NULL` for none.
 * @param spans If not `NULL`, receives the spans, which the caller MUST free.
 * @param count If not `NULL`, receives the number of spans; meaningful only with `spans`.
 * @return The layout string, which the caller MUST free.
 */
OBJECTIVELYMVC_EXPORT char *MVC_LayoutText(const Font *font, const char *text, SDL_Color color, const ImageAtlas *icons, TextSpan **spans, size_t *count);

/**
 * @brief One quad of a rendered TrueType Text: a region of its texture, in texels, drawn in
 * `color`, or, when `icon` is set, that icon drawn as a square of the line height within it.
 */
typedef struct {
  SDL_Rect src;
  SDL_Color color;
  AtlasImage *icon;
} TextRun;

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
   * @brief The text color.
   */
  SDL_Color color;

  /**
   * @brief The Font. A fixed-width Font carries a bitmap, and is drawn as one quad per glyph
   * from its glyph sheet instead of being rasterized per string; `color` and color escapes then
   * apply per vertex, for free.
   * @remarks Do not set this property directly.
   * @see Text::setFont(Text *, Font *)
   */
  Font *font;

  /**
   * @brief If true, wrap text along word boundaries to fit this Text's width.
   */
  bool lineWrap;

  /**
   * @brief The cached Text::naturalSize, valid while `isValid` is set and `pixelDensity`
   * matches the Font's pixel density.
   * @private
   */
  struct {
    SDL_Size size;
    float pixelDensity;
    bool isValid;
  } naturalSizeCache;

  /**
   * @brief The icon ImageAtlas this Text's `runs` and cached size were resolved against, and its
   * generation then, so that icons registered later, or a Theme swap, re-resolve them.
   * @private
   */
  struct {
    const ImageAtlas *atlas;
    unsigned generation;
  } icons;

  /**
   * @brief The runs of `texture`, one textured quad each, when the text contains escapes;
   * otherwise `NULL`, and `texture` is drawn as a single quad in `color`.
   * @protected
   */
  TextRun *runs;

  /**
   * @brief The number of entries in `runs`.
   * @protected
   */
  size_t runCount;

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
   * @fn SDL_Size Text::sizeText(const Text *self, const char *text)
   * @brief Measures the given text as this Text would render it: in its Font, with color escapes
   * removed and registered icons from its window's Theme in place.
   * @param self The Text.
   * @param text The text.
   * @return The size, in logical pixels.
   * @memberof Text
   */
  SDL_Size (*sizeText)(const Text *self, const char *text);

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
