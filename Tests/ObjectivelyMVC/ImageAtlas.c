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

#include <check.h>

#include "ObjectivelyMVC.h"

/**
 * @brief An Image of the given size, filled with the given color.
 */
static Image *filledImage(int w, int h, Uint32 rgba) {

  SDL_Surface *surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
  ck_assert_ptr_nonnull(surface);

  SDL_FillSurfaceRect(surface, NULL, rgba);

  Image *image = $$(Image, imageWithSurface, surface);
  SDL_DestroySurface(surface);

  return image;
}

static Uint32 pixelAt(const SDL_Surface *surface, int x, int y) {
  return *(const Uint32 *) ((const Uint8 *) surface->pixels + y * surface->pitch + x * 4);
}

START_TEST(compilePacksWithoutOverlap) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  const SDL_Size sizes[] = { { 16, 32 }, { 8, 8 }, { 24, 12 }, { 32, 32 }, { 5, 7 } };
  AtlasImage *images[lengthof(sizes)];

  for (size_t i = 0; i < lengthof(sizes); i++) {
    Image *image = filledImage(sizes[i].w, sizes[i].h, 0xff000000 | (Uint32) i);
    images[i] = $(atlas, addImage, image);
    ck_assert_ptr_nonnull(images[i]);
    ck_assert_ptr_eq(atlas, images[i]->atlas);
    release(image);
  }

  ck_assert($(atlas, compile));
  ck_assert_ptr_nonnull(atlas->image);

  const SDL_Rect bounds = MakeRect(0, 0, atlas->image->surface->w, atlas->image->surface->h);

  for (size_t i = 0; i < lengthof(sizes); i++) {

    const SDL_Rect rect = images[i]->rect;
    ck_assert_int_eq(sizes[i].w, rect.w);
    ck_assert_int_eq(sizes[i].h, rect.h);

    SDL_Rect clipped;
    ck_assert(SDL_GetRectIntersection(&bounds, &rect, &clipped));
    ck_assert(SDL_RectsEqual(&rect, &clipped));

    for (size_t j = i + 1; j < lengthof(sizes); j++) {
      ck_assert(!SDL_HasRectIntersection(&rect, &images[j]->rect));
    }

    const SDL_FRect texcoords = $(images[i], texcoords);
    ck_assert_float_eq_tol(rect.x / (float) bounds.w, texcoords.x, 1e-6f);
    ck_assert_float_eq_tol(rect.w / (float) bounds.w, texcoords.w, 1e-6f);
  }

  release(atlas);

} END_TEST

START_TEST(compileCopiesPixels) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  Image *red = filledImage(4, 4, 0xff0000ff);
  Image *green = filledImage(4, 4, 0xff00ff00);

  AtlasImage *a = $(atlas, addImage, red);
  AtlasImage *b = $(atlas, addImage, green);

  release(red);
  release(green);

  ck_assert($(atlas, compile));

  const SDL_Surface *sheet = atlas->image->surface;

  ck_assert_uint_eq(0xff0000ff, pixelAt(sheet, a->rect.x, a->rect.y));
  ck_assert_uint_eq(0xff0000ff, pixelAt(sheet, a->rect.x + 3, a->rect.y + 3));
  ck_assert_uint_eq(0xff00ff00, pixelAt(sheet, b->rect.x, b->rect.y));
  ck_assert_uint_eq(0xff00ff00, pixelAt(sheet, b->rect.x + 3, b->rect.y + 3));

  const SDL_Size size = $((Image *) a, size);
  ck_assert_int_eq(4, size.w);
  ck_assert_int_eq(4, size.h);

  release(atlas);

} END_TEST

START_TEST(compileGrowsToFit) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  for (int i = 0; i < 5; i++) {
    Image *image = filledImage(64, 64, 0xffffffff);
    $(atlas, addImage, image);
    release(image);
  }

  ck_assert($(atlas, compile));
  ck_assert_int_eq(256, atlas->image->surface->w);
  ck_assert_int_eq(256, atlas->image->surface->h);

  release(atlas);

} END_TEST

START_TEST(compileFailsBeyondMaxSize) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);
  atlas->maxSize = 64;

  Image *fits = filledImage(64, 64, 0xffffffff);
  Image *overflows = filledImage(1, 1, 0xffffffff);

  $(atlas, addImage, fits);
  $(atlas, addImage, overflows);

  release(fits);
  release(overflows);

  ck_assert(!$(atlas, compile));
  ck_assert_ptr_null(atlas->image);

  release(atlas);

} END_TEST

START_TEST(retainedImageSurvivesAtlas) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);

  Image *image = filledImage(2, 2, 0xffffffff);
  AtlasImage *atlasImage = retain($(atlas, addImage, image));
  release(image);

  ck_assert($(atlas, compile));
  ck_assert_ptr_eq(atlas, atlasImage->atlas);

  release(atlas);

  ck_assert_ptr_null(atlasImage->atlas);

  const SDL_FRect texcoords = $(atlasImage, texcoords);
  ck_assert_float_eq(0.f, texcoords.w);

  release(atlasImage);

} END_TEST

START_TEST(failedCompileLeavesRectsUntouched) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);
  atlas->maxSize = 64;

  Image *small = filledImage(8, 8, 0xffffffff);
  AtlasImage *a = $(atlas, addImage, small);
  release(small);

  ck_assert($(atlas, compile));
  const SDL_Rect before = a->rect;

  Image *huge = filledImage(128, 128, 0xffffffff);
  $(atlas, addImage, huge);
  release(huge);

  ck_assert(!$(atlas, compile));
  ck_assert(SDL_RectsEqual(&before, &a->rect));

  release(atlas);

} END_TEST

START_TEST(namedImages) {

  ImageAtlas *atlas = $(alloc(ImageAtlas), init);
  ck_assert_uint_eq(0, atlas->generation);

  ck_assert_ptr_null($(atlas, imageWithName, "heart"));

  Image *red = filledImage(4, 4, 0xff0000ff);
  AtlasImage *a = $(atlas, addImageWithName, "heart", red);
  release(red);

  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_eq(a, $(atlas, imageWithName, "heart"));
  ck_assert_ptr_null($(atlas, imageWithName, "hearts"));
  ck_assert_uint_eq(1, atlas->generation);

  // Compiling changes layout, so it counts as a generation too
  ck_assert($(atlas, compile));
  ck_assert_uint_eq(2, atlas->generation);

  // Re-registering a name replaces the mapping; the old image stays in the atlas
  Image *green = filledImage(4, 4, 0xff00ff00);
  AtlasImage *b = $(atlas, addImageWithName, "heart", green);
  release(green);

  ck_assert_ptr_eq(b, $(atlas, imageWithName, "heart"));
  ck_assert_int_eq(2, (int) atlas->images->count);

  release(atlas);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("ImageAtlas");
  tcase_add_test(tcase, namedImages);
  tcase_add_test(tcase, compilePacksWithoutOverlap);
  tcase_add_test(tcase, compileCopiesPixels);
  tcase_add_test(tcase, compileGrowsToFit);
  tcase_add_test(tcase, compileFailsBeyondMaxSize);
  tcase_add_test(tcase, retainedImageSurvivesAtlas);
  tcase_add_test(tcase, failedCompileLeavesRectsUntouched);

  Suite *suite = suite_create("ImageAtlas");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
