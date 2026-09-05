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

#include <check.h>
#include <stdio.h>

#include <Objectively/Number.h>

#include "ObjectivelyMVC.h"

/**
 * @brief Fixed-width faces commonly present on the platforms CI runs on.
 */
static const char *monospacePaths[] = {
  "/System/Library/Fonts/Menlo.ttc",
  "/System/Library/Fonts/Monaco.ttf",
  "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
  "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
  "C:\\Windows\\Fonts\\consola.ttf",
};

/**
 * @brief Registers a fixed-width Font as "Mono", if one is available.
 * @return The Font at the given pixel density, or `NULL` if none was found.
 */
static Font *monospaceFont(float pixelDensity) {

  static bool cached;

  if (!cached) {
    for (size_t i = 0; i < lengthof(monospacePaths); i++) {
      Data *data = $$(Data, dataWithContentsOfFile, monospacePaths[i]);
      if (data) {
        $$(Font, cacheFont, data, "Mono");
        release(data);
        cached = true;
        break;
      }
    }
  }

  if (!cached) {
    printf("No fixed-width face found; skipping\n");
    return NULL;
  }

  const FontAttributes attributes = { "Mono", 16, FontStyleRegular };
  return $$(Font, fontWithAttributes, &attributes, pixelDensity);
}

static Image *filledImage(int w, int h, Uint32 rgba) {

  SDL_Surface *surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
  SDL_FillSurfaceRect(surface, NULL, rgba);

  Image *image = $$(Image, imageWithSurface, surface);
  SDL_DestroySurface(surface);

  return image;
}

START_TEST(proportionalFaceIsRejected) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  Font *coda = $$(Font, defaultFont);
  ck_assert(!TTF_FontIsFixedWidth(coda->font));

  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, coda, ' ', 95, NULL, atlas);
  ck_assert_ptr_null(bitmapFont);

  release(atlas);

} END_TEST

START_TEST(rangeWithoutGlyphsIsRejected) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  // Unassigned codepoints: only the replacement glyph would bake, so there is no advance
  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, 0xE0000, 16, NULL, atlas);
  ck_assert_ptr_null(bitmapFont);

  release(atlas);
  release(font);

} END_TEST

START_TEST(textClearsBitmapFont) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);
  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, ' ', 95, NULL, atlas);
  ck_assert_ptr_nonnull(bitmapFont);
  ck_assert($(atlas, compile));

  Text *text = $(alloc(Text), initWithText, "Hello", NULL);

  $(text, setBitmapFont, bitmapFont);
  ck_assert_ptr_eq(bitmapFont, text->bitmapFont);

  const SDL_Size bitmapSize = $(text, naturalSize);
  ck_assert_int_gt(bitmapSize.w, 0);

  $(text, setBitmapFont, NULL);
  ck_assert_ptr_null(text->bitmapFont);

  const SDL_Size fontSize = $(text, naturalSize);
  ck_assert_int_gt(fontSize.w, 0);

  release(text);
  release(bitmapFont);
  release(atlas);
  release(font);

} END_TEST

START_TEST(metricsAreUniformAndLogical) {

  Font *font = monospaceFont(2.f);
  if (font == NULL) {
    return;
  }

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, ' ', 95, NULL, atlas);
  ck_assert_ptr_nonnull(bitmapFont);
  ck_assert($(atlas, compile));

  ck_assert_int_gt(bitmapFont->advance, 0);
  ck_assert_int_ge(bitmapFont->cellSize.w, bitmapFont->advance);
  ck_assert_int_eq(TTF_GetFontHeight(font->font), bitmapFont->cellSize.h);

  // The grid is one node in the atlas, sized to hold every cell
  ck_assert_int_eq(bitmapFont->columns * bitmapFont->cellSize.w, bitmapFont->cells->rect.w);
  ck_assert_int_ge(bitmapFont->cells->rect.w * bitmapFont->cells->rect.h,
                   (int) (bitmapFont->count + 1) * bitmapFont->cellSize.w * bitmapFont->cellSize.h);

  // Cells are physical texels at density 2; reported sizes are logical, so half
  int w, h;
  $(bitmapFont, sizeCharacters, "ABCD", false, 0, &w, &h);
  ck_assert_int_eq((int) ceilf(bitmapFont->cellSize.h / 2.f), h);
  ck_assert_int_ge(w, (int) (4 * bitmapFont->advance / 2.f));
  ck_assert_int_le(w, (int) ceilf((4 * bitmapFont->advance + bitmapFont->cellSize.w) / 2.f));

  int w2, h2;
  $(bitmapFont, sizeCharacters, "AB\nCDEFG", false, 0, &w2, &h2);
  ck_assert_int_eq((int) ceilf(2 * bitmapFont->cellSize.h / 2.f), h2);
  ck_assert_int_gt(w2, w);

  // Color escapes are not characters; the replacement glyph is
  int w3, w4, w5;
  $(bitmapFont, sizeCharacters, "^1ABCD", true, 0, &w3, NULL);
  ck_assert_int_eq(w, w3);
  $(bitmapFont, sizeCharacters, "^1ABCD", false, 0, &w4, NULL);
  ck_assert_int_gt(w4, w);
  $(bitmapFont, sizeCharacters, "AB\xC3\xA9" "D", false, 0, &w5, NULL);
  ck_assert_int_eq(w, w5);

  release(bitmapFont);
  release(atlas);
  release(font);

} END_TEST

