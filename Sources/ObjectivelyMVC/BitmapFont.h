/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 */

#pragma once

#include <Objectively/Dictionary.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/ImageAtlas.h>
#include <ObjectivelyMVC/Renderer.h>

/**
 * @file
 * @brief Fixed-width fonts baked into an ImageAtlas, drawn as one quad per glyph.
 */

typedef struct BitmapFont BitmapFont;
typedef struct BitmapFontInterface BitmapFontInterface;

/**
 * @brief Fixed-width fonts baked into an ImageAtlas, drawn as one quad per glyph.
 * @details A Font rasterizes each string into its own surface and Texture, which costs
 * hundreds of microseconds every time the string changes. A BitmapFont instead rasterizes a
 * range of codepoints once, into a uniform grid of cells that is added to an ImageAtlas as a
 * single AtlasImage, and draws text as one quad per glyph from that sheet. Text that changes
 * every frame becomes free, and text that shares an atlas with the images around it draws in
 * the same call.
 *
 * Only fixed-width faces are supported, so there are no per-glyph metrics: a codepoint's cell
 * is `codepoint - first`, and overhang is baked by sizing every cell to the face's maximum
 * ink bounds. A BitmapFont therefore discards kerning and shaping (ligatures, joining,
 * reordering), which is fine for a HUD or console and wrong for proportional prose; it is
 * opt-in per Text for that reason.
 *
 * Named inline images (`:name:` in the text) are baked as extra cells past the codepoint
 * range, resized to a square of `span` cells, and drawn white regardless of the text color.
 *
 * Cells are physical texels, rasterized at the Font's pixel density; sizes reported through
 * BitmapFont::sizeCharacters are logical, as Font's are. A BitmapFont is bound to its Font's
 * pixel density: when that changes, the owner MUST bake a new one from a Font at the new
 * density, just as WindowController re-resolves Fonts.
 * @extends Object
 */
struct BitmapFont {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  BitmapFontInterface *interface[0];

  /**
   * @brief The pen advance per cell, in texels.
   */
  int advance;

  /**
   * @brief The ImageAtlas holding the cells.
   */
  ImageAtlas *atlas;

  /**
   * @brief The horizontal offset from the pen to a cell's left edge, in texels.
   * @details Negative when glyphs lean left of the pen, as a synthetic italic does.
   */
  int bearing;

  /**
   * @brief The grid of cells within the atlas.
   */
  AtlasImage *cells;

  /**
   * @brief The size of one cell, in texels.
   */
  SDL_Size cellSize;

  /**
   * @brief The number of cells per grid row.
   */
  int columns;

  /**
   * @brief The number of codepoints baked, starting at `first`.
   */
  Uint32 count;

  /**
   * @brief The first baked codepoint.
   */
  Uint32 first;

  /**
   * @brief The source Font.
   */
  Font *font;

  /**
   * @brief Codepoints outside the baked range already reported, so each is logged once; when
   * full, further ones go unreported.
   * @private
   */
  Uint32 logged[32];

  /**
   * @brief The number of entries in `logged`.
   * @private
   */
  size_t loggedCount;

  /**
   * @brief Inline image name to its first cell index, as a Number, or `NULL`.
   */
  Dictionary *named;

  /**
   * @brief The number of cells an inline image spans, `cellSize.h / cellSize.w` and at least
   * one, so that a square image stays square in non-square cells.
   */
  int span;
};

/**
 * @brief The BitmapFont interface.
 */
struct BitmapFontInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn BitmapFont *BitmapFont::initWithFont(BitmapFont *self, Font *font, Uint32 first, Uint32 count, const Dictionary *named, ImageAtlas *atlas)
   * @brief Initializes this BitmapFont, baking the given codepoints and images into `atlas`.
   * @details One extra cell past the range holds the replacement glyph (U+FFFD, or `?` if the
   * face lacks it), drawn for any codepoint outside the range, which is also logged once.
   * The grid is added to `atlas` but not compiled: the caller MUST call ImageAtlas::compile
   * after adding everything that will share the sheet, and before drawing.
   * @param self The BitmapFont.
   * @param font The Font, which MUST be fixed-width.
   * @param first The first codepoint to bake.
   * @param count The number of codepoints to bake.
   * @param named Inline image names to `Image`s, or `NULL`. Every image is resized to a square
   * of `span` cells, so they SHOULD be square to begin with.
   * @param atlas The ImageAtlas to bake into.
   * @return The initialized BitmapFont, or `NULL` if `font` is not fixed-width.
   * @memberof BitmapFont
   */
  BitmapFont *(*initWithFont)(BitmapFont *self, Font *font, Uint32 first, Uint32 count, const Dictionary *named, ImageAtlas *atlas);

  /**
   * @fn void BitmapFont::renderCharacters(BitmapFont *self, const Renderer *renderer, const char *chars, SDL_Color color, bool colorEscapes, int wrapWidth, const SDL_Point *origin)
   * @brief Records one quad per glyph of the given characters.
   * @param self The BitmapFont.
   * @param renderer The Renderer.
   * @param chars The null-terminated UTF-8 encoded C string to render.
   * @param color The color.
   * @param colorEscapes If true, `^0` through `^9` select from `TextEscapeColors`.
   * @param wrapWidth The maximum line width, in logical pixels, or `0` for no wrapping.
   * @param origin The top-left corner of the first line, in logical pixels.
   * @memberof BitmapFont
   */
  void (*renderCharacters)(BitmapFont *self, const Renderer *renderer, const char *chars, SDL_Color color, bool colorEscapes, int wrapWidth, const SDL_Point *origin);

  /**
   * @fn void BitmapFont::sizeCharacters(BitmapFont *self, const char *chars, bool colorEscapes, int wrapWidth, int *w, int *h)
   * @brief Measures the given characters.
   * @param self The BitmapFont.
   * @param chars The null-terminated UTF-8 encoded C string to size.
   * @param colorEscapes If true, `^0` through `^9` are escapes rather than characters.
   * @param wrapWidth The maximum line width, in logical pixels, or `0` for no wrapping.
   * @param w The width to return, in logical pixels.
   * @param h The height to return, in logical pixels.
   * @memberof BitmapFont
   */
  void (*sizeCharacters)(BitmapFont *self, const char *chars, bool colorEscapes, int wrapWidth, int *w, int *h);
};

OBJECTIVELYMVC_EXPORT Class *_BitmapFont(void);
