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

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/Array.h>
#include <Objectively/String.h>

#include "Font.h"
#include "Log.h"
#include "View.h"
#include "Window.h"

#include "../Assets/coda.ttf.h"

const EnumName FontStyleNames[] = MakeEnumNames(
  MakeEnumAlias(FontStyleRegular, regular),
  MakeEnumAlias(FontStyleBold, bold),
  MakeEnumAlias(FontStyleItalic, italic),
  MakeEnumAlias(FontStyleUnderline, underline),
  MakeEnumAlias(FontStyleStrikeThrough, strikethrough)
);

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Font *this = (Font *) self;

  free(this->family);

  release(this->data);
  
  TTF_CloseFont(this->font);

  super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  Font *this = (Font *) self;

  int hash = HASH_SEED;
  hash = HashForCString(hash, this->family);
  hash = HashForInteger(hash, this->size);
  hash = HashForInteger(hash, this->style);
  hash = HashForInteger(hash, this->renderSize);

  return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Font())) {

    const Font *this = (Font *) self;
    const Font *that = (Font *) other;

    if (!strcmp(this->family, that->family) &&
        this->size == that->size &&
        this->style == that->style &&
        this->renderSize == that->renderSize) {
      return true;
    }
  }

  return false;
}

#pragma mark - Font

static Dictionary *_cache;

/**
 * @fn void Font::cacheFont(Data *data, const char *family)
 * @memberof Font
 */
static void cacheFont(Data *data, const char *family) {
  $(_cache, setObjectForKeyPath, data, family);
}

/**
 * @brief Normalizes Font attributes to their defaults, in one place, so that
 * Font::nameWithAttributes and Font::fontWithAttributes always agree.
 */
static void normalizeAttributes(const char **family, int *size, int *style) {

  if (*family == NULL) {
    *family = DEFAULT_FONT_FAMILY;
  }

  if (*size < 1) {
    *size = DEFAULT_FONT_SIZE;
  }

  if (*style < FontStyleRegular || *style > FontStyleStrikeThrough) {
    *style = DEFAULT_FONT_STYLE;
  }
}

/**
 * @fn Font *Font::fontWithAttributes(const char *family, int size, int style, float pixelDensity)
 * @memberof Font
 */
static Font *fontWithAttributes(const char *family, int size, int style, float pixelDensity) {

  normalizeAttributes(&family, &size, &style);

  if (pixelDensity < 1.f) {
    pixelDensity = 1.f;
  }

  Data *data = $((Dictionary *) _cache, objectForKeyPath, family);
  if (data) {
    Font *font = $(alloc(Font), initWithData, data, family, size, style, pixelDensity);
    assert(font);

    return font;
  }

  MVC_LogWarn("%s-%d-%d not found\n", family, size, style);
  return retain($$(Font, defaultFont));
}

/**
 * @fn void Font::clearCache(void)
 * @memberof Font
 */
static void clearCache(void) {
  $(_cache, removeAllObjects);
}

static Font *_defaultFont;

/**
 * @fn Font *Font::defaultFont(void)
 * @memberof Font
 */
static Font *defaultFont(void) {
  static Once once;

  do_once(&once, {
    Data *data = $(alloc(Data), initWithConstMemory, coda_ttf, coda_ttf_len - 1);
    assert(data);

    $$(Font, cacheFont, data, DEFAULT_FONT_FAMILY);

    release(data);

    _defaultFont = $$(Font, fontWithAttributes, DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE, 1.f);
    assert(_defaultFont);
  });

  return _defaultFont;
}

/**
 * @fn Font *Font::fontWithName(const char *name, float pixelDensity)
 * @memberof Font
 */
static Font *fontWithName(const char *name, float pixelDensity) {

  assert(name);

  char *chars = strdup(name);
  assert(chars);

  Font *font = NULL;

  char *styleToken = strrchr(chars, '-');
  if (styleToken) {
    *styleToken++ = '\0';

    char *sizeToken = strrchr(chars, '-');
    if (sizeToken) {
      *sizeToken++ = '\0';

      const int style = valueof(FontStyleNames, styleToken);
      const int size = (int) strtol(sizeToken, NULL, 10);

      font = $$(Font, fontWithAttributes, chars, size, style, pixelDensity);
    }
  }

  free(chars);

  if (font == NULL) {
    MVC_LogWarn("%s is not a valid Font name\n", name);
    font = retain($$(Font, defaultFont));
  }

  return font;
}

/**
 * @fn Font *Font::initWithData(Font *self, Data *data, int size, int index)
 * @memberof Font
 */
