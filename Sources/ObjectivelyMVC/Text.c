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

#define _GNU_SOURCE

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Colors.h"
#include "Text.h"
#include "Theme.h"

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
  { 0xFF, 0xFF, 0xFF, 0xFF },  // ^7 White
  { 0xFF, 0x80, 0x00, 0xFF },  // ^8 Orange
  { 0x80, 0x80, 0x80, 0xFF }   // ^9 Grey
};

bool MVC_HasColorEscapes(const char *text) {

  for (const char *p = text ? strchr(text, '^') : NULL; p; p = strchr(p + 1, '^')) {
    if ((p[1] >= '0' && p[1] <= '9') || p[1] == '^') {
      return true;
    }
  }

  return false;
}

char *MVC_StripColorEscapes(const char *text) {

  assert(text);

  char *stripped = malloc(strlen(text) + 1);
  size_t idx = 0;

  for (const char *p = text; *p; p++) {
    if (p[0] == '^' && p[1] >= '0' && p[1] <= '9') {
      p++;
    } else if (p[0] == '^' && p[1] == '^') {
      stripped[idx++] = *p++;
    } else {
      stripped[idx++] = *p;
    }
  }

  stripped[idx] = '\0';
  return stripped;
}

size_t MVC_IconEscapeLength(const char *chars, const ImageAtlas *icons, AtlasImage **image) {

  if (image) {
    *image = NULL;
  }

  if (chars == NULL || icons == NULL || chars[0] != ':') {
    return 0;
  }

  const char *q = chars + 1;
  while (SDL_isalnum((unsigned char) *q) || *q == '_' || *q == '-') {
    q++;
  }

  char name[64];
  const size_t len = q - chars - 1;

  if (*q != ':' || len == 0 || len >= sizeof(name)) {
    return 0;
  }

  memcpy(name, chars + 1, len);
  name[len] = '\0';

  AtlasImage *icon = $(icons, imageWithName, name);
  if (icon == NULL) {
    return 0;
  }

  if (image) {
    *image = icon;
  }

  return len + 2;
}

/**
 * @brief A growable string for MVC_LayoutText.
 */
typedef struct {
  char *chars;
  size_t length;
  size_t capacity;
} LayoutBuffer;

static void append(LayoutBuffer *buffer, const char *chars, size_t length) {

  if (buffer->length + length + 1 > buffer->capacity) {
    buffer->capacity = max(buffer->capacity * 2, buffer->length + length + 1);
    buffer->chars = realloc(buffer->chars, buffer->capacity);
    assert(buffer->chars);
  }

  memcpy(buffer->chars + buffer->length, chars, length);
  buffer->length += length;
  buffer->chars[buffer->length] = '\0';
}

/**
 * @brief The placeholder an icon occupies in `font`'s layout: a codepoint that is not a
 * line-break opportunity, repeated until it is at least a line height wide. Resolved once per
 * Font, on first use.
 * @details Measured with TTF_GetStringSize, the same engine that lays out and renders the text,
 * rather than glyph metrics: Coda has no U+00A0, and shaping substitutes the space glyph for it
 * at the space's advance, where the glyph metrics would report `.notdef`'s.
 */
static const char *iconPlaceholder(const Font *font) {

  if (font->iconPlaceholder) {
    return font->iconPlaceholder;
  }

  static const Uint32 candidates[] = { 0x00A0, 0x2007, '_' };

  Uint32 codepoint = candidates[lengthof(candidates) - 1];
  for (size_t i = 0; i < lengthof(candidates); i++) {
    if (TTF_FontHasGlyph(font->font, candidates[i])) {
      codepoint = candidates[i];
      break;
    }
  }

  char utf8[5] = { 0 };
  SDL_UCS4ToUTF8(codepoint, utf8);

  const int lineHeight = TTF_GetFontHeight(font->font);

  LayoutBuffer placeholder = { 0 };

  for (int i = 0, width = 0; i < 16 && width < lineHeight; i++) {
    append(&placeholder, utf8, strlen(utf8));
    if (!TTF_GetStringSize(font->font, placeholder.chars, 0, &width, NULL)) {
      break;
    }
  }

  // A lazily resolved cache on an otherwise immutable Font, like its bitmap's Texture
  ((Font *) font)->iconPlaceholder = placeholder.chars;

  return font->iconPlaceholder;
}

static void appendSpan(TextSpan **spans, size_t *count, TextSpan span) {

  if (spans == NULL) {
    return;
  }

  TextSpan *grown = realloc(*spans, (*count + 1) * sizeof(TextSpan));
  assert(grown);

  *spans = grown;
  (*spans)[(*count)++] = span;
}

