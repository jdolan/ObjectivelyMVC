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

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Text");
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
