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
#include <stdio.h>

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
static Image *imageWithBytes(const uint8_t *bytes, size_t length, float pixelDensity) {
  return $(alloc(Image), initWithBytes, bytes, length, pixelDensity);
}

/**
 * @fn Image *Image::imageWithData(const Data *data)
 * @memberof Image
 */
static Image *imageWithData(const Data *data, float pixelDensity) {
  return $(alloc(Image), initWithData, data, pixelDensity);
}

/**
 * @fn Image *Image::imageWithResource(const Resource *resource)
 * @memberof Image
 */
static Image *imageWithResource(const Resource *resource, float pixelDensity) {
  return $(alloc(Image), initWithResource, resource, pixelDensity);
}

/**
 * @fn Image *Image::imageWithResourceName(const char *name)
 * @memberof Image
 */
static Image *imageWithResourceName(const char *name, float pixelDensity) {
  return $(alloc(Image), initWithResourceName, name, pixelDensity);
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
static Image *initWithBytes(Image *self, const uint8_t *bytes, size_t length, float pixelDensity) {

  if (isSVG(self, bytes, length)) {
    return $(self, initWithSVG, bytes, length, pixelDensity);
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
static Image *initWithData(Image *self, const Data *data, float pixelDensity) {

  if (data) {
    self = $(self, initWithBytes, data->bytes, data->length, pixelDensity);
  } else {
    self = release(self);
  }

  return self;
}

/**
 * @fn Image *Image::initWithResource(Image *self, const Resource *resource)
 * @memberof Image
 */
static Image *initWithResource(Image *self, const Resource *resource, float pixelDensity) {

  if (resource) {
    self->type = strrchr(resource->name, '.') ? strrchr(resource->name, '.') + 1 : NULL;
    self = $(self, initWithData, resource->data, pixelDensity);
  } else {
    self = release(self);
  }

  return self;
}

/**
 * @brief Interprets an `@` decoration in `name`.
 * @details Where the decoration sits says what it is. `name.svg@<width>x<height>` trails the
 * whole reference and requests a size in points: it is a rasterization request, so it is
 * stripped, and `name.svg` is what loads. `name@<scale>x.ext` sits inside the name, before the
 * extension, and declares how many pixels per point a raster carries, following Apple; a file is
 * named that way on disk, so it is left alone and loaded verbatim.
 * @param name The resource name, edited in place.
 * @param points Receives the requested size, or zero.
 * @param scale Receives the declared pixels per point, or zero.
 */
static void parseDecoration(char *name, SDL_Size *points, float *scale) {

  *points = MakeSize(0, 0);
  *scale = 0.f;

  char *at = strrchr(name, '@');
  if (at == NULL) {
    return;
  }

  char excess;
  int w, h;

  const char *extension = strrchr(name, '.');

  if (extension && at > extension) {

    if (sscanf(at + 1, "%dx%d%c", &w, &h, &excess) == 2 && w > 0 && h > 0) {
      *points = MakeSize(w, h);
      *at = '\0';
    }

    return;
  }

  const size_t length = (extension ? (size_t) (extension - at) : strlen(at)) - 1;

  char decoration[32];
  if (length == 0 || length >= sizeof(decoration)) {
    return;
  }

  memcpy(decoration, at + 1, length);
  decoration[length] = '\0';

  if (sscanf(decoration, "%dx%c", &w, &excess) == 1 && w > 0) {
    *scale = (float) w;
  }
}

/**
 * @brief Composes the name of the `@<density>x` variant of `name`, e.g. `foo@2x.png`.
 * @return The variant name, to be freed by the caller, or `NULL` on error.
 */
static char *variantName(const char *name, int density) {

  const char *extension = strrchr(name, '.');
  const int stem = extension ? (int) (extension - name) : (int) strlen(name);

  char *variant = NULL;
  if (SDL_asprintf(&variant, "%.*s@%dx%s", stem, name, density, extension ?: "") < 0) {
    return NULL;
  }

  return variant;
}

/**
 * @fn Image *Image::initWithResourceName(Image *self, const char *name)
 * @memberof Image
 */
static Image *initWithResourceName(Image *self, const char *name, float pixelDensity) {

  assert(name);

  char *resourceName = strdup(name);
  assert(resourceName);

  SDL_Size points;
  float scale;

  parseDecoration(resourceName, &points, &scale);

  Resource *resource = NULL;

  const char *extension = strrchr(resourceName, '.');

  // an undecorated raster name prefers the variant matching the display, as @2x assets are
  // chosen; a vector needs no variant, rasterizing to the density in initWithBytes instead
  if (points.w == 0 && scale == 0.f && !(extension && SDL_strcasecmp(extension, ".svg") == 0)) {

    const int density = (int) SDL_roundf(pixelDensity);
    if (density > 1) {

      char *variant = variantName(resourceName, density);
      if (variant) {

        resource = $$(Resource, resourceWithName, variant);
        if (resource) {
          scale = (float) density;
        }

        free(variant);
      }
    }
  }

  if (resource == NULL) {
    resource = $$(Resource, resourceWithName, resourceName);
  }

  free(resourceName);

  if (resource == NULL) {
    return release(self);
  }

  if (points.w && points.h && resource->data &&
      isSVG(self, resource->data->bytes, resource->data->length)) {
    self = $(self, initWithSVGSize, resource->data->bytes, resource->data->length, points,
                  pixelDensity);
  } else {
    self = $(self, initWithResource, resource, pixelDensity);

    if (self && scale > 0.f) {
      self->scale = scale;
    }
  }

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
 * @fn Image *Image::initWithSVGSize(Image *self, const uint8_t *bytes, size_t length, SDL_Size points)
 * @memberof Image
 */
static Image *initWithSVGSize(Image *self, const uint8_t *bytes, size_t length, SDL_Size points, float pixelDensity) {

  const SDL_Size pixels = MakeSize(
    (int) SDL_roundf(points.w * pixelDensity),
    (int) SDL_roundf(points.h * pixelDensity)
  );

  SDL_Surface *surface = rasterizeSVG(bytes, length, pixels);
  if (surface) {
    self = $(self, initWithSurface, surface);
    SDL_DestroySurface(surface);

    if (self) {
      self->type = "svg";
      self->scale = pixelDensity;
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
  ((ImageInterface *) clazz->interface)->initWithSVGSize = initWithSVGSize;
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
