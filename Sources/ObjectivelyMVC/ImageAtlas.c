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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ImageAtlas.h"
#include "Log.h"

#define _Class _ImageAtlas

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  ImageAtlas *this = (ImageAtlas *) self;

  // An AtlasImage retained elsewhere (by an ImageView, say) outlives this atlas: sever its
  // back-pointer so that it degrades to drawing nothing rather than dereferencing freed memory
  for (size_t i = 0; i < this->images->count; i++) {
    AtlasImage *image = $(this->images, objectAtIndex, i);
    image->atlas = NULL;
  }

  release(this->texture);
  release(this->image);
  release(this->images);
  release(this->names);

  super(Object, self, dealloc);
}

#pragma mark - ImageAtlas

/**
 * @fn AtlasImage *ImageAtlas::addImage(ImageAtlas *self, Image *image)
 * @memberof ImageAtlas
 */
static AtlasImage *addImage(ImageAtlas *self, Image *image) {

  assert(image);
  assert(!$((Object *) image, isKindOfClass, _AtlasImage()));

  AtlasImage *atlasImage = $(alloc(AtlasImage), initWithImage, self, image);
  assert(atlasImage);

  $(self->images, addObject, atlasImage);
  release(atlasImage);

  return atlasImage;
}

/**
 * @fn AtlasImage *ImageAtlas::addImageWithResourceName(ImageAtlas *self, const char *name)
 * @memberof ImageAtlas
 */
static AtlasImage *addImageWithResourceName(ImageAtlas *self, const char *name) {

  AtlasImage *atlasImage = NULL;

  Image *image = $$(Image, imageWithResourceName, name);
  if (image) {
    atlasImage = $(self, addImage, image);
    release(image);
  } else {
    MVC_LogError("Image %s not found\n", name);
  }

  return atlasImage;
}

/**
 * @fn AtlasImage *ImageAtlas::addImageWithName(ImageAtlas *self, const char *name, Image *image)
 * @memberof ImageAtlas
 */
static AtlasImage *addImageWithName(ImageAtlas *self, const char *name, Image *image) {

  assert(name);

  AtlasImage *atlasImage = $(self, addImage, image);

  $(self->names, setObjectForKeyPath, atlasImage, name);
  self->generation++;

  return atlasImage;
}

/**
 * @fn AtlasImage *ImageAtlas::imageWithName(const ImageAtlas *self, const char *name)
 * @memberof ImageAtlas
 */
static AtlasImage *imageWithName(const ImageAtlas *self, const char *name) {

  assert(name);

  return $(self->names, objectForKeyPath, name);
}

/**
 * @brief Sorts AtlasImages tallest first, then widest, so that shelf rows waste the least.
 */
static int compareImages(const void *a, const void *b) {

  const SDL_Surface *sa = (*(const AtlasImage **) a)->image.surface;
  const SDL_Surface *sb = (*(const AtlasImage **) b)->image.surface;

  if (sb->h != sa->h) {
    return sb->h - sa->h;
  }

  return sb->w - sa->w;
}

/**
 * @brief Lays out the sorted images in shelves on a square of the given size, writing their
 * rects to `rects` rather than to the images, so that a failed attempt leaves them untouched.
 * @return `true` if every image fit, `false` otherwise.
 */
static bool pack(AtlasImage **images, SDL_Rect *rects, size_t count, int size) {

  int x = 0, y = 0, row = 0;

  for (size_t i = 0; i < count; i++) {

    const SDL_Surface *surface = images[i]->image.surface;

    if (x + surface->w > size) {
      x = 0;
      y += row;
      row = 0;
    }

    if (y + surface->h > size) {
      return false;
    }

    rects[i] = MakeRect(x, y, surface->w, surface->h);

    x += surface->w;
    row = max(row, surface->h);
  }

  return true;
}

/**
 * @fn bool ImageAtlas::compile(ImageAtlas *self)
 * @memberof ImageAtlas
 */
