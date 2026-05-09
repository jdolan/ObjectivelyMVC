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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Text.h"

#define _Class _Text

#pragma mark - Color Escape Sequences

SDL_Color TextEscapeColors[] = {
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
 * @brief Strips color escape sequences from text, returning a newly allocated string.
 */
static char *stripColors(const char *text) {

  assert(text);

  char *stripped = malloc(strlen(text) + 1);
  int idx = 0;

  for (const char *p = text; *p; p++) {
    if (p[0] == '^' && p[1] >= '0' && p[1] <= '7') {
      p++;
    } else {
      stripped[idx++] = *p;
    }
  }

  stripped[idx] = '\0';
  return stripped;
}

/**
 * @brief Character position and color for one visible character in a color-escaped string.
 */
typedef struct {
  SDL_Rect rect;
  SDL_Color color;
} CharInfo;

/**
 * @brief Colorizes a character's rect on a locked SDL_Surface.
 */
static void colorize(SDL_Surface *surface, const CharInfo *info, float scale) {

  int x = (int) floorf(info->rect.x * scale) - 1;
  int y = (int) floorf(info->rect.y * scale) - 1;
  int w = (int) ceilf(info->rect.w * scale) + 2;
  int h = (int) ceilf(info->rect.h * scale) + 2;

  if (w <= 0 || h <= 0) {
    return;
  }

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  uint32_t *pixels = (uint32_t *) surface->pixels;
  const int pitch = surface->pitch / 4;
  const SDL_Color color = info->color;

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
 * @brief Builds a CharInfo table for text containing color escape sequences.
 *
 * For each visible character, calls TTF_GetStringSizeWrapped on the stripped prefix up
 * through that character to detect line breaks via height changes, then measures x from
 * the line-start byte with TTF_GetStringSize. SDL_ttf owns all word-wrap decisions.
 */
static CharInfo *buildCharInfo(const Font *font, const char *text,
                               SDL_Color defaultColor, int wrapWidth, int *outCount) {

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
  $(font, sizeCharacters, "A", NULL, &lineHeight);

  SDL_Color currentColor = defaultColor;
  int charIdx = 0;
  int lineStartByte = 0;
  int prevH = 0;

  for (const char *p = text; *p; p++) {
    if (p[0] == '^' && p[1] >= '0' && p[1] <= '7') {
      currentColor = TextEscapeColors[p[1] - '0'];
      p++;
      continue;
    }

    int currH;
    if (scaledWrapWidth) {
      TTF_GetStringSizeWrapped(font->font, stripped, charIdx + 1, scaledWrapWidth, NULL, &currH);
    } else {
      TTF_GetStringSize(font->font, stripped, charIdx + 1, NULL, &currH);
    }

    if (currH > prevH) {
      lineStartByte = charIdx;
      prevH = currH;
    }

    int lineX = 0;
    if (charIdx > lineStartByte) {
      TTF_GetStringSize(font->font, stripped + lineStartByte, charIdx - lineStartByte, &lineX, NULL);
    }

    int charW;
    TTF_GetStringSize(font->font, stripped + charIdx, 1, &charW, NULL);

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
 * @brief Renders text with color escape sequences applied to an SDL_Surface.
 */
static SDL_Surface *renderWithColorEscapes(const Text *self, int wrapWidth) {

  char *stripped = stripColors(self->text);
  SDL_Surface *surface = $(self->font, renderCharacters, stripped, self->color, wrapWidth);
  free(stripped);

  if (!surface) {
    return NULL;
  }

  int charCount = 0;
  CharInfo *charInfo = buildCharInfo(self->font, self->text, self->color, wrapWidth, &charCount);

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
 * @brief Resolves the rendered size of this Text's content, stripping color escapes.
 */
static void sizeWithColorEscapes(const Text *self, int *w, int *h) {

  char *stripped = stripColors(self->text ?: "");
  $(self->font, sizeCharacters, stripped, w, h);
  free(stripped);
}

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Text *this = (Text *) self;

  release(this->font);

  free(this->text);

  if (this->texture) {
    glDeleteTextures(1, &this->texture);
  }

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  View *this = (View *) self;
  const SDL_Rect bounds = $(this, bounds);

  String *classNames = $((Object *) this->classNames, description);
  String *description = str("%s@%p \"%s\" %s [%d, %d, %d, %d]",
                this->identifier ?: classnameof(self),
                self,
                ((Text *) self)->text,
                classNames->chars,
                bounds.x, bounds.y, bounds.w, bounds.h);

  release(classNames);
  return description;
}

#pragma mark - View

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  Text *this = (Text *) self;

  const Inlet colorInlets[] = MakeInlets(
    MakeInlet("color", InletTypeColor, &this->color, NULL)
  );

  if ($(self, bind, colorInlets, style->attributes)) {
    if (this->texture) {
      glDeleteTextures(1, &this->texture);
      this->texture = 0;
    }
  }

  char *fontFamily = NULL;
  int fontSize = -1, fontStyle = -1;

  const Inlet fontInlets[] = MakeInlets(
    MakeInlet("font-family", InletTypeCharacters, &fontFamily, NULL),
    MakeInlet("font-size", InletTypeInteger, &fontSize, NULL),
    MakeInlet("font-style", InletTypeEnum, &fontStyle, (ident) FontStyleNames)
  );

  if ($(self, bind, fontInlets, style->attributes)) {

    Font *font = $$(Font, cachedFont, fontFamily , fontSize, fontStyle);
    assert(font);

    $(this, setFont, font);

    if (fontFamily) {
      free(fontFamily);
    }
  }
}

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  Text *this = (Text *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("color", InletTypeColor, &this->color, NULL),
    MakeInlet("colorEscapes", InletTypeBool, &this->colorEscapes, NULL),
    MakeInlet("font", InletTypeFont, &this->font, NULL),
    MakeInlet("lineWrap", InletTypeBool, &this->lineWrap, NULL),
    MakeInlet("text", InletTypeCharacters, &this->text, NULL)
  );

  $(self, bind, inlets, dictionary);

  $(self, sizeToFit);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Text *) self, initWithText, NULL, NULL);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

  super(View, self, render, renderer);

  Text *this = (Text *) self;

  assert(this->font);

  if (this->text) {

    const SDL_Rect frame = $(self, renderFrame);

    if (this->texture == 0) {
      SDL_Surface *surface;
      
      if (this->colorEscapes) {
        surface = renderWithColorEscapes(this, this->lineWrap ? frame.w : 0);
      } else {
        surface = $(this->font, renderCharacters,
                    this->text,
                    this->color,
                    this->lineWrap ? frame.w : 0);
      }
      
      assert(surface);

      this->texture = $(renderer, createTexture, surface);

      SDL_DestroySurface(surface);
    }

    assert(this->texture);

    $(renderer, drawTexture, this->texture, &frame);
  }
}

