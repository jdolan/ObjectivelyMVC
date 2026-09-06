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
#include <stdlib.h>
#include <string.h>

#include "ObjectivelyMVC.h"

START_TEST(hasColorEscapes) {

  ck_assert(!MVC_HasColorEscapes(NULL));
  ck_assert(!MVC_HasColorEscapes(""));
  ck_assert(!MVC_HasColorEscapes("Hello"));
  ck_assert(!MVC_HasColorEscapes("x^y"));
  ck_assert(!MVC_HasColorEscapes("^"));

  ck_assert(MVC_HasColorEscapes("^1Hello"));
  ck_assert(MVC_HasColorEscapes("Hello^7"));
  ck_assert(MVC_HasColorEscapes("^^"));

} END_TEST

START_TEST(stripColorEscapes) {

  char *stripped;

  stripped = MVC_StripColorEscapes("^1Hello ^2World^7");
  ck_assert_str_eq("Hello World", stripped);
  free(stripped);

  // ^^ is a literal caret, and never combines with what follows
  stripped = MVC_StripColorEscapes("^^1 x^^ ^^^^");
  ck_assert_str_eq("^1 x^ ^^", stripped);
  free(stripped);

  // A lone caret, or one before a non-digit, is ordinary text
  stripped = MVC_StripColorEscapes("x^y ^");
  ck_assert_str_eq("x^y ^", stripped);
  free(stripped);

} END_TEST

START_TEST(escapesDoNotAffectProportionalSize) {

  Font *font = $$(Font, defaultFont);
  ck_assert_ptr_null(font->bitmap.surface);

  Text *plain = $(alloc(Text), initWithText, "Hello", font);
  Text *colored = $(alloc(Text), initWithText, "^1Hel^2lo^7", font);
  Text *caret = $(alloc(Text), initWithText, "^^Hello", font);

  const SDL_Size plainSize = $(plain, naturalSize);
  const SDL_Size coloredSize = $(colored, naturalSize);
  const SDL_Size caretSize = $(caret, naturalSize);

  ck_assert_int_gt(plainSize.w, 0);
  ck_assert_int_eq(plainSize.w, coloredSize.w);
  ck_assert_int_eq(plainSize.h, coloredSize.h);
  ck_assert_int_gt(caretSize.w, plainSize.w);

  // Nothing but escapes is nothing to draw
  Text *empty = $(alloc(Text), initWithText, "^1^7", font);
  const SDL_Size emptySize = $(empty, naturalSize);
  ck_assert_int_eq(0, emptySize.w);

  release(empty);
  release(caret);
  release(colored);
  release(plain);

} END_TEST

/**
 * @brief A Theme with a square `heart` icon registered.
 */
static Theme *themeWithHeart(void) {

  Theme *theme = $(alloc(Theme), init);

  SDL_Surface *surface = SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_RGBA32);
  SDL_FillSurfaceRect(surface, NULL, 0xff0000ff);

  Image *heart = $$(Image, imageWithSurface, surface);
  SDL_DestroySurface(surface);

  ck_assert_ptr_nonnull($(theme, addIcon, "heart", heart));
  release(heart);

  return theme;
}

START_TEST(iconEscapeLength) {

  Theme *theme = themeWithHeart();
  const ImageAtlas *icons = $(theme, icons);

  AtlasImage *image = NULL;
  ck_assert_uint_eq(7, MVC_IconEscapeLength(":heart:", icons, &image));
  ck_assert_ptr_eq($(theme, icon, "heart"), image);
  ck_assert_uint_eq(7, MVC_IconEscapeLength(":heart: 100", icons, NULL));

  ck_assert_uint_eq(0, MVC_IconEscapeLength(":heart:", NULL, &image));
  ck_assert_ptr_null(image);
  ck_assert_uint_eq(0, MVC_IconEscapeLength(":nope:", icons, NULL));
  ck_assert_uint_eq(0, MVC_IconEscapeLength("::", icons, NULL));
  ck_assert_uint_eq(0, MVC_IconEscapeLength(":he art:", icons, NULL));
  ck_assert_uint_eq(0, MVC_IconEscapeLength(":heart", icons, NULL));
  ck_assert_uint_eq(0, MVC_IconEscapeLength("heart:", icons, NULL));

  char longName[80];
  memset(longName, 'a', sizeof(longName));
  longName[0] = longName[65] = ':';
  longName[66] = '\0';
  ck_assert_uint_eq(0, MVC_IconEscapeLength(longName, icons, NULL));

  release(theme);

} END_TEST

