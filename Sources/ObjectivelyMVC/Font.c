/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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
#include <stdlib.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
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

static MutableDictionary *_cache;
static MutableArray *_fonts;

/**
 * @fn void Font::cacheFont(Data *data, const char *family)
 * @memberof Font
 */
static void cacheFont(Data *data, const char *family) {
  $(_cache, setObjectForKeyPath, data, family);
}

/**
 * @fn Font *Font::cachedFont(const char *family, int size, int style)
 * @memberof Font
 */
static Font *cachedFont(const char *family, int size, int style) {

  if (family == NULL) {
    family = DEFAULT_FONT_FAMILY;
  }
  if (size < 1) {
    size = DEFAULT_FONT_SIZE;
  }
  if (style < FontStyleRegular || style > FontStyleStrikeThrough) {
    style = DEFAULT_FONT_STYLE;
  }

  const int renderSize = size * SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());

  const Array *fonts = (Array *) _fonts;
  for (size_t i = 0; i < fonts->count; i++) {

    Font *font = $(fonts, objectAtIndex, i);

    if (!strcmp(font->family, family) &&
        font->size == size &&
        font->style == style &&
        font->renderSize == renderSize) {
      return font;
    }
  }

  Data *data = $((Dictionary *) _cache, objectForKeyPath, family);
  if (data) {
    Font *font = $(alloc(Font), initWithData, data, family, size, style);
    assert(font);

    $(_fonts, addObject, font);
    release(font);

    return font;
  }

  MVC_LogWarn("%s-%d-%d not found\n", family, size, style);
  return $$(Font, defaultFont);
}

/**
 * @fn void Font::clearCache(void)
 * @memberof Font
 */
static void clearCache(void) {
  $(_cache, removeAllObjects);
}

/**
 * @fn Font *Font::defaultFont(void)
 * @memberof Font
 */
static Font *defaultFont(void) {
  static Once once;

  do_once(&once, {
    Data *data = $(alloc(Data), initWithConstMemory, coda_ttf, coda_ttf_len);
    assert(data);

    $$(Font, cacheFont, data, DEFAULT_FONT_FAMILY);

    release(data);
  });

  return $$(Font, cachedFont, DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE);
}

/**
 * @fn Font *Font::initWithData(Font *self, Data *data, int size, int index)
 * @memberof Font
 */
static Font *initWithData(Font *self, Data *data, const char *family, int size, int style) {

  self = (Font *) super(Object, self, init);
  if (self) {

    self->data = retain(data);
    assert(self->data);

    self->family = strdup(family);
    assert(self->family);

    self->size = size;
    assert(self->size);

    self->style = style;

    $(self, renderDeviceDidReset);
  }

  return self;
}

#pragma mark - Color Escape Sequences

/**
 * @brief Color palette for escape sequences (^0-^7)
 * 
 * This palette defines the colors used for text escape sequences in the format ^0-^7.
 * Embedding applications can customize these colors by modifying this array at runtime
 * before rendering text with color codes.
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
 * @warning Do not modify the array size (must remain 8 elements).
 * @warning Modifications should be made early in initialization, before rendering text.
 * 
 * Example customization:
 * @code
 * // Use a custom color scheme
 * FontEscapeColors[1] = MakeSDLColor(0xFF, 0x80, 0x00, 0xFF);  // ^1 = Orange
 * @endcode
 */
SDL_Color FontEscapeColors[] = {
  { 0x00, 0x00, 0x00, 0xFF },  // ^0 Black
  { 0xFF, 0x00, 0x00, 0xFF },  // ^1 Red
  { 0x00, 0xFF, 0x00, 0xFF },  // ^2 Green
  { 0xFF, 0xFF, 0x00, 0xFF },  // ^3 Yellow
  { 0x00, 0x00, 0xFF, 0xFF },  // ^4 Blue
  { 0xFF, 0x00, 0xFF, 0xFF },  // ^5 Magenta
  { 0x00, 0xFF, 0xFF, 0xFF },  // ^6 Cyan
  { 0xFF, 0xFF, 0xFF, 0xFF }   // ^7 White
};

