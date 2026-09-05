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

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include <Objectively/String.h>

#include "Colors.h"
#include "Font.h"
#include "Log.h"
#include "Text.h"

#define _Class _Font

#pragma mark - Baking

/**
 * @brief The top-left texel of the given cell within the grid surface.
 */
static SDL_Point cellOrigin(const FontBitmap *bitmap, Uint32 cell) {
  return MakePoint((cell % FONT_BITMAP_COLUMNS) * bitmap->cellSize.w, (cell / FONT_BITMAP_COLUMNS) * bitmap->cellSize.h);
}

/**
 * @brief Copies `src` into `dest` at `rect`, scaling if necessary, without alpha blending, so
 * that glyph coverage lands in the sheet exactly as rasterized.
 */
static void blit(SDL_Surface *src, SDL_Surface *dest, const SDL_Rect *rect) {

  SDL_BlendMode blendMode;
  SDL_GetSurfaceBlendMode(src, &blendMode);
  SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_NONE);

  if (src->w == rect->w && src->h == rect->h) {
    SDL_BlitSurface(src, NULL, dest, rect);
  } else {
    SDL_BlitSurfaceScaled(src, NULL, dest, rect, SDL_SCALEMODE_LINEAR);
  }

  SDL_SetSurfaceBlendMode(src, blendMode);
}

/**
 * @brief Rasterizes `codepoint` into the given cell, with its bearing baked in.
 */
static void bakeGlyph(const FontBitmap *bitmap, TTF_Font *font, Uint32 codepoint, Uint32 cell) {

  SDL_Surface *sheet = bitmap->surface;

  if (!TTF_FontHasGlyph(font, codepoint)) {
    return;
  }

  int minX = 0;
  TTF_GetGlyphMetrics(font, codepoint, &minX, NULL, NULL, NULL, NULL);

  SDL_Surface *glyph = TTF_RenderGlyph_Blended(font, codepoint, Colors.White);
  if (glyph) {

    // The glyph surface's left edge is the pen, with the glyph's own bearing already applied,
    // except that SDL_ttf shifts a glyph with a negative bearing right so that its ink is not
    // cut off. The cell's left edge is the pen plus the range's minimum bearing.
    const SDL_Point origin = cellOrigin(bitmap, cell);
    const SDL_Rect rect = MakeRect(origin.x - bitmap->bearing + min(minX, 0), origin.y, glyph->w, glyph->h);

    // A glyph surface can exceed its cell, and must not overwrite its neighbours
    const SDL_Rect clip = MakeRect(origin.x, origin.y, bitmap->cellSize.w, bitmap->cellSize.h);
    SDL_SetSurfaceClipRect(sheet, &clip);

    blit(glyph, sheet, &rect);

    SDL_SetSurfaceClipRect(sheet, NULL);
    SDL_DestroySurface(glyph);
  } else {
    MVC_LogError("U+%04X: %s\n", codepoint, SDL_GetError());
  }
}

#pragma mark - Walking

typedef enum {
  TokenEnd,
  TokenNewline,
  TokenColor,
  TokenGlyph
} TokenType;

typedef struct {
  TokenType type;
  Uint32 codepoint;
  Uint32 cell;
  SDL_Color color;
} Token;

/**
 * @brief Resolves `codepoint` to a cell: FONT_BITMAP_REPLACEMENT's for one outside the baked range.
 */
static Uint32 cellForCodepoint(Uint32 codepoint) {

  if (codepoint >= FONT_BITMAP_FIRST && codepoint < FONT_BITMAP_LAST) {
    return codepoint - FONT_BITMAP_FIRST;
  }

  return FONT_BITMAP_REPLACEMENT - FONT_BITMAP_FIRST;
}

/**
 * @brief Advances `*chars` past the next token, describing it in `token`.
 */
static void nextToken(const char **chars, Token *token) {

  const char *p = *chars;

  while (*p == '\r') {
    p++;
  }

  if (*p == '\0') {
    token->type = TokenEnd;
    *chars = p;
    return;
  }

  if (*p == '\n') {
    token->type = TokenNewline;
    *chars = p + 1;
    return;
  }

  if (p[0] == '^' && p[1] >= '0' && p[1] <= '9') {
    token->type = TokenColor;
    token->color = TextEscapeColors[p[1] - '0'];
    *chars = p + 2;
    return;
  }

  if (p[0] == '^' && p[1] == '^') {
    token->type = TokenGlyph;
    token->codepoint = '^';
    token->cell = cellForCodepoint('^');
    *chars = p + 2;
    return;
  }

  const Uint32 codepoint = SDL_StepUTF8(&p, NULL);

  token->type = TokenGlyph;
  token->codepoint = codepoint;
  token->cell = cellForCodepoint(codepoint);
  *chars = p;
}