char *MVC_LayoutText(const Font *font, const char *text, SDL_Color color, const ImageAtlas *icons, TextSpan **spans, size_t *count) {

  assert(font);
  assert(text);

  LayoutBuffer layout = { 0 };
  append(&layout, "", 0);

  size_t n = 0;
  if (spans) {
    *spans = NULL;
  }

  size_t start = 0;

  for (const char *p = text; ; ) {

    const bool end = *p == '\0';
    const bool escape = !end && p[0] == '^' && p[1] >= '0' && p[1] <= '9';

    AtlasImage *icon = NULL;
    const size_t iconLength = (!end && *p == ':') ? MVC_IconEscapeLength(p, icons, &icon) : 0;

    if (end || escape || iconLength) {
      if (layout.length > start) {
        appendSpan(spans, &n, (TextSpan) { (int) start, (int) (layout.length - start), color, NULL });
      }
      if (end) {
        break;
      }
      if (escape) {
        color = TextEscapeColors[p[1] - '0'];
        p += 2;
      } else {
        const char *placeholder = iconPlaceholder(font);
        appendSpan(spans, &n, (TextSpan) { (int) layout.length, (int) strlen(placeholder), Colors.White, icon });
        append(&layout, placeholder, strlen(placeholder));
        p += iconLength;
      }
      start = layout.length;
    } else if (p[0] == '^' && p[1] == '^') {
      append(&layout, "^", 1);
      p += 2;
    } else {
      append(&layout, p, 1);
      p++;
    }
  }

  if (count) {
    *count = n;
  }

  return layout.chars;
}

/**
 * @brief Releases this Text's rendered texture and runs, and invalidates its cached size.
 */
static void invalidate(Text *self) {

  self->texture = release(self->texture);

  for (size_t i = 0; i < self->runCount; i++) {
    release(self->runs[i].icon);
  }

  free(self->runs);
  self->runs = NULL;
  self->runCount = 0;

  self->naturalSizeCache.isValid = false;
}

/**
 * @return The icon ImageAtlas of this Text's window's Theme, or `NULL` when detached.
 */
static ImageAtlas *iconsFor(const Text *self) {

  const View *view = (View *) self;

  Theme *theme = view->window ? $$(Theme, theme, view->window) : NULL;

  return theme ? $(theme, icons) : NULL;
}

/**
 * @brief Invalidates this Text if the icon atlas it was prepared against has changed: a Theme
 * swap, attaching to a window, or an icon registered since. Sets `needsLayout` when it does,
 * since the natural size may have changed.
 */
static void checkIcons(Text *self) {

  const ImageAtlas *icons = iconsFor(self);
  const unsigned generation = icons ? icons->generation : 0;

  if (icons != self->icons.atlas || generation != self->icons.generation) {
    invalidate(self);
    self->icons.atlas = icons;
    self->icons.generation = generation;
    $((View *) self, setNeedsLayout);
  }
}

/**
 * @return True if `text` contains anything MVC_LayoutText might resolve: a caret or a colon.
 */
static bool hasEscapes(const char *text) {
  return MVC_HasColorEscapes(text) || strchr(text, ':') != NULL;
}

/**
 * @brief Appends the runs for one TextSpan of `layout`: one color run per line the span touches,
 * or a single icon run positioned by the span's first line.
 */
static void appendRuns(Text *self, TTF_Text *layout, const TextSpan *span, int surfaceWidth) {

  int count = 0;
  TTF_SubString **substrings = TTF_GetTextSubStringsForRange(layout, span->offset, span->length, &count);
  assert(substrings);

  if (count == 0) {
    SDL_free(substrings);
    return;
  }

  TextRun *runs = realloc(self->runs, (self->runCount + count) * sizeof(TextRun));
  assert(runs);
  self->runs = runs;

  if (span->icon) {
    // The placeholder is one word, so it only splits when the wrap width is narrower than a
    // line height; the icon stays where the escape is, in the first fragment, and is clamped
    // to it when drawn
    self->runs[self->runCount++] = (TextRun) { substrings[0]->rect, Colors.White, retain(span->icon) };
    SDL_free(substrings);
    return;
  }

  for (int i = 0; i < count; i++) {
    SDL_Rect rect = substrings[i]->rect;

    // A cluster's rect is its advance box; the last glyph on a line can ink past it, and the
    // surface is sized to include that, so let the line's last run reach the surface's edge
    if (substrings[i]->flags & TTF_SUBSTRING_LINE_END) {
      rect.w = surfaceWidth - rect.x;
    }

    self->runs[self->runCount++] = (TextRun) { rect, span->color, NULL };
  }

  SDL_free(substrings);
}

/**
 * @brief Builds this Text's runs for its rendered texture: the texture is rendered from the
 * layout string in white, so each color run is a region of it drawn in the color in effect for
 * those characters, and each icon run is the placeholder region an icon is drawn over. The
 * layout is SDL_ttf's own, so the regions are exact glyph clusters, including where wrapping
 * broke the lines.
 * @remarks A cluster's rect is its advance box: ink that overhangs a run boundary within a line
 * (italics, tight kerning) is clipped to whichever run owns that column.
 */
