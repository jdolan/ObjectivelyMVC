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


#include <assert.h>

#include "AtlasImage.h"
#include "ImageAtlas.h"

#define _Class _AtlasImage

#pragma mark - AtlasImage

/**
 * @fn AtlasImage *AtlasImage::initWithImage(AtlasImage *self, ImageAtlas *atlas, Image *image)
 * @memberof AtlasImage
 */
static AtlasImage *initWithImage(AtlasImage *self, ImageAtlas *atlas, Image *image) {

  assert(atlas);
  assert(image);
  assert(image->surface);

  self = (AtlasImage *) super(Image, self, initWithSurface, image->surface);
  if (self) {
    self->atlas = atlas;
  }

  return self;
}

/**
 * @fn SDL_FRect AtlasImage::texcoords(const AtlasImage *self)
 * @memberof AtlasImage
 */
static SDL_FRect texcoords(const AtlasImage *self) {

  const Image *sheet = self->atlas ? self->atlas->image : NULL;
  if (sheet == NULL) {
    return (SDL_FRect) { 0.f, 0.f, 0.f, 0.f };
  }

  const float w = (float) sheet->surface->w, h = (float) sheet->surface->h;

  return (SDL_FRect) {
    self->rect.x / w,
    self->rect.y / h,
    self->rect.w / w,
    self->rect.h / h
  };
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((AtlasImageInterface *) clazz->interface)->initWithImage = initWithImage;
  ((AtlasImageInterface *) clazz->interface)->texcoords = texcoords;
}

/**
 * @fn Class *AtlasImage::_AtlasImage(void)
 * @memberof AtlasImage
 */
Class *_AtlasImage(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "AtlasImage",
      .superclass = _Image(),
      .instanceSize = sizeof(AtlasImage),
      .interfaceSize = sizeof(AtlasImageInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