/**
 * @see View::renderDeviceDidReset(View *)
 */
static void renderDeviceDidReset(View *self) {

  Text *this = (Text *) self;

  $(this->font, renderDeviceDidReset);

  super(View, self, renderDeviceDidReset);
}

/**
 * @see View::renderDeviceWillReset(View *)
 */
static void renderDeviceWillReset(View *self) {

  Text *this = (Text *) self;

  if (this->texture) {
    glDeleteTextures(1, &this->texture);
    this->texture = 0;
  }

  super(View, self, renderDeviceWillReset);
}

/**
 * @see View::sizeThatFits(View *)
 */
static SDL_Size sizeThatFits(const View *self) {
  return $((Text *) self, naturalSize);
}

#pragma mark - Text

/**
 * @fn Text *Text::initWithText(Text *self, const char *text, Font *font)
 * @memberof Text
 */
static Text *initWithText(Text *self, const char *text, Font *font) {

  self = (Text *) super(View, self, initWithFrame, NULL);
  if (self) {
    $(self, setFont, font);
    $(self, setText, text);
  }

  return self;
}

/**
 * @fn SDL_Size Text::naturalSize(const Text *self)
 * @memberof Text
 */
static SDL_Size naturalSize(const Text *self) {

  SDL_Size size = MakeSize(0, 0);

  if (self->font) {
    const char *text = self->text ?: "";
    
    if (self->colorEscapes) {
      sizeWithColorEscapes(self, &size.w, &size.h);
    } else {
      $(self->font, sizeCharacters, text, &size.w, &size.h);
    }
  }

  return size;
}

/**
 * @fn void Text::setFont(Text *self, Font *font)
 * @memberof Text
 */
static void setFont(Text *self, Font *font) {

  font = font ?: $$(Font, defaultFont);

  if (font != self->font) {

    release(self->font);
    self->font = retain(font);

    if (self->texture) {
      glDeleteTextures(1, &self->texture);
      self->texture = 0;
    }

    $((View *) self, sizeToFit);
  }
}

/**
 * @fn void Text::setText(Text *self, const char *text)
 * @memberof Text
 */
static void setText(Text *self, const char *text) {

  if (strcmp(self->text ?: "", text ?: "")) {

    free(self->text);

    if (text && strlen(text)) {
      self->text = strdup(text);
    } else {
      self->text = NULL;
    }

    if (self->texture) {
      glDeleteTextures(1, &self->texture);
      self->texture = 0;
    }

    $((View *) self, sizeToFit);
  }
}

/**
 * @fn void Text::setTextWithFormat(Text *self, const char *fmt, ...)
 * @brief Sets this Text's text with the given format string.
 * @param self The Text.
 * @param fmt The format string.
 * @memberof Text
 */
static void setTextWithFormat(Text *self, const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);

  char *text;
  const int len = vasprintf(&text, fmt, args);
  if (len >= 0) {
    $(self, setText, text);
  }

  free(text);
  va_end(args);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->render = render;
  ((ViewInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((ViewInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

  ((TextInterface *) clazz->interface)->initWithText = initWithText;
  ((TextInterface *) clazz->interface)->naturalSize = naturalSize;
  ((TextInterface *) clazz->interface)->setFont = setFont;
  ((TextInterface *) clazz->interface)->setText = setText;
  ((TextInterface *) clazz->interface)->setTextWithFormat = setTextWithFormat;
}

/**
 * @fn Class *Text::_Text(void)
 * @memberof Text
 */
Class *_Text(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Text",
      .superclass = _View(),
      .instanceSize = sizeof(Text),
      .interfaceOffset = offsetof(Text, interface),
      .interfaceSize = sizeof(TextInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
