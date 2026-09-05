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
#include "../Assets/shareTechMono.ttf.h"

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

  deallocBitmap(&this->bitmap);

  TTF_CloseFont(this->font);

  free(this->family);

  release(this->data);  

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
static void normalizeAttributes(FontAttributes *attributes) {

  if (attributes->family == NULL) {
    attributes->family = DEFAULT_FONT_FAMILY;
  }

  if (attributes->size < 1) {
    attributes->size = DEFAULT_FONT_SIZE;
  }

  if (attributes->style < FontStyleRegular || attributes->style > FontStyleStrikeThrough) {
    attributes->style = DEFAULT_FONT_STYLE;
  }
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
  
  const FontAttributes attrs = {
    DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE
  };

  do_once(&once, {
    _defaultFont = $$(Font, fontWithAttributes, &attrs, 1.f);
    assert(_defaultFont);
  });

  return _defaultFont;
}

static Font *_defaultMonospaceFont;

/**
 * @fn Font *Font::defaultMonospaceFont(void)
 * @memberof Font
 */
static Font *defaultMonospaceFont(void) {
  static Once once;
  
  const FontAttributes attrs = {
    DEFAULT_MONOSPACE_FONT_FAMILY, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE
  };

  do_once(&once, {
    _defaultMonospaceFont = $$(Font, fontWithAttributes, &attrs, 1.f);
    assert(_defaultMonospaceFont);
  });

  return _defaultMonospaceFont;
}

/**
 * @fn Font *Font::fontWithAttributes(const FontAttributes *attributes, float pixelDensity)
 * @memberof Font
 */
static Font *fontWithAttributes(const FontAttributes *attributes, float pixelDensity) {

  FontAttributes normalized = *attributes;
  normalizeAttributes(&normalized);

  if (pixelDensity < 1.f) {
    pixelDensity = 1.f;
  }

  Data *data = $((Dictionary *) _cache, objectForKeyPath, normalized.family);
  if (data) {
    Font *font = $(alloc(Font), initWithData, data, &normalized, pixelDensity);
    assert(font);

    return font;
  }

  MVC_LogWarn("%s-%d-%d not found\n", normalized.family, normalized.size, normalized.style);
  return retain($$(Font, defaultFont));
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

      const FontAttributes attributes = {
        chars, (int) strtol(sizeToken, NULL, 10), valueof(FontStyleNames, styleToken)
      };

      font = $$(Font, fontWithAttributes, &attributes, pixelDensity);
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
 * @fn Font *Font::initWithData(Font *self, Data *data, const FontAttributes *attributes, float pixelDensity)
 * @memberof Font
 */
static Font *initWithData(Font *self, Data *data, const FontAttributes *attributes, float pixelDensity) {

  self = (Font *) super(Object, self, init);
  if (self) {

    self->data = retain(data);
    assert(self->data);

    self->family = strdup(attributes->family);
    assert(self->family);

    self->size = attributes->size;
    assert(self->size);

    self->style = attributes->style;

    self->pixelDensity = pixelDensity;
    self->renderSize = self->size * self->pixelDensity;

    SDL_IOStream *buffer = SDL_IOFromConstMem(self->data->bytes, (int) self->data->length);
    assert(buffer);

    self->font = TTF_OpenFontIO(buffer, 1, self->renderSize);
    assert(self->font);

    TTF_SetFontStyle(self->font, self->style);
    TTF_SetFontHinting(self->font, TTF_HINTING_LIGHT_SUBPIXEL);

    if (TTF_FontIsFixedWidth(self->font)) {
      initBitmap(&self->bitmap, self);
    }
  }

  return self;
}

/**
 * @fn String *Font::name(const Font *self)
 * @memberof Font
 */
static String *name(const Font *self) {
  const FontAttributes attributes = { self->family, self->size, self->style };
  return $$(Font, nameWithAttributes, &attributes);
}

/**
 * @fn String *Font::nameWithAttributes(const FontAttributes *attributes)
 * @memberof Font
 */
static String *nameWithAttributes(const FontAttributes *attributes) {

  FontAttributes normalized = *attributes;
  normalizeAttributes(&normalized);

  for (const EnumName *en = FontStyleNames; en->name; en++) {
    if (en->value == normalized.style) {
      return str("%s-%d-%s", normalized.family, normalized.size, en->alias ?: en->name);
    }
  }

  return str("%s-%d-%d", normalized.family, normalized.size, normalized.style);
}

/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth)
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth) {

  // Always the wrapping renderer: with a width of 0 it breaks on newlines only, which is what
  // Font::sizeCharacters and the TTF_Text layout Text builds its color runs from both assume,
  // whereas TTF_RenderText_Blended draws a newline as a missing glyph on a single line
  SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(self->font, chars, 0, color, wrapWidth * self->pixelDensity);

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

  ((FontInterface *) clazz->interface)->cacheFont = cacheFont;
  ((FontInterface *) clazz->interface)->clearCache = clearCache;
  ((FontInterface *) clazz->interface)->defaultFont = defaultFont;
  ((FontInterface *) clazz->interface)->defaultMonospaceFont = defaultMonospaceFont;
  ((FontInterface *) clazz->interface)->fontWithAttributes = fontWithAttributes;
  ((FontInterface *) clazz->interface)->fontWithName = fontWithName;
  ((FontInterface *) clazz->interface)->initWithData = initWithData;
  ((FontInterface *) clazz->interface)->name = name;
  ((FontInterface *) clazz->interface)->nameWithAttributes = nameWithAttributes;
  ((FontInterface *) clazz->interface)->renderBitmapCharacters = renderCharactersBitmap;
  ((FontInterface *) clazz->interface)->renderCharacters = renderCharacters;
  ((FontInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillResetBitmap;
  ((FontInterface *) clazz->interface)->sizeBitmapCharacters = sizeCharactersBitmap;
  ((FontInterface *) clazz->interface)->sizeCharacters = sizeCharacters;

  const bool init = TTF_Init();
  assert(init);
  (void) init;

  _cache = $$(Dictionary, dictionary);
  assert(_cache);

  Data *coda = $(alloc(Data), initWithConstMemory, coda_ttf, coda_ttf_len - 1);
  assert(coda);
  cacheFont(coda, DEFAULT_FONT_FAMILY);
  release(coda);

  Data *shareTechMono = $(alloc(Data), initWithConstMemory, shareTechMono_ttf, shareTechMono_ttf_len - 1);
  assert(shareTechMono);
  cacheFont(shareTechMono, DEFAULT_MONOSPACE_FONT_FAMILY);
  release(shareTechMono);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

  release(_cache);
  release(_defaultFont);
  release(_defaultMonospaceFont);

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
      .interfaceSize = sizeof(FontInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