static void buildRuns(Text *self, const char *layoutText, const TextSpan *spans, size_t count, int wrapWidth, int surfaceWidth) {

  assert(self->runs == NULL);

  TTF_Text *layout = TTF_CreateText(NULL, self->font->font, layoutText, 0);
  assert(layout);

  TTF_SetTextWrapWidth(layout, (int) (wrapWidth * self->font->pixelDensity));

  for (size_t i = 0; i < count; i++) {
    appendRuns(self, layout, &spans[i], surfaceWidth);
  }

  TTF_DestroyText(layout);
}

/**
 * @brief Resolves and applies the Font with the given attributes: through the window's
 * Theme when attached, which supplies the window's pixel density, or at a density
 * of 1.0 otherwise, to be re-resolved on attachment via View::didMoveToWindow. The
 * cache's reference is retained to mirror the owned reference the fallback returns.
 */
static void resolveFont(Text *self, const FontAttributes *attributes) {

  View *view = (View *) self;

  Theme *theme = view->window ? $$(Theme, theme, view->window) : NULL;

  Font *font;
  if (theme) {
    const float pixelDensity = SDL_GetWindowPixelDensity(view->window);
    font = retain($(theme, font, attributes, pixelDensity));
  } else {
    font = $$(Font, fontWithAttributes, attributes, 1.f);
  }

  assert(font);

  $(self, setFont, font);
  release(font);
}

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Text *this = (Text *) self;

  invalidate(this);

  release(this->font);

  free(this->text);

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
    invalidate(this);
  }

  char *fontFamily = NULL;
  int fontSize = -1, fontStyle = -1;

  const Inlet fontInlets[] = MakeInlets(
    MakeInlet("font-family", InletTypeCharacters, &fontFamily, NULL),
    MakeInlet("font-size", InletTypeInteger, &fontSize, NULL),
    MakeInlet("font-style", InletTypeEnum, &fontStyle, (ident) FontStyleNames)
  );

  if ($(self, bind, fontInlets, style->attributes)) {

    const FontAttributes attributes = { fontFamily, fontSize, fontStyle };
    resolveFont(this, &attributes);

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
    MakeInlet("lineWrap", InletTypeBool, &this->lineWrap, NULL),
    MakeInlet("text", InletTypeCharacters, &this->text, NULL)
  );

  $(self, bind, inlets, dictionary);

  this->naturalSizeCache.isValid = false;

  $(self, sizeToFit);
}

/**
 * @brief Re-resolves this Text's Font at the current window's pixel density. Shared by
 * didMoveToWindow and renderDeviceDidReset, rather than the latter dispatching through the
 * public didMoveToWindow, which would re-trigger View's own attachment side effects.
 */
static void refreshFont(Text *self) {

  const View *view = (View *) self;

  if (view->window && self->font) {
    const FontAttributes attributes = { self->font->family, self->font->size, self->font->style };
    resolveFont(self, &attributes);
  }
}

/**
 * @see View::didMoveToWindow(View *, SDL_Window *)
 */