/**
 * @brief Strip color escape sequences from text.
 */
static char *stripColors(const char *text) {

  assert(text);

  char *stripped = malloc(strlen(text) + 1);
  int idx = 0;

  for (const char *p = text; *p; p++) {
    if (p[0] == '^' && p[1] >= '0' && p[1] <= '7') {
      p++; // Skip color code
    } else {
      stripped[idx++] = *p;
    }
  }

  stripped[idx] = '\0';
  return stripped;
}

/**
 * @brief Size colored text by stripping colors and measuring the result
 */
static void sizeCharactersWithColors(const Font *self, const char *text,
                                     int *w, int *h) {
  char *stripped = stripColors(text);
  $(self, sizeCharacters, stripped, w, h);
  free(stripped);
}

/**
 * @brief Character info for color-escape encoded Strings.
 */
typedef struct {
  SDL_Rect rect;
  SDL_Color color;
} CharInfo;

/**
 * @brief Colorize a character's rect on an SDL_Surface (surface must be locked).
 */
static void colorize(SDL_Surface *surface, const CharInfo *charInfo, float scale) {

  const SDL_Rect *rect = &charInfo->rect;
  int x = (int) floorf(rect->x * scale) - 1;
  int y = (int) floorf(rect->y * scale) - 1;
  int w = (int) ceilf(rect->w * scale) + 2;
  int h = (int) ceilf(rect->h * scale) + 2;

  if (w <= 0 || h <= 0) {
    return;
  }

  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }

  uint32_t *pixels = (uint32_t *) surface->pixels;
  const int pitch = surface->pitch / 4;
  const SDL_Color color = charInfo->color;

  for (int row = y; row < y + h && row < surface->h; row++) {
    for (int col = x; col < x + w && col < surface->w; col++) {
      SDL_Color *c = (SDL_Color *) &pixels[row * pitch + col];
      c->r = (int) (c->r * color.r / 255.0f);
      c->g = (int) (c->g * color.g / 255.0f);
      c->b = (int) (c->b * color.b / 255.0f);
    }
  }
}

/**
 * @brief Build a CharInfo table for text with color escape sequences.
 *
 * For each visible character at position @c charIdx in the stripped string, this function
 * calls @c TTF_GetStringSizeWrapped on the prefix up through that character to detect line
 * breaks via height changes. The x position within each line is measured with
 * @c TTF_GetStringSize from the start of the current line. SDL_ttf handles all word-wrapping;
 * this function only observes the results.
 */
static CharInfo *buildCharInfo(const Font *self, const char *text,
                               SDL_Color defaultColor, int wrapWidth,
                               int *outCount) {

  if (!text || !*text) {
    *outCount = 0;
    return NULL;
  }

  char *stripped = stripColors(text);
  if (!stripped || !*stripped) {
    free(stripped);
    *outCount = 0;
    return NULL;
  }

  const size_t strippedLen = strlen(stripped);
  CharInfo *chars = malloc(sizeof(CharInfo) * strippedLen);

  const float scale = SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());
  const int scaledWrapWidth = wrapWidth ? (int) (wrapWidth * scale) : 0;

  int lineHeight;
  $(self, sizeCharacters, "A", NULL, &lineHeight);

  SDL_Color currentColor = defaultColor;
  int charIdx = 0;
  int lineStartByte = 0;
  int prevH = 0;

  for (const char *p = text; *p; p++) {
    if (p[0] == '^' && p[1] >= '0' && p[1] <= '7') {
      currentColor = FontEscapeColors[p[1] - '0'];
      p++;
      continue;
    }

    int currH;
    if (scaledWrapWidth) {
      TTF_GetStringSizeWrapped(self->font, stripped, charIdx + 1, scaledWrapWidth, NULL, &currH);
    } else {
      TTF_GetStringSize(self->font, stripped, charIdx + 1, NULL, &currH);
    }

    if (currH > prevH) {
      lineStartByte = charIdx;
      prevH = currH;
    }

    int lineX = 0;
    if (charIdx > lineStartByte) {
      TTF_GetStringSize(self->font, stripped + lineStartByte, charIdx - lineStartByte, &lineX, NULL);
    }

    int charW;
    TTF_GetStringSize(self->font, stripped + charIdx, 1, &charW, NULL);

    chars[charIdx].rect.x = (int) (lineX / scale);
    chars[charIdx].rect.y = (int) (currH / scale) - lineHeight;
    chars[charIdx].rect.w = (int) (charW / scale);
    chars[charIdx].rect.h = lineHeight;
    chars[charIdx].color = currentColor;

    charIdx++;
  }

  free(stripped);
  *outCount = charIdx;
  return chars;
}

