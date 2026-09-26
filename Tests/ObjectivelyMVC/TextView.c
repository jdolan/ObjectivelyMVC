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

static const char **candidates;
static int requests;
static char lastPrefix[64];

/**
 * @brief TextViewDelegate callback answering the candidates that begin with `prefix`.
 */
static Array *completionsForPrefix(TextView *textView, const char *prefix) {

  requests++;
  strncpy(lastPrefix, prefix, sizeof(lastPrefix) - 1);

  Array *completions = $$(Array, array);

  for (const char **c = candidates; *c; c++) {
    if (!strncmp(*c, prefix, strlen(prefix))) {
      String *completion = $$(String, stringWithCharacters, *c);
      $(completions, addObject, completion);
      release(completion);
    }
  }

  return completions;
}

static TextView *textViewWithText(const char *text) {

  TextView *textView = $(alloc(TextView), initWithFrame, NULL);
  textView->delegate.completionsForPrefix = completionsForPrefix;

  $(textView, setAttributedText, text);

  requests = 0;
  memset(lastPrefix, 0, sizeof(lastPrefix));

  return textView;
}

static void keyDown(TextView *textView, SDL_Keycode key, SDL_Keymod mod) {

  SDL_Event event = { .type = SDL_EVENT_KEY_DOWN };
  event.key.key = key;
  event.key.mod = mod;

  $((Control *) textView, captureEvent, &event);
}

static const char *text(const TextView *textView) {
  return textView->attributedText->chars ?: "";
}

START_TEST(tabCyclesWhenSharedPrefixDoesNotExtend) {

  candidates = (const char *[]) { "abc", "abd", "abe", "x", NULL };

  TextView *textView = textViewWithText("ab");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abd", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abe", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));

  keyDown(textView, SDLK_TAB, SDL_KMOD_SHIFT);
  ck_assert_str_eq("abe", text(textView));

  ck_assert_int_eq(1, requests);

  release(textView);

} END_TEST

START_TEST(tabCompletesSharedPrefixThenCycles) {

  candidates = (const char *[]) { "abc", "abd", NULL };

  TextView *textView = textViewWithText("a");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("ab", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abd", text(textView));

  release(textView);

  textView = textViewWithText("a");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("ab", text(textView));

  keyDown(textView, SDLK_TAB, SDL_KMOD_SHIFT);
  ck_assert_str_eq("abd", text(textView));

  release(textView);

  textView = textViewWithText("ab");

  keyDown(textView, SDLK_TAB, SDL_KMOD_SHIFT);
  ck_assert_str_eq("abd", text(textView));

  release(textView);

} END_TEST

START_TEST(tabCompletesBeforeCursor) {

  candidates = (const char *[]) { "abc", NULL };

  TextView *textView = textViewWithText("a xyz");
  textView->position = 1;

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("a", lastPrefix);
  ck_assert_str_eq("abc xyz", text(textView));
  ck_assert_int_eq(3, textView->position);

  release(textView);

} END_TEST

START_TEST(tabAfterLoneCompletionRequestsAgain) {

  candidates = (const char *[]) { "dir/", NULL };

  TextView *textView = textViewWithText("d");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("dir/", text(textView));

  candidates = (const char *[]) { "dir/x", "dir/y", NULL };

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("dir/", lastPrefix);
  ck_assert_str_eq("dir/x", text(textView));
  ck_assert_int_eq(2, requests);

  release(textView);

} END_TEST

START_TEST(tabWithNothingToCompleteChangesNothing) {

  candidates = (const char *[]) { "abc", "x", NULL };

  TextView *textView = textViewWithText("abc");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));
  ck_assert_ptr_null(textView->completions);

  $(textView, setAttributedText, "q");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("q", text(textView));

  textView->delegate.completionsForPrefix = NULL;

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("q", text(textView));
  ck_assert_int_eq(2, requests);

  release(textView);

} END_TEST

START_TEST(editingEndsCycling) {

  candidates = (const char *[]) { "abc", "abd", NULL };

  TextView *textView = textViewWithText("ab");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));

  // Pressing Shift on its own must not end cycling, or Shift+Tab could never cycle backward
  keyDown(textView, SDLK_LSHIFT, 0);
  keyDown(textView, SDLK_TAB, SDL_KMOD_SHIFT);
  ck_assert_str_eq("abd", text(textView));
  ck_assert_int_eq(1, requests);

  keyDown(textView, SDLK_BACKSPACE, 0);
  ck_assert_str_eq("ab", text(textView));
  ck_assert_ptr_null(textView->completions);

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));
  ck_assert_int_eq(2, requests);

  keyDown(textView, SDLK_LEFT, 0);
  ck_assert_ptr_null(textView->completions);

  release(textView);

} END_TEST

START_TEST(sharedPrefixKeepsWholeCharacters) {

  candidates = (const char *[]) { "a\xc3\xa9", "a\xc3\xa8", NULL };

  TextView *textView = textViewWithText("");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("a", text(textView));

  release(textView);

  candidates = (const char *[]) { "a^1x", "a^2x", NULL };

  textView = textViewWithText("");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("a", text(textView));

  release(textView);

} END_TEST

START_TEST(cyclingToEmptyCompletionClearsText) {

  candidates = (const char *[]) { "", "abc", NULL };

  TextView *textView = textViewWithText("");

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("abc", text(textView));

  keyDown(textView, SDLK_TAB, 0);
  ck_assert_str_eq("", text(textView));
  ck_assert_int_eq(0, textView->attributedText->length);
  ck_assert_int_eq(0, textView->position);

  release(textView);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("TextView");
  tcase_add_test(tcase, tabCyclesWhenSharedPrefixDoesNotExtend);
  tcase_add_test(tcase, tabCompletesSharedPrefixThenCycles);
  tcase_add_test(tcase, tabCompletesBeforeCursor);
  tcase_add_test(tcase, tabAfterLoneCompletionRequestsAgain);
  tcase_add_test(tcase, tabWithNothingToCompleteChangesNothing);
  tcase_add_test(tcase, editingEndsCycling);
  tcase_add_test(tcase, sharedPrefixKeepsWholeCharacters);
  tcase_add_test(tcase, cyclingToEmptyCompletionClearsText);

  Suite *suite = suite_create("TextView");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