START_TEST(namedImagesSpanSquareCells) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  Image *heart = filledImage(32, 32, 0xff0000ff);
  Dictionary *named = $$(Dictionary, dictionaryWithObjectsAndKeys, heart, str("heart"), NULL);

  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, ' ', 95, named, atlas);
  ck_assert_ptr_nonnull(bitmapFont);
  ck_assert($(atlas, compile));

  ck_assert_int_eq(max(1, bitmapFont->cellSize.h / bitmapFont->cellSize.w), bitmapFont->span);

  const Number *cell = $(bitmapFont->named, objectForKeyPath, "heart");
  ck_assert_ptr_nonnull(cell);
  ck_assert_int_ge((int) cell->value, (int) bitmapFont->count + 1);

  int glyph, image, literal;
  $(bitmapFont, sizeCharacters, "A", false, 0, &glyph, NULL);
  $(bitmapFont, sizeCharacters, ":heart:", false, 0, &image, NULL);
  $(bitmapFont, sizeCharacters, ":nope:", false, 0, &literal, NULL);

  ck_assert_int_eq(glyph - bitmapFont->advance + bitmapFont->span * bitmapFont->advance, image);
  ck_assert_int_eq(glyph - bitmapFont->advance + 6 * bitmapFont->advance, literal);

  // The image's cells hold its pixels, in the sheet
  const SDL_Surface *sheet = atlas->image->surface;
  const int x = bitmapFont->cells->rect.x + ((int) cell->value % bitmapFont->columns) * bitmapFont->cellSize.w;
  const int y = bitmapFont->cells->rect.y + ((int) cell->value / bitmapFont->columns) * bitmapFont->cellSize.h;
  const Uint32 pixel = *(const Uint32 *) ((const Uint8 *) sheet->pixels + (y + 1) * sheet->pitch + (x + 1) * 4);
  ck_assert_uint_eq(0xff0000ff, pixel);

  release(bitmapFont);
  release(named);
  release(heart);
  release(atlas);
  release(font);

} END_TEST

START_TEST(wordWrapBreaksAtSpaces) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);
  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, ' ', 95, NULL, atlas);
  ck_assert_ptr_nonnull(bitmapFont);
  ck_assert($(atlas, compile));

  int empty;
  $(bitmapFont, sizeCharacters, "", false, 0, NULL, &empty);
  ck_assert_int_eq(0, empty);

  int lineHeight;
  $(bitmapFont, sizeCharacters, "x", false, 0, NULL, &lineHeight);

  // Room for eight advances: "aaaa bbbb" needs nine, so it wraps to two lines at the space
  const int wrapWidth = 8 * bitmapFont->advance;

  int w, h;
  $(bitmapFont, sizeCharacters, "aaaa bbbb", false, wrapWidth, &w, &h);
  ck_assert_int_eq(2 * lineHeight, h);
  ck_assert_int_le(w, wrapWidth + bitmapFont->cellSize.w);

  $(bitmapFont, sizeCharacters, "aaaa bbbb", false, 0, &w, &h);
  ck_assert_int_eq(lineHeight, h);

  release(bitmapFont);
  release(atlas);
  release(font);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("BitmapFont");
  tcase_add_test(tcase, proportionalFaceIsRejected);
  tcase_add_test(tcase, rangeWithoutGlyphsIsRejected);
  tcase_add_test(tcase, textClearsBitmapFont);
  tcase_add_test(tcase, metricsAreUniformAndLogical);
  tcase_add_test(tcase, namedImagesSpanSquareCells);
  tcase_add_test(tcase, wordWrapBreaksAtSpaces);

  Suite *suite = suite_create("BitmapFont");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