static bool compile(ImageAtlas *self) {

  self->generation++;

  const size_t count = self->images->count;
  if (count == 0) {
    return true;
  }

  AtlasImage **images = calloc(count, sizeof(AtlasImage *));
  assert(images);

  SDL_Rect *rects = calloc(count, sizeof(SDL_Rect));
  assert(rects);

  size_t area = 0;
  int size = 1;

  for (size_t i = 0; i < count; i++) {
    images[i] = $(self->images, objectAtIndex, i);

    const SDL_Surface *surface = images[i]->image.surface;
    area += (size_t) surface->w * (size_t) surface->h;
    size = max(size, max(surface->w, surface->h));
  }

  qsort(images, count, sizeof(AtlasImage *), compareImages);

  size = max(size, (int) ceil(sqrt((double) area)));

  int sheetSize = 1;
  while (sheetSize < size) {
    sheetSize <<= 1;
  }

  bool packed = false;
  while (sheetSize <= self->maxSize) {
    if (pack(images, rects, count, sheetSize)) {
      packed = true;
      break;
    }
    sheetSize <<= 1;
  }

  if (packed) {

    for (size_t i = 0; i < count; i++) {
      images[i]->rect = rects[i];
    }

    SDL_Surface *sheet = SDL_CreateSurface(sheetSize, sheetSize, SDL_PIXELFORMAT_RGBA32);
    assert(sheet);

    SDL_FillSurfaceRect(sheet, NULL, 0);

    for (size_t i = 0; i < count; i++) {

      const AtlasImage *image = images[i];
      const SDL_Surface *surface = image->image.surface;
      const size_t pitch = surface->w * SDL_BYTESPERPIXEL(surface->format);

      for (int y = 0; y < surface->h; y++) {
        memcpy((Uint8 *) sheet->pixels + (image->rect.y + y) * sheet->pitch + image->rect.x * SDL_BYTESPERPIXEL(sheet->format),
               (const Uint8 *) surface->pixels + y * surface->pitch,
               pitch);
      }
    }

    release(self->image);
    self->image = $$(Image, imageWithSurface, sheet);
    assert(self->image);

    SDL_DestroySurface(sheet);

    self->texture = release(self->texture);

  } else {
    MVC_LogError("%zu images (%zu texels) do not fit within %dx%d\n", count, area, self->maxSize, self->maxSize);
  }

  free(rects);
  free(images);

  return packed;
}

/**
 * @fn ImageAtlas *ImageAtlas::init(ImageAtlas *self)
 * @memberof ImageAtlas
 */
static ImageAtlas *init(ImageAtlas *self) {

  self = (ImageAtlas *) super(Object, self, init);
  if (self) {

    self->images = $$(Array, array);
    assert(self->images);

    self->names = $$(Dictionary, dictionary);
    assert(self->names);

    self->maxSize = IMAGE_ATLAS_MAX_SIZE;
  }

  return self;
}

/**
 * @fn void ImageAtlas::renderDeviceWillReset(ImageAtlas *self)
 * @memberof ImageAtlas
 */
static void renderDeviceWillReset(ImageAtlas *self) {
  self->texture = release(self->texture);
}

/**
 * @fn Texture *ImageAtlas::texture(ImageAtlas *self, RenderDevice *device)
 * @memberof ImageAtlas
 */
static Texture *texture(ImageAtlas *self, RenderDevice *device) {

  assert(device);

  if (self->texture == NULL && self->image) {
    self->texture = $(device, createTextureFromSurface, self->image->surface, SDL_GPU_TEXTUREUSAGE_SAMPLER, false);
    assert(self->texture);
  }

  return self->texture;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ImageAtlasInterface *) clazz->interface)->addImage = addImage;
  ((ImageAtlasInterface *) clazz->interface)->addImageWithName = addImageWithName;
  ((ImageAtlasInterface *) clazz->interface)->addImageWithResourceName = addImageWithResourceName;
  ((ImageAtlasInterface *) clazz->interface)->compile = compile;
  ((ImageAtlasInterface *) clazz->interface)->imageWithName = imageWithName;
  ((ImageAtlasInterface *) clazz->interface)->init = init;
  ((ImageAtlasInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((ImageAtlasInterface *) clazz->interface)->texture = texture;
}

/**
 * @fn Class *ImageAtlas::_ImageAtlas(void)
 * @memberof ImageAtlas
 */
Class *_ImageAtlas(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "ImageAtlas",
      .superclass = _Object(),
      .instanceSize = sizeof(ImageAtlas),
      .interfaceSize = sizeof(ImageAtlasInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
