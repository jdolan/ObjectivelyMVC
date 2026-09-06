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
#include <string.h>

#include "ObjectivelyMVC.h"

static const char *svg =
  "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"32\" height=\"16\" viewBox=\"0 0 32 16\">"
  "<rect width=\"32\" height=\"16\" fill=\"#ff0000\"/></svg>";

static void assertCenterIsRed(const Image *image) {

  SDL_Surface *surface = image->surface;

  Uint8 r, g, b, a;
  ck_assert(SDL_ReadSurfacePixel(surface, surface->w / 2, surface->h / 2, &r, &g, &b, &a));
  ck_assert_int_eq(255, r);
  ck_assert_int_eq(0, g);
  ck_assert_int_eq(0, b);
  ck_assert_int_eq(255, a);
}

START_TEST(svgLoadsAtIntrinsicSize) {

  Image *image = $$(Image, imageWithBytes, (const uint8_t *) svg, strlen(svg));
  ck_assert_ptr_nonnull(image);

  ck_assert_str_eq("svg", image->type);
  ck_assert_float_eq(1.f, image->scale);
  ck_assert_int_eq(32, image->surface->w);
  ck_assert_int_eq(16, image->surface->h);

  const SDL_Size size = $(image, size);
  ck_assert_int_eq(32, size.w);
  ck_assert_int_eq(16, size.h);

  assertCenterIsRed(image);

  release(image);

} END_TEST

START_TEST(svgRasterizesAtScale) {

  Image *image = $$(Image, imageWithSVG, (const uint8_t *) svg, strlen(svg), 2.f);
  ck_assert_ptr_nonnull(image);

  ck_assert_float_eq(2.f, image->scale);
  ck_assert_int_eq(64, image->surface->w);
  ck_assert_int_eq(32, image->surface->h);

  // Points, not pixels, so a View sized to the image is the intrinsic size
  const SDL_Size size = $(image, size);
  ck_assert_int_eq(32, size.w);
  ck_assert_int_eq(16, size.h);

  assertCenterIsRed(image);

  release(image);

} END_TEST

START_TEST(rasterKeepsUnitScale) {

  SDL_Surface *surface = SDL_CreateSurface(8, 4, SDL_PIXELFORMAT_RGBA32);
  Image *image = $$(Image, imageWithSurface, surface);
  SDL_DestroySurface(surface);

  ck_assert_float_eq(1.f, image->scale);

  const SDL_Size size = $(image, size);
  ck_assert_int_eq(8, size.w);
  ck_assert_int_eq(4, size.h);

  release(image);

} END_TEST

START_TEST(garbageFails) {

  const char *garbage = "not an image";
  Image *image = $$(Image, imageWithSVG, (const uint8_t *) garbage, strlen(garbage), 1.f);
  ck_assert_ptr_null(image);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Image");
  tcase_add_test(tcase, svgLoadsAtIntrinsicSize);
  tcase_add_test(tcase, svgRasterizesAtScale);
  tcase_add_test(tcase, rasterKeepsUnitScale);
  tcase_add_test(tcase, garbageFails);

  Suite *suite = suite_create("Image");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
