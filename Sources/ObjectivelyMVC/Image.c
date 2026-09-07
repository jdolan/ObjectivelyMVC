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
#include <string.h>

#include <SDL3_image/SDL_image.h>

#include "Image.h"
#include "Log.h"

#define _Class _Image

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Image *this = (Image *) self;

  SDL_DestroySurface(this->surface);

  super(Object, self, dealloc);
}

#pragma mark - Image

/**
 * @fn Image *Image::imageWithBytes(const uint8_t *bytes, size_t length)
 * @memberof Image
 */
static Image *imageWithBytes(const uint8_t *bytes, size_t length) {
  return $(alloc(Image), initWithBytes, bytes, length);
}

/**
 * @fn Image *Image::imageWithData(const Data *data)
 * @memberof Image
 */
static Image *imageWithData(const Data *data) {
  return $(alloc(Image), initWithData, data);
}

/**
 * @fn Image *Image::imageWithResource(const Resource *resource)
 * @memberof Image
 */
static Image *imageWithResource(const Resource *resource) {
  return $(alloc(Image), initWithResource, resource);
}

/**
 * @fn Image *Image::imageWithResourceName(const char *name)
 * @memberof Image
 */
static Image *imageWithResourceName(const char *name) {
  return $(alloc(Image), initWithResourceName, name);
}

/**
 * @fn Image *Image::imageWithSurface(SDL_Surface *surface)
 * @memberof Image
 */
static Image *imageWithSurface(SDL_Surface *surface) {
  return $(alloc(Image), initWithSurface, surface);
}

/**
 * @fn Image *Image::imageWithSVG(const uint8_t *bytes, size_t length, float scale)
 * @memberof Image
 */
static Image *imageWithSVG(const uint8_t *bytes, size_t length, float scale) {
  return $(alloc(Image), initWithSVG, bytes, length, scale);
}

/**
 * @return True if `bytes` are an SVG document, by declared type or by sniffing.
 */
static bool isSVG(const Image *self, const uint8_t *bytes, size_t length) {

  if (self->type && SDL_strcasecmp(self->type, "svg") == 0) {
    return true;
  }

  bool svg = false;

  SDL_IOStream *stream = SDL_IOFromConstMem(bytes, (int) length);
  if (stream) {
    svg = IMG_isSVG(stream);
    SDL_CloseIO(stream);
  }

  return svg;
}

/**
 * @fn Image *Image::initWithBytes(Image *self, const uint8_t *bytes, size_t length)
 * @memberof Image
 */
static Image *initWithBytes(Image *self, const uint8_t *bytes, size_t length) {

  if (isSVG(self, bytes, length)) {
    return $(self, initWithSVG, bytes, length, 1.f);
  }

  SDL_IOStream *stream = SDL_IOFromConstMem(bytes, (int) length);
  if (stream) {
    SDL_Surface *surface = IMG_LoadTyped_IO(stream, 0, self->type);
    if (surface) {
      self = $(self, initWithSurface, surface);
      SDL_DestroySurface(surface);
    } else {
      self = release(self);
    }
    SDL_CloseIO(stream);
  } else {
    self = release(self);
  }

  return self;
}

/**
 * @fn Image *Image::initWithData(Image *self, const Data *data)
 * @memberof Image
 */
static Image *initWithData(Image *self, const Data *data) {

  if (data) {
    self = $(self, initWithBytes, data->bytes, data->length);
  } else {
    self = release(self);
  }

  return self;
}

/**
 * @fn Image *Image::initWithResource(Image *self, const Resource *resource)
 * @memberof Image
 */
static Image *initWithResource(Image *self, const Resource *resource) {

  if (resource) {
    self->type = strrchr(resource->name, '.') ? strrchr(resource->name, '.') + 1 : NULL;
    self = $(self, initWithData, resource->data);
  } else {
    self = release(self);
  }

  return self;
}

/**
 * @fn Image *Image::initWithResourceName(Image *self, const char *name)
 * @memberof Image
 */
static Image *initWithResourceName(Image *self, const char *name) {

  Resource *resource = $$(Resource, resourceWithName, name);

  self = $(self, initWithResource, resource);

  release(resource);

  return self;
}

/**
 * @brief Rasterizes `bytes` as SVG at `size` pixels, or at its intrinsic size when `size` is zero.
 */
static SDL_Surface *rasterizeSVG(const uint8_t *bytes, size_t length, SDL_Size size) {

  SDL_Surface *surface = NULL;

  SDL_IOStream *stream = SDL_IOFromConstMem(bytes, (int) length);
  if (stream) {
    surface = IMG_LoadSizedSVG_IO(stream, size.w, size.h);
    SDL_CloseIO(stream);
  }

  return surface;
}

/**
 * @fn Image *Image::initWithSVG(Image *self, const uint8_t *bytes, size_t length, float scale)
 * @memberof Image
 */
static Image *initWithSVG(Image *self, const uint8_t *bytes, size_t length, float scale) {

  scale = scale > 0.f ? scale : 1.f;

  SDL_Surface *surface = rasterizeSVG(bytes, length, MakeSize(0, 0));

  if (surface && scale != 1.f) {
    const SDL_Size size = MakeSize(
      (int) SDL_roundf(surface->w * scale),
      (int) SDL_roundf(surface->h * scale)
    );

    SDL_DestroySurface(surface);
    surface = rasterizeSVG(bytes, length, size);
  }

  if (surface) {
    self = $(self, initWithSurface, surface);
    SDL_DestroySurface(surface);

    if (self) {
      self->type = "svg";
      self->scale = scale;
    }
  } else {
    MVC_LogWarn("%s\n", SDL_GetError());
    self = release(self);
  }

  return self;
}

/**
 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
 * @memberof Image
 */
static Image *initWithSurface(Image *self, SDL_Surface *surface) {

  self = (Image *) super(Object, self, init);
  if (self) {
    self->scale = 1.f;

    if (surface) {
      if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        self->surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
      } else {
        self->surface = surface;
        self->surface->refcount++;
      }

      assert(self->surface);
    }
  }

  return self;
}

/**
 * @fn SDL_Size Image::size(const Image *self)
 * @memberof Image
 */
static SDL_Size size(const Image *self) {
  return MakeSize(
    (int) SDL_roundf(self->surface->w / self->scale),
    (int) SDL_roundf(self->surface->h / self->scale)
  );
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ImageInterface *) clazz->interface)->imageWithBytes = imageWithBytes;
  ((ImageInterface *) clazz->interface)->imageWithData = imageWithData;
  ((ImageInterface *) clazz->interface)->imageWithResource = imageWithResource;
  ((ImageInterface *) clazz->interface)->imageWithResourceName = imageWithResourceName;
  ((ImageInterface *) clazz->interface)->imageWithSurface = imageWithSurface;
  ((ImageInterface *) clazz->interface)->imageWithSVG = imageWithSVG;
  ((ImageInterface *) clazz->interface)->initWithBytes = initWithBytes;
  ((ImageInterface *) clazz->interface)->initWithData = initWithData;
  ((ImageInterface *) clazz->interface)->initWithResource = initWithResource;
  ((ImageInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
  ((ImageInterface *) clazz->interface)->initWithSurface = initWithSurface;
  ((ImageInterface *) clazz->interface)->initWithSVG = initWithSVG;
  ((ImageInterface *) clazz->interface)->size = size;
}

/**
 * @fn Class *Image::_Image(void)
 * @memberof Image
 */
Class *_Image(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Image",
      .superclass = _Object(),
      .instanceSize = sizeof(Image),
      .interfaceSize = sizeof(ImageInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