static void didMoveToWindow(View *self, SDL_Window *window) {

  super(View, self, didMoveToWindow, window);

  refreshFont((Text *) self);

  // Icons resolve through the window's Theme; sizing re-prepares against it (see checkIcons)
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

  const float scale = this->font->pixelDensity;

  if (this->text) {

    checkIcons(this);

    const SDL_Rect frame = $(self, renderFrame);

    const int wrapWidth = this->lineWrap ? frame.w : 0;

    if (this->font->bitmap.surface) {
      $(this->font, renderBitmapCharacters, renderer, this->text, this->color, wrapWidth,
        &(const SDL_Point) { frame.x, frame.y }, this->icons.atlas);
      return;
    }

    if (this->texture == NULL) {
      SDL_Surface *surface = NULL;

      if (hasEscapes(this->text)) {
        TextSpan *spans = NULL;
        size_t count = 0;

        char *layout = MVC_LayoutText(this->font, this->text, this->color, this->icons.atlas, &spans, &count);

        // A colon or caret that resolved to nothing -- "Health: 100", a URL -- is plain text,
        // and takes the single-quad path rather than a run per line
        bool plain = true;
        for (size_t i = 0; i < count && plain; i++) {
          plain = spans[i].icon == NULL && SDL_memcmp(&spans[i].color, &this->color, sizeof(SDL_Color)) == 0;
        }

        // Text that is nothing but escapes has nothing to draw
        if (*layout && plain) {
          surface = $(this->font, renderCharacters, layout, this->color, wrapWidth);
        } else if (*layout) {
          surface = $(this->font, renderCharacters, layout, Colors.White, wrapWidth);
          assert(surface);
          buildRuns(this, layout, spans, count, wrapWidth, surface->w);
        }

        free(spans);
        free(layout);

        if (surface == NULL) {
          return;
        }
      } else {
        surface = $(this->font, renderCharacters, this->text, this->color, wrapWidth);
      }

      assert(surface);

      const SDL_GPUTextureCreateInfo texInfo = {
        .type                 = SDL_GPU_TEXTURETYPE_2D,
        .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width                = (Uint32) surface->w,
        .height               = (Uint32) surface->h,
        .layer_count_or_depth = 1,
        .num_levels           = 1,
      };

      this->texture = $(renderer->device, createTexture, &texInfo, surface->pixels);

      SDL_DestroySurface(surface);
    }

    assert(this->texture);

    // The destination size must be the texture's exact native size divided by scale, not that
    // rounded to an integer: rounding it first, then having the renderer's projection
    // multiply back by scale to reach physical pixels, lands on a physical width that differs
    // from the texture's actual resolution -- stretching it by that (sub-)pixel remainder. Since
    // the remainder depends on the string's own pixel width, this stretch changes with every
    // keystroke, visibly shifting every glyph in the string, not just the one that was typed.

    if (this->runs) {
      const int lineHeight = TTF_GetFontHeight(this->font->font);

      for (size_t i = 0; i < this->runCount; i++) {
        const TextRun *run = &this->runs[i];

        if (run->icon) {
          // Drawn from the icon atlas, not this Text's texture, so an icon splits the Renderer's
          // merged draw; centered in its placeholder, which is at least a line height wide
          if (run->icon->atlas) {
            const int side = min(lineHeight, run->src.w);
            const SDL_FRect dest = {
              frame.x + (run->src.x + (run->src.w - side) / 2) / scale, frame.y + run->src.y / scale,
              side / scale, side / scale
            };
            Texture *texture = $(run->icon->atlas, texture, renderer->device);
            $(renderer, drawTextureRegion, texture, &run->icon->rect, &dest, &Colors.White);
          }
          continue;
        }

        const SDL_FRect dest = {
          frame.x + run->src.x / scale, frame.y + run->src.y / scale,
          run->src.w / scale, run->src.h / scale
        };
        $(renderer, drawTextureRegion, this->texture, &run->src, &dest, &run->color);
      }
    } else {
      const SDL_FRect dest = {
        (float) frame.x, (float) frame.y,
        this->texture->size.w / scale, this->texture->size.h / scale
      };
      $(renderer, drawTexture, this->texture, &dest, &Colors.White);
    }
  }
}

/**
 * @see View::renderDeviceDidReset(View *)
 */
static void renderDeviceDidReset(View *self) {

  refreshFont((Text *) self);

  $(self, sizeToFit);

  super(View, self, renderDeviceDidReset);
}

/**
 * @see View::renderDeviceWillReset(View *)
 */
static void renderDeviceWillReset(View *self) {

  invalidate((Text *) self);

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

  Font *font = self->font;
  if (font == NULL) {
    return MakeSize(0, 0);
  }

  // Nominally const, but a changed icon atlas invalidates the texture and runs here. Safe only
  // because layout precedes drawing within a frame and the Renderer keeps unretained Textures.
  Text *this = (Text *) self;

  checkIcons(this);

  if (self->naturalSizeCache.isValid && font->pixelDensity == self->naturalSizeCache.pixelDensity) {
    return self->naturalSizeCache.size;
  }

  const SDL_Size size = $(self, sizeText, self->text ?: "");

  this->naturalSizeCache.size = size;
  this->naturalSizeCache.pixelDensity = font->pixelDensity;
  this->naturalSizeCache.isValid = true;

  return size;
}

/**
 * @fn SDL_Size Text::sizeText(const Text *self, const char *text)
 * @memberof Text
 */
static SDL_Size sizeText(const Text *self, const char *text) {

  SDL_Size size = MakeSize(0, 0);

  if (self->font == NULL || text == NULL) {
    return size;
  }

  const ImageAtlas *icons = iconsFor(self);

  if (self->font->bitmap.surface) {
    $(self->font, sizeBitmapCharacters, text, 0, icons, &size.w, &size.h);
  } else if (hasEscapes(text)) {
    char *layout = MVC_LayoutText(self->font, text, self->color, icons, NULL, NULL);
    $(self->font, sizeCharacters, layout, &size.w, &size.h);
    free(layout);
  } else {
    $(self->font, sizeCharacters, text, &size.w, &size.h);
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

    invalidate(self);

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

    invalidate(self);

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
  ((ViewInterface *) clazz->interface)->didMoveToWindow = didMoveToWindow;
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
  ((TextInterface *) clazz->interface)->sizeText = sizeText;
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
      .interfaceSize = sizeof(TextInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
