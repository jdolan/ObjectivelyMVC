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

START_TEST(proportionalFaceHasNoBitmap) {

  Font *coda = $$(Font, defaultFont);
  ck_assert(!TTF_FontIsFixedWidth(coda->font));

  ck_assert_ptr_null(coda->bitmap.surface);

} END_TEST

START_TEST(fixedWidthFaceBakesAutomatically) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  ck_assert(TTF_FontIsFixedWidth(font->font));
  ck_assert_ptr_nonnull(font->bitmap.surface);

  // The bundled monospace face bakes too, so a bitmap font is always available
  ck_assert_ptr_nonnull($$(Font, defaultMonospaceFont)->bitmap.surface);

  release(font);

} END_TEST

START_TEST(textDrawsFromBitmapWhenFixedWidth) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  Text *text = $(alloc(Text), initWithText, "Hello", font);
  ck_assert_ptr_eq(font, text->font);
  ck_assert_ptr_nonnull(text->font->bitmap.surface);

  // Measured against the bitmap: an exact multiple of the cell advance, plus overhang
  const SDL_Size bitmapSize = $(text, naturalSize);
  ck_assert_int_ge(bitmapSize.w, 5 * font->bitmap.advance);
  ck_assert_int_eq(font->bitmap.cellSize.h, bitmapSize.h);

  $(text, setFont, $$(Font, defaultFont));
  ck_assert_ptr_null(text->font->bitmap.surface);

  const SDL_Size fontSize = $(text, naturalSize);
  ck_assert_int_gt(fontSize.w, 0);

  release(text);
  release(font);

} END_TEST

START_TEST(metricsAreUniformAndLogical) {

  Font *font = monospaceFont(2.f);
  if (font == NULL) {
    return;
  }

  const FontBitmap *bitmap = &font->bitmap;
  ck_assert_ptr_nonnull(bitmap->surface);

  ck_assert_int_gt(bitmap->advance, 0);
  ck_assert_int_ge(bitmap->cellSize.w, bitmap->advance);
  ck_assert_int_eq(TTF_GetFontHeight(font->font), bitmap->cellSize.h);

  // The sheet is exactly the grid, one cell per baked codepoint
  ck_assert_int_eq(FONT_BITMAP_COLUMNS * bitmap->cellSize.w, bitmap->surface->w);
  ck_assert_int_eq(FONT_BITMAP_COUNT * bitmap->cellSize.w * bitmap->cellSize.h,
                   bitmap->surface->w * bitmap->surface->h);

  // Cells are physical texels at density 2; reported sizes are logical, so half
  int w, h;
  $(font, sizeBitmapCharacters, "ABCD", 0, &w, &h);
  ck_assert_int_eq((int) ceilf(bitmap->cellSize.h / 2.f), h);
  ck_assert_int_ge(w, (int) (4 * bitmap->advance / 2.f));
  ck_assert_int_le(w, (int) ceilf((4 * bitmap->advance + bitmap->cellSize.w) / 2.f));

  int w2, h2;
  $(font, sizeBitmapCharacters, "AB\nCDEFG", 0, &w2, &h2);
  ck_assert_int_eq((int) ceilf(2 * bitmap->cellSize.h / 2.f), h2);
  ck_assert_int_gt(w2, w);

  // Color escapes are not characters, ^^ is one, and a codepoint past the range draws as '?'
  int w3, w4, w5;
  $(font, sizeBitmapCharacters, "^1ABCD", 0, &w3, NULL);
  ck_assert_int_eq(w, w3);
  $(font, sizeBitmapCharacters, "^^ABCD", 0, &w4, NULL);
  ck_assert_int_eq(w + (int) ceilf(bitmap->advance / 2.f), w4);
  $(font, sizeBitmapCharacters, "AB\xE4\xB8\x80" "D", 0, &w5, NULL);
  ck_assert_int_eq(w, w5);

  release(font);

} END_TEST

START_TEST(wordWrapBreaksAtSpaces) {

  Font *font = monospaceFont(1.f);
  if (font == NULL) {
    return;
  }

  const FontBitmap *bitmap = &font->bitmap;
  ck_assert_ptr_nonnull(bitmap->surface);

  int empty;
  $(font, sizeBitmapCharacters, "", 0, NULL, &empty);
  ck_assert_int_eq(0, empty);

  int lineHeight;
  $(font, sizeBitmapCharacters, "x", 0, NULL, &lineHeight);

  // Room for eight advances: "aaaa bbbb" needs nine, so it wraps to two lines at the space
  const int wrapWidth = 8 * bitmap->advance;

  int w, h;
  $(font, sizeBitmapCharacters, "aaaa bbbb", wrapWidth, &w, &h);
  ck_assert_int_eq(2 * lineHeight, h);
  ck_assert_int_le(w, wrapWidth + bitmap->cellSize.w);

  $(font, sizeBitmapCharacters, "aaaa bbbb", 0, &w, &h);
  ck_assert_int_eq(lineHeight, h);

  release(font);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Font+Bitmap");
  tcase_add_test(tcase, proportionalFaceHasNoBitmap);
  tcase_add_test(tcase, fixedWidthFaceBakesAutomatically);
  tcase_add_test(tcase, textDrawsFromBitmapWhenFixedWidth);
  tcase_add_test(tcase, metricsAreUniformAndLogical);
  tcase_add_test(tcase, wordWrapBreaksAtSpaces);

  Suite *suite = suite_create("Font+Bitmap");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