/**
 * @brief The pen advance of the given token, in texels.
 */
static int tokenAdvance(const FontBitmap *bitmap, const Token *token) {
  return token->type == TokenGlyph ? bitmap->advance : 0;
}

/**
 * @brief The advance of the word starting at `chars`, up to the next space, newline or end.
 */
static int wordAdvance(const FontBitmap *bitmap, const char *chars) {

  int advance = 0;

  while (true) {
    const char *next = chars;

    Token token;
    nextToken(&next, &token);

    if (token.type == TokenEnd || token.type == TokenNewline) {
      break;
    }

    if (token.type == TokenGlyph && token.codepoint == ' ') {
      break;
    }

    advance += tokenAdvance(bitmap, &token);
    chars = next;
  }

  return advance;
}

/**
 * @brief Receives each drawable token with the pen position of its cell, in texels.
 */
typedef void (*TokenVisitor)(const FontBitmap *bitmap, const Token *token, int x, int y, ident data);

/**
 * @brief Walks `chars`, wrapping at word boundaries when `wrapWidth` (in texels) is non-zero,
 * and reports the extent of the text in texels.
 */
static void walk(const FontBitmap *bitmap, const char *chars, int wrapWidth, SDL_Color color,
                 TokenVisitor visitor, ident data, int *w, int *h) {

  int x = 0, y = 0, maxX = 0;
  bool lineHasContent = false;

  const int overhang = max(0, bitmap->bearing + bitmap->cellSize.w - bitmap->advance);

  Token token = { .type = TokenEnd, .color = color };

  while (true) {
    nextToken(&chars, &token);

    if (token.type == TokenEnd) {
      break;
    }

    if (token.type == TokenNewline) {
      x = 0;
      y += bitmap->cellSize.h;
      lineHasContent = false;
      continue;
    }

    if (token.type == TokenColor) {
      color = token.color;
      continue;
    }

    if (wrapWidth && token.type == TokenGlyph && token.codepoint == ' ' && x > 0) {
      if (x + bitmap->advance + wordAdvance(bitmap, chars) > wrapWidth) {
        x = 0;
        y += bitmap->cellSize.h;
        lineHasContent = false;
        continue;
      }
    }

    const int advance = tokenAdvance(bitmap, &token);

    if (wrapWidth && x > 0 && x + advance > wrapWidth) {
      x = 0;
      y += bitmap->cellSize.h;
      lineHasContent = false;
    }

    if (visitor) {
      token.color = color;
      visitor(bitmap, &token, x, y, data);
    }

    x += advance;
    lineHasContent = true;
    maxX = max(maxX, x + overhang);
  }

  if (w) {
    *w = maxX;
  }
  if (h) {
    *h = y + (lineHasContent ? bitmap->cellSize.h : 0);
  }
}

#pragma mark - FontBitmap

bool initBitmap(FontBitmap *bitmap, Font *font) {

  assert(bitmap);
  assert(font);

  if (!TTF_FontIsFixedWidth(font->font)) {
    String *name = $(font, name);
    MVC_LogWarn("%s is not fixed-width\n", name->chars);
    release(name);
    return false;
  }

  int minX = INT_MAX, maxX = INT_MIN, advance = 0;

  for (Uint32 codepoint = FONT_BITMAP_FIRST; codepoint < FONT_BITMAP_LAST; codepoint++) {

    if (TTF_FontHasGlyph(font->font, codepoint)) {
      int glyphMinX = 0, glyphMaxX = 0, glyphAdvance = 0;
      if (!TTF_GetGlyphMetrics(font->font, codepoint, &glyphMinX, &glyphMaxX, NULL, NULL, &glyphAdvance)) {
        continue;
      }

      minX = min(minX, glyphMinX);
      maxX = max(maxX, max(glyphMaxX, glyphAdvance));
      advance = max(advance, glyphAdvance);
    }
  }

  if (maxX <= minX || advance == 0) {
    String *name = $(font, name);
    MVC_LogWarn("%s has no glyphs in U+%04X-U+%04X\n", name->chars, FONT_BITMAP_FIRST, FONT_BITMAP_LAST - 1);
    release(name);
    return false;
  }

  bitmap->advance = advance;
  bitmap->bearing = minX;
  bitmap->cellSize = MakeSize(maxX - minX, TTF_GetFontHeight(font->font));

  const int rows = FONT_BITMAP_COUNT / FONT_BITMAP_COLUMNS;

  bitmap->surface = SDL_CreateSurface(FONT_BITMAP_COLUMNS * bitmap->cellSize.w, rows * bitmap->cellSize.h, SDL_PIXELFORMAT_RGBA32);
  assert(bitmap->surface);

  SDL_FillSurfaceRect(bitmap->surface, NULL, 0);

  for (Uint32 i = 0; i < FONT_BITMAP_COUNT; i++) {
    bakeGlyph(bitmap, font->font, FONT_BITMAP_FIRST + i, i);
  }

  return true;
}