START_TEST(layoutTextResolvesIcons) {

  Theme *theme = themeWithHeart();
  const ImageAtlas *icons = $(theme, icons);

  Font *font = $$(Font, defaultFont);
  ck_assert_float_eq(1.f, font->pixelDensity);

  // Without icons, the layout is the escape-stripped text
  char *plain = MVC_LayoutText(font, "^1A:heart:B^^", Colors.White, NULL, NULL, NULL);
  char *stripped = MVC_StripColorEscapes("^1A:heart:B^^");
  ck_assert_str_eq(stripped, plain);
  free(stripped);
  free(plain);

  TextSpan *spans = NULL;
  size_t count = 0;
  char *layout = MVC_LayoutText(font, "^1A:heart:B", Colors.White, icons, &spans, &count);

  // Red "A", the icon's placeholder, red "B"
  ck_assert_uint_eq(3, count);
  ck_assert_ptr_null(spans[0].icon);
  ck_assert_int_eq(0, spans[0].offset);
  ck_assert_int_eq(1, spans[0].length);
  ck_assert_ptr_eq($(theme, icon, "heart"), spans[1].icon);
  ck_assert_int_eq(1, spans[1].offset);
  ck_assert_int_gt(spans[1].length, 0);
  ck_assert_ptr_null(spans[2].icon);
  ck_assert_int_eq(spans[1].offset + spans[1].length, spans[2].offset);
  ck_assert_int_eq((int) strlen(layout), spans[2].offset + spans[2].length);

  // The placeholder is at least a line height wide
  int w, ab;
  $(font, sizeCharacters, layout, &w, NULL);
  $(font, sizeCharacters, "AB", &ab, NULL);
  ck_assert_int_ge(w - ab, (int) (TTF_GetFontHeight(font->font) / font->pixelDensity) - 1);

  free(spans);
  free(layout);
  release(theme);

} END_TEST

START_TEST(bitmapIconsAdvanceWholeCells) {

  Theme *theme = themeWithHeart();
  const ImageAtlas *icons = $(theme, icons);

  Font *font = $$(Font, defaultMonospaceFont);
  ck_assert_ptr_nonnull(font->bitmap.surface);
  ck_assert_float_eq(1.f, font->pixelDensity);

  int literal, icon, glyph;
  $(font, sizeBitmapCharacters, ":heart:", 0, NULL, &literal, NULL);
  $(font, sizeBitmapCharacters, ":heart:", 0, icons, &icon, NULL);
  $(font, sizeBitmapCharacters, "A", 0, NULL, &glyph, NULL);

  // Seven literal glyphs without icons, plus the last glyph's overhang; with them, a slot of
  // whole cells at least a line high, and no glyph overhang
  const int overhang = glyph - font->bitmap.advance;
  ck_assert_int_eq(7 * font->bitmap.advance + overhang, literal);

  const int cells = icon / font->bitmap.advance;
  ck_assert_int_eq(cells * font->bitmap.advance, icon);
  ck_assert_int_ge(icon, font->bitmap.cellSize.h);
  ck_assert_int_lt(icon - font->bitmap.cellSize.h, font->bitmap.advance);

  release(theme);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Text");
  tcase_add_test(tcase, iconEscapeLength);
  tcase_add_test(tcase, layoutTextResolvesIcons);
  tcase_add_test(tcase, bitmapIconsAdvanceWholeCells);
  tcase_add_test(tcase, hasColorEscapes);
  tcase_add_test(tcase, stripColorEscapes);
  tcase_add_test(tcase, escapesDoNotAffectProportionalSize);

  Suite *suite = suite_create("Text");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
