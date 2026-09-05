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

#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Fixed-width Fonts baked into a glyph sheet, drawn as one quad per glyph.
 */

/**
 * @brief The first codepoint Font bakes into its bitmap.
 */
#define FONT_BITMAP_FIRST ' '

/**
 * @brief The codepoint count Font bakes into its bitmap.
 * @details Covers Basic Latin, Latin-1 Supplement, Latin Extended-A/B, and Greek and Coptic
 * (`U+0020-U+041F`) -- generous enough for any Latin- or Greek-script text without configuration.
 * Individual codepoints a Font lacks a glyph for are simply skipped, not rejected, so this is
 * safe to bake against any fixed-width face regardless of its actual coverage.
 */
#define FONT_BITMAP_COUNT 1024

/**
 * @brief One past the last baked codepoint in a Font bitmap (exclusive upper bound).
 */
#define FONT_BITMAP_LAST (FONT_BITMAP_FIRST + FONT_BITMAP_COUNT)
/**
 * @brief The number of cells per row of the glyph sheet: `FONT_BITMAP_COUNT` cells make a
 * square grid of this many columns and rows.
 */
#define FONT_BITMAP_COLUMNS 32

/**
 * @brief The codepoint drawn in place of one outside the baked range.
 */
#define FONT_BITMAP_REPLACEMENT '?'

typedef struct Font Font;

/**
 * @brief A fixed-width Font's glyphs, baked into one sheet, drawn as one quad per glyph.
 * @details A Font rasterizes each string into its own surface and Texture, which costs
 * hundreds of microseconds every time the string changes. A fixed-width Font therefore
 * rasterizes a range of codepoints once, into a uniform grid of glyphs on a single surface, and
 * Text draws it as one quad per glyph from that sheet. Unlike an ImageAtlas, nothing is packed:
 * every glyph's position follows from its codepoint and the glyph size, so the grid is its own
 * lookup table.
 * Text that changes every frame becomes free.
 *
 * Only fixed-width faces are supported, so there are no per-glyph metrics: a codepoint's cell
 * is `codepoint - FONT_BITMAP_FIRST`, and overhang is baked by sizing every cell to the face's maximum
 * ink bounds. This discards kerning and shaping (ligatures, joining, reordering), which is
 * fine for a HUD or console and wrong for proportional prose; proportional Fonts never bake.
 *
 * Cells are physical texels, rasterized at the Font's pixel density; sizes reported through
 * sizeBitmapCharacters are logical, as Font's are. Like the Font that owns it, a
 * FontBitmap is immutable and bound to that Font's pixel density.
 *
 * This is a value embedded in Font, not an Object: `surface == NULL` means the Font is not
 * fixed-width and has no bitmap.
 */
typedef struct {

  /**
   * @brief The pen advance per glyph, in texels.
   */
  int advance;

  /**
   * @brief The horizontal offset from the pen to a cell's left edge, in texels.
   * @details Negative when glyphs lean left of the pen, as a synthetic italic does.
   */
  int bearing;

  /**
   * @brief The size of one cell, in texels.
   */
  SDL_Size cellSize;

  /**
   * @brief The glyph sheet: a grid of FONT_BITMAP_COLUMNS by FONT_BITMAP_COLUMNS cells of
   * `cellSize`, in texels, or `NULL` if the Font is not fixed-width.
   */
  SDL_Surface *surface;

  /**
   * @brief The Texture of `surface`, created on first draw and released on render device reset.
   * @private
   */
  Texture *texture;
} FontBitmap;

/**
 * @brief Bakes FONT_BITMAP_COUNT codepoints of `font`, from FONT_BITMAP_FIRST, into a glyph
 * sheet, initializing `bitmap`.
 * @details A codepoint outside the range draws FONT_BITMAP_REPLACEMENT instead.
 * @param bitmap The FontBitmap, zero-initialized.
 * @param font The Font, which MUST be fixed-width.
 * @return True if the bitmap was baked, false if `font` is not fixed-width or has no glyphs
 * in the range, in which case `bitmap` is left zeroed.
 */
OBJECTIVELYMVC_EXPORT bool initBitmap(FontBitmap *bitmap, Font *font);

/**
 * @brief Releases the resources held by `bitmap`. Safe to call on a zeroed FontBitmap.
 * @param bitmap The FontBitmap.
 */
OBJECTIVELYMVC_EXPORT void deallocBitmap(FontBitmap *bitmap);

/**
 * @brief Implements Font::renderDeviceWillReset.
 */
OBJECTIVELYMVC_EXPORT void renderDeviceWillResetBitmap(Font *self);

/**
 * @brief Implements Font::renderBitmapCharacters.
 */
OBJECTIVELYMVC_EXPORT void renderCharactersBitmap(Font *self, const Renderer *renderer, const char *chars, SDL_Color color, int wrapWidth, const SDL_Point *origin);

/**
 * @brief Implements Font::sizeBitmapCharacters.
 */
OBJECTIVELYMVC_EXPORT void sizeCharactersBitmap(const Font *self, const char *chars, int wrapWidth, int *w, int *h);