static Font *initWithData(Font *self, Data *data, const char *family, int size, int style, float pixelDensity) {

  self = (Font *) super(Object, self, init);
  if (self) {

    self->data = retain(data);
    assert(self->data);

    self->family = strdup(family);
    assert(self->family);

    self->size = size;
    assert(self->size);

    self->style = style;

    self->pixelDensity = pixelDensity;
    self->renderSize = self->size * self->pixelDensity;

    SDL_IOStream *buffer = SDL_IOFromConstMem(self->data->bytes, (int) self->data->length);
    assert(buffer);

    self->font = TTF_OpenFontIO(buffer, 1, self->renderSize);
    assert(self->font);

    TTF_SetFontStyle(self->font, self->style);
    TTF_SetFontHinting(self->font, TTF_HINTING_LIGHT_SUBPIXEL);
  }

  return self;
}

/**
 * @fn String *Font::name(const Font *self)
 * @memberof Font
 */
static String *name(const Font *self) {
  return $$(Font, nameWithAttributes, self->family, self->size, self->style);
}

/**
 * @fn String *Font::nameWithAttributes(const char *family, int size, int style)
 * @memberof Font
 */
static String *nameWithAttributes(const char *family, int size, int style) {

  normalizeAttributes(&family, &size, &style);

  for (const EnumName *en = FontStyleNames; en->name; en++) {
    if (en->value == style) {
      return str("%s-%d-%s", family, size, en->alias ?: en->name);
    }
  }

  return str("%s-%d-%d", family, size, style);
}

/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth)
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth) {

  SDL_Surface *surface;
  if (wrapWidth) {
    surface = TTF_RenderText_Blended_Wrapped(self->font, chars, 0, color, wrapWidth * self->pixelDensity);
  } else {
    surface = TTF_RenderText_Blended(self->font, chars, 0, color);
  }

  SDL_Surface *converted = NULL;
  if (surface) {
    converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
  } else {
    MVC_LogError("%s\n", SDL_GetError());
  }

  return converted;
}

/**
 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
 * @memberof Font
 */
static void sizeCharacters(const Font *self, const char *chars, int *w, int *h) {

  if (w) {
    *w = 0;
  }
  if (h) {
    *h = 0;
  }

  if (chars) {
    char *lines = strdup(chars);

    const int font_h = TTF_GetFontHeight(self->font);

    for (char *line = strtok(lines, "\n\r"); line; line = strtok(NULL, "\n\r")) {

      if (w) {

        int lineWidth;
        TTF_GetStringSize(self->font, line, 0, &lineWidth, NULL);

        // TTF_GetStringSize sums glyph advances, but the final glyph's ink can extend past its
        // own advance (common with bold or large faces). Check the final glyph's bounds and
        // ensure we take the larger value.
        const char *end = line + strlen(line);
        const Uint32 ch = SDL_StepBackUTF8(line, &end);
        if (ch) {
          int maxX, advance;
          if (TTF_GetGlyphMetrics(self->font, ch, NULL, &maxX, NULL, NULL, &advance)) {
            if (maxX > advance) {
              lineWidth += maxX - advance;
            }
          }
        }

        *w = max(*w, lineWidth);
      }
      if (h) {
        *h += font_h;
      }
    }
    free(lines);

    if (w) {
      *w = ceilf(*w / self->pixelDensity);
    }
    if (h) {
      *h = ceilf(*h / self->pixelDensity);
    }
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((FontInterface *) clazz->interface)->fontWithAttributes = fontWithAttributes;
  ((FontInterface *) clazz->interface)->cacheFont = cacheFont;
  ((FontInterface *) clazz->interface)->clearCache = clearCache;
  ((FontInterface *) clazz->interface)->defaultFont = defaultFont;
  ((FontInterface *) clazz->interface)->fontWithName = fontWithName;
  ((FontInterface *) clazz->interface)->initWithData = initWithData;
  ((FontInterface *) clazz->interface)->name = name;
  ((FontInterface *) clazz->interface)->nameWithAttributes = nameWithAttributes;
  ((FontInterface *) clazz->interface)->renderCharacters = renderCharacters;
  ((FontInterface *) clazz->interface)->sizeCharacters = sizeCharacters;

  const bool init = TTF_Init();
  assert(init);
  (void) init;

  _cache = $$(Dictionary, dictionary);
  assert(_cache);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

  release(_cache);
  release(_defaultFont);

  TTF_Quit();
}

/**
 * @fn Class *Font::_Font(void)
 * @memberof Font
 */
Class *_Font(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Font",
      .superclass = _Object(),
      .instanceSize = sizeof(Font),
      .interfaceOffset = offsetof(Font, interface),
      .interfaceSize = sizeof(FontInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