/**
 * @fn SDL_Surface *Font::renderCharactersWithColors(const Font *self, const char *chars, SDL_Color defaultColor, int wrapWidth)
 * @memberof Font
 */
static SDL_Surface *renderCharactersWithColors(const Font *self, const char *chars,
                                                SDL_Color defaultColor, int wrapWidth) {

  // Render stripped text to get correct wrapping
  char *stripped = stripColors(chars);
  SDL_Surface *surface = $(self, renderCharacters, stripped, defaultColor, wrapWidth);
  free(stripped);

  if (!surface) {
    return NULL;
  }

  // Build character info directly from original text with colors
  int charCount = 0;
  CharInfo *charInfo = buildCharInfo(self, chars, defaultColor, wrapWidth, &charCount);

  if (charInfo) {
    const float scale = SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());
    SDL_LockSurface(surface);
    for (int i = 0; i < charCount; i++) {
      colorize(surface, &charInfo[i], scale);
    }
    SDL_UnlockSurface(surface);
    free(charInfo);
  }

  return surface;
}

/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth)
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color, int wrapWidth) {

  SDL_Surface *surface;
  if (wrapWidth) {
    const float scale = SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());
    surface = TTF_RenderText_Blended_Wrapped(self->font, chars, 0, color, wrapWidth * scale);
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
 * @fn void Font::renderDeviceDidReset(Font *self)
 * @memberof Font
 */
static void renderDeviceDidReset(Font *self) {

  const int renderSize = self->size * SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());
  if (renderSize != self->renderSize) {

    self->renderSize = renderSize;

    if (self->font) {
      TTF_CloseFont(self->font);
    }

    SDL_IOStream *buffer = SDL_IOFromConstMem(self->data->bytes, (int) self->data->length);
    assert(buffer);

    self->font = TTF_OpenFontIO(buffer, 1, self->renderSize);
    assert(self->font);

    TTF_SetFontStyle(self->font, self->style);
  }
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

    for (char *line = strtok(lines, "\n\r"); line; line = strtok(NULL, "\n\r")) {

      int line_w, line_h;
      TTF_GetStringSize(self->font, line, 0, &line_w, &line_h);

      if (w) {
        *w = max(*w, line_w);
      }
      if (h) {
        *h += line_h;
      }
    }
    free(lines);

    const float scale = SDL_GetWindowDisplayScale(SDL_GL_GetCurrentWindow());
    if (w) {
      *w = ceilf(*w / scale);
    }
    if (h) {
      *h = ceilf(*h / scale);
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

  ((FontInterface *) clazz->interface)->cachedFont = cachedFont;
  ((FontInterface *) clazz->interface)->cacheFont = cacheFont;
  ((FontInterface *) clazz->interface)->clearCache = clearCache;
  ((FontInterface *) clazz->interface)->defaultFont = defaultFont;
  ((FontInterface *) clazz->interface)->initWithData = initWithData;
  ((FontInterface *) clazz->interface)->renderCharacters = renderCharacters;
  ((FontInterface *) clazz->interface)->renderCharactersWithColors = renderCharactersWithColors;
  ((FontInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((FontInterface *) clazz->interface)->sizeCharacters = sizeCharacters;
  ((FontInterface *) clazz->interface)->sizeCharactersWithColors = sizeCharactersWithColors;

  const bool init = TTF_Init();
  assert(init);

  _cache = $$(MutableDictionary, dictionary);
  assert(_cache);

  _fonts = $$(MutableArray, array);
  assert(_fonts);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

  release(_cache);
  release(_fonts);

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