void deallocBitmap(FontBitmap *bitmap) {

  assert(bitmap);

  release(bitmap->texture);

  if (bitmap->surface) {
    SDL_DestroySurface(bitmap->surface);
  }
}

/**
 * @fn void Font::renderDeviceWillReset(Font *self)
 * @memberof Font
 */
void renderDeviceWillResetBitmap(Font *self) {

  assert(self);

  self->bitmap.texture = release(self->bitmap.texture);
}

typedef struct {
  const Renderer *renderer;
  Texture *texture;
  SDL_FPoint origin;
  float scale;
} RenderContext;

/**
 * @brief TokenVisitor for Font::renderBitmapCharacters.
 */
static void renderToken(const FontBitmap *bitmap, const Token *token, int x, int y, ident data) {

  const RenderContext *context = data;

  const SDL_Point origin = cellOrigin(bitmap, token->cell);
  const SDL_Rect src = MakeRect(origin.x, origin.y, bitmap->cellSize.w, bitmap->cellSize.h);

  // Positioned in whole texels so that the sheet is sampled 1:1, then scaled to logical
  const SDL_FRect dest = {
    (context->origin.x + x + bitmap->bearing) / context->scale,
    (context->origin.y + y) / context->scale,
    src.w / context->scale,
    src.h / context->scale
  };

  $(context->renderer, drawTextureRegion, context->texture, &src, &dest, &token->color);
}

/**
 * @fn void Font::renderBitmapCharacters(Font *self, const Renderer *renderer, const char *chars, SDL_Color color, int wrapWidth, const SDL_Point *origin)
 * @memberof Font
 */
void renderCharactersBitmap(Font *self, const Renderer *renderer, const char *chars, SDL_Color color, int wrapWidth, const SDL_Point *origin) {

  assert(self);
  assert(renderer);
  assert(origin);

  FontBitmap *bitmap = &self->bitmap;
  assert(bitmap->surface);

  if (chars == NULL) {
    return;
  }

  if (bitmap->texture == NULL) {
    bitmap->texture = $(renderer->device, createTextureFromSurface, bitmap->surface, SDL_GPU_TEXTUREUSAGE_SAMPLER, false);
    assert(bitmap->texture);
  }

  RenderContext context = {
    .renderer = renderer,
    .texture = bitmap->texture,
    .origin = { roundf(origin->x * self->pixelDensity), roundf(origin->y * self->pixelDensity) },
    .scale = self->pixelDensity,
  };

  walk(bitmap, chars, (int) (wrapWidth * context.scale), color, renderToken, &context, NULL, NULL);
}

/**
 * @fn void Font::sizeBitmapCharacters(const Font *self, const char *chars, int wrapWidth, int *w, int *h)
 * @memberof Font
 */
void sizeCharactersBitmap(const Font *self, const char *chars, int wrapWidth, int *w, int *h) {

  assert(self);

  const FontBitmap *bitmap = &self->bitmap;
  assert(bitmap->surface);

  int texelsW = 0, texelsH = 0;

  if (chars) {
    walk(bitmap, chars, (int) (wrapWidth * self->pixelDensity), Colors.White, NULL, NULL, &texelsW, &texelsH);
  }

  if (w) {
    *w = (int) ceilf(texelsW / self->pixelDensity);
  }
  if (h) {
    *h = (int) ceilf(texelsH / self->pixelDensity);
  }
}

#undef _Class
