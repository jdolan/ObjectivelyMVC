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

#include <Objectively/Number.h>
#include <Objectively/String.h>

#include "BitmapFont.h"
#include "Colors.h"
#include "Log.h"
#include "Text.h"

#define _Class _BitmapFont

#define REPLACEMENT_CHARACTER 0xFFFD

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  BitmapFont *this = (BitmapFont *) self;

  release(this->named);
  release(this->cells);
  release(this->atlas);
  release(this->font);

  super(Object, self, dealloc);
}

#pragma mark - Baking

/**
 * @brief The top-left texel of the given cell within the grid surface.
 */
static SDL_Point cellOrigin(const BitmapFont *self, Uint32 cell) {
  return MakePoint((cell % self->columns) * self->cellSize.w, (cell / self->columns) * self->cellSize.h);
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
static void bakeGlyph(BitmapFont *self, SDL_Surface *sheet, Uint32 codepoint, Uint32 cell) {

  TTF_Font *font = self->font->font;

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
    const SDL_Point origin = cellOrigin(self, cell);
    const SDL_Rect rect = MakeRect(origin.x - self->bearing + min(minX, 0), origin.y, glyph->w, glyph->h);

    // A glyph surface can exceed its cell, and must not overwrite its neighbours
    const SDL_Rect clip = MakeRect(origin.x, origin.y, self->cellSize.w, self->cellSize.h);
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
  TokenGlyph,
  TokenImage
} TokenType;

typedef struct {
  TokenType type;
  Uint32 codepoint;
  Uint32 cell;
  SDL_Color color;
} Token;

/**
 * @brief Resolves `codepoint` to a cell, logging the first sighting of each unbaked one.
 */
static Uint32 cellForCodepoint(BitmapFont *self, Uint32 codepoint) {

  if (codepoint >= self->first && codepoint < self->first + self->count) {
    return codepoint - self->first;
  }

  bool logged = false;
  for (size_t i = 0; i < self->loggedCount; i++) {
    if (self->logged[i] == codepoint) {
      logged = true;
      break;
    }
  }

  if (!logged && self->loggedCount < lengthof(self->logged)) {
    self->logged[self->loggedCount++] = codepoint;

    if (self->loggedCount < lengthof(self->logged)) {
      MVC_LogWarn("U+%04X is outside the baked range U+%04X-U+%04X\n", codepoint, self->first, self->first + self->count - 1);
    } else {
      MVC_LogWarn("U+%04X and further codepoints outside the baked range U+%04X-U+%04X will not be reported\n", codepoint, self->first, self->first + self->count - 1);
    }
  }

  return self->count;
}

/**
 * @brief Advances `*chars` past the next token, describing it in `token`.
 */
static void nextToken(BitmapFont *self, const char **chars, bool colorEscapes, Token *token) {

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

  if (colorEscapes && p[0] == '^' && p[1] >= '0' && p[1] <= '9') {
    token->type = TokenColor;
    token->color = TextEscapeColors[p[1] - '0'];
    *chars = p + 2;
    return;
  }

  if (self->named && *p == ':') {
    const char *q = p + 1;
    while (SDL_isalnum((unsigned char) *q) || *q == '_' || *q == '-') {
      q++;
    }

    char name[64];
    const size_t len = q - p - 1;

    if (*q == ':' && len > 0 && len < sizeof(name)) {
      memcpy(name, p + 1, len);
      name[len] = '\0';

      const Number *cell = $(self->named, objectForKeyPath, name);
      if (cell) {
        token->type = TokenImage;
        token->cell = (Uint32) cell->value;
        *chars = q + 1;
        return;
      }
    }
  }

  const Uint32 codepoint = SDL_StepUTF8(&p, NULL);

  token->type = TokenGlyph;
  token->codepoint = codepoint;
  token->cell = cellForCodepoint(self, codepoint);
  *chars = p;
}

/**
 * @brief The pen advance of the given token, in texels.
 */
static int tokenAdvance(const BitmapFont *self, const Token *token) {

  switch (token->type) {
    case TokenGlyph:
      return self->advance;
    case TokenImage:
      return self->advance * self->span;
    default:
      return 0;
  }
}

/**
 * @brief The advance of the word starting at `chars`, up to the next space, newline or end.
 */
static int wordAdvance(BitmapFont *self, const char *chars, bool colorEscapes) {

  int advance = 0;

  while (true) {
    const char *next = chars;

    Token token;
    nextToken(self, &next, colorEscapes, &token);

    if (token.type == TokenEnd || token.type == TokenNewline) {
      break;
    }

    if (token.type == TokenGlyph && token.codepoint == ' ') {
      break;
    }

    advance += tokenAdvance(self, &token);
    chars = next;
  }

  return advance;
}

/**
 * @brief Receives each drawable token with the pen position of its cell, in texels.
 */
typedef void (*TokenVisitor)(BitmapFont *self, const Token *token, int x, int y, ident data);

/**
 * @brief Walks `chars`, wrapping at word boundaries when `wrapWidth` (in texels) is non-zero,
 * and reports the extent of the text in texels.
 */
static void walk(BitmapFont *self, const char *chars, bool colorEscapes, int wrapWidth, SDL_Color color,
                 TokenVisitor visitor, ident data, int *w, int *h) {

  int x = 0, y = 0, maxX = 0;
  bool lineHasContent = false;

  const int overhang = max(0, self->bearing + self->cellSize.w - self->advance);

  Token token = { .type = TokenEnd, .color = color };

  while (true) {
    nextToken(self, &chars, colorEscapes, &token);

    if (token.type == TokenEnd) {
      break;
    }

    if (token.type == TokenNewline) {
      x = 0;
      y += self->cellSize.h;
      lineHasContent = false;
      continue;
    }

    if (token.type == TokenColor) {
      color = token.color;
      continue;
    }

    if (wrapWidth && token.type == TokenGlyph && token.codepoint == ' ' && x > 0) {
      if (x + self->advance + wordAdvance(self, chars, colorEscapes) > wrapWidth) {
        x = 0;
        y += self->cellSize.h;
        lineHasContent = false;
        continue;
      }
    }

    const int advance = tokenAdvance(self, &token);

    if (wrapWidth && x > 0 && x + advance > wrapWidth) {
      x = 0;
      y += self->cellSize.h;
      lineHasContent = false;
    }

    if (visitor) {
      token.color = token.type == TokenImage ? Colors.White : color;
      visitor(self, &token, x, y, data);
    }

    x += advance;
    lineHasContent = true;
    maxX = max(maxX, x + overhang);
  }

  if (w) {
    *w = maxX;
  }
  if (h) {
    *h = y + (lineHasContent ? self->cellSize.h : 0);
  }
}

#pragma mark - BitmapFont

/**
 * @fn BitmapFont *BitmapFont::initWithFont(BitmapFont *self, Font *font, Uint32 first, Uint32 count, const Dictionary *named, ImageAtlas *atlas)
 * @memberof BitmapFont
 */
static BitmapFont *initWithFont(BitmapFont *self, Font *font, Uint32 first, Uint32 count, const Dictionary *named, ImageAtlas *atlas) {

  assert(font);
  assert(count);
  assert(atlas);

  if (!TTF_FontIsFixedWidth(font->font)) {
    String *name = $(font, name);
    MVC_LogWarn("%s is not fixed-width\n", name->chars);
    release(name);
    return release(self);
  }

  self = (BitmapFont *) super(Object, self, init);
  if (self) {

    self->font = retain(font);
    self->atlas = retain(atlas);
    self->first = first;
    self->count = count;

    int minX = INT_MAX, maxX = INT_MIN;

    for (Uint32 i = 0; i <= count; i++) {
      const Uint32 codepoint = i < count ? first + i : REPLACEMENT_CHARACTER;

      if (TTF_FontHasGlyph(font->font, codepoint)) {
        int glyphMinX = 0, glyphMaxX = 0, advance = 0;
        if (!TTF_GetGlyphMetrics(font->font, codepoint, &glyphMinX, &glyphMaxX, NULL, NULL, &advance)) {
          continue;
        }

        minX = min(minX, glyphMinX);
        maxX = max(maxX, max(glyphMaxX, advance));

        if (i < count) {
          self->advance = max(self->advance, advance);
        }
      }
    }

    if (maxX <= minX || self->advance == 0) {
      String *name = $(font, name);
      MVC_LogWarn("%s has no glyphs in U+%04X-U+%04X\n", name->chars, first, first + count - 1);
      release(name);
      return release(self);
    }

    self->bearing = minX;
    self->cellSize = MakeSize(maxX - minX, TTF_GetFontHeight(font->font));
    self->span = max(1, self->cellSize.h / self->cellSize.w);

    Array *names = named ? $(named, allKeys) : NULL;
    const size_t namedCount = names ? names->count : 0;

    const Uint32 glyphCells = count + 1;
    const Uint32 totalCells = glyphCells + (Uint32) (namedCount * self->span);

    self->columns = max(self->span, (int) ceilf(sqrtf((float) totalCells)));

    Uint32 cell = glyphCells;

    if (namedCount) {
      self->named = $$(Dictionary, dictionary);

      for (size_t i = 0; i < namedCount; i++) {
        if ((int) (cell % self->columns) + self->span > self->columns) {
          cell += self->columns - cell % self->columns;
        }

        const String *name = $(names, objectAtIndex, i);

        Number *number = $$(Number, numberWithValue, cell);
        $(self->named, setObjectForKeyPath, number, name->chars);
        release(number);

        cell += self->span;
      }
    }

    const int rows = (int) ((cell + self->columns - 1) / self->columns);

    SDL_Surface *sheet = SDL_CreateSurface(self->columns * self->cellSize.w, rows * self->cellSize.h, SDL_PIXELFORMAT_RGBA32);
    assert(sheet);

    SDL_FillSurfaceRect(sheet, NULL, 0);

    for (Uint32 i = 0; i < count; i++) {
      bakeGlyph(self, sheet, first + i, i);
    }

    bakeGlyph(self, sheet, TTF_FontHasGlyph(font->font, REPLACEMENT_CHARACTER) ? REPLACEMENT_CHARACTER : '?', count);

    for (size_t i = 0; i < namedCount; i++) {
      const String *name = $(names, objectAtIndex, i);

      const Number *number = $(self->named, objectForKeyPath, name->chars);
      const Image *image = $(named, objectForKeyPath, name->chars);
      assert(image);

      const SDL_Point origin = cellOrigin(self, (Uint32) number->value);
      const SDL_Rect rect = MakeRect(origin.x, origin.y, self->cellSize.w * self->span, self->cellSize.h);

      blit(image->surface, sheet, &rect);
    }

    release(names);

    Image *grid = $$(Image, imageWithSurface, sheet);
    assert(grid);

    self->cells = retain($(atlas, addImage, grid));

    release(grid);
    SDL_DestroySurface(sheet);
  }

  return self;
}

typedef struct {
  const Renderer *renderer;
  Texture *texture;
  SDL_FPoint origin;
  float scale;
} RenderContext;

/**
 * @brief TokenVisitor for renderCharacters.
 */
static void renderToken(BitmapFont *self, const Token *token, int x, int y, ident data) {

  const RenderContext *context = data;

  const int span = token->type == TokenImage ? self->span : 1;

  const SDL_Point origin = cellOrigin(self, token->cell);
  const SDL_Rect src = MakeRect(self->cells->rect.x + origin.x, self->cells->rect.y + origin.y, self->cellSize.w * span, self->cellSize.h);

  // Positioned in whole texels so that the sheet is sampled 1:1, then scaled to logical
  const SDL_FRect dest = {
    (context->origin.x + x + self->bearing) / context->scale,
    (context->origin.y + y) / context->scale,
    src.w / context->scale,
    src.h / context->scale
  };

  $(context->renderer, drawTextureRegion, context->texture, &src, &dest, &token->color);
}

/**
 * @fn void BitmapFont::renderCharacters(BitmapFont *self, const Renderer *renderer, const char *chars, SDL_Color color, bool colorEscapes, int wrapWidth, const SDL_Point *origin)
 * @memberof BitmapFont
 */
static void renderCharacters(BitmapFont *self, const Renderer *renderer, const char *chars, SDL_Color color, bool colorEscapes, int wrapWidth, const SDL_Point *origin) {

  assert(renderer);
  assert(origin);

  if (chars == NULL || self->cells->atlas == NULL) {
    return;
  }

  RenderContext context = {
    .renderer = renderer,
    .texture = $(self->atlas, texture, renderer->device),
    .origin = { roundf(origin->x * self->font->pixelDensity), roundf(origin->y * self->font->pixelDensity) },
    .scale = self->font->pixelDensity,
  };

  if (context.texture == NULL) {
    return;
  }

  walk(self, chars, colorEscapes, (int) (wrapWidth * context.scale), color, renderToken, &context, NULL, NULL);
}

/**
 * @fn void BitmapFont::sizeCharacters(BitmapFont *self, const char *chars, bool colorEscapes, int wrapWidth, int *w, int *h)
 * @memberof BitmapFont
 */
static void sizeCharacters(BitmapFont *self, const char *chars, bool colorEscapes, int wrapWidth, int *w, int *h) {

  int texelsW = 0, texelsH = 0;

  if (chars) {
    walk(self, chars, colorEscapes, (int) (wrapWidth * self->font->pixelDensity), Colors.White, NULL, NULL, &texelsW, &texelsH);
  }

  if (w) {
    *w = (int) ceilf(texelsW / self->font->pixelDensity);
  }
  if (h) {
    *h = (int) ceilf(texelsH / self->font->pixelDensity);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((BitmapFontInterface *) clazz->interface)->initWithFont = initWithFont;
  ((BitmapFontInterface *) clazz->interface)->renderCharacters = renderCharacters;
  ((BitmapFontInterface *) clazz->interface)->sizeCharacters = sizeCharacters;
}

/**
 * @fn Class *BitmapFont::_BitmapFont(void)
 * @memberof BitmapFont
 */
Class *_BitmapFont(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "BitmapFont",
      .superclass = _Object(),
      .instanceSize = sizeof(BitmapFont),
      .interfaceSize = sizeof(BitmapFontInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
