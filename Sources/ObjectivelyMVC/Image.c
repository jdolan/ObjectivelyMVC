/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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
 * @fn Image *Image::initWithBytes(Image *self, const uint8_t *bytes, size_t length)
 * @memberof Image
 */
static Image *initWithBytes(Image *self, const uint8_t *bytes, size_t length) {

  SDL_IOStream *stream = SDL_IOFromConstMem(bytes, (int) length);
  if (stream) {
    SDL_Surface *surface = IMG_LoadTyped_IO(stream, 0, self->type);
    if (surface) {
      self = $(self, initWithSurface, surface);
      SDL_DestroySurface(surface);
    } else {
      self = release(self);
    }
  } else {
    self = release(self);
  }

  SDL_CloseIO(stream);
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
 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
 * @memberof Image
 */
static Image *initWithSurface(Image *self, SDL_Surface *surface) {

  self = (Image *) super(Object, self, init);
  if (self) {

    if (surface) {
      if (surface->format != SDL_PIXELFORMAT_ABGR8888) {
        self->surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ABGR8888);
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
  return MakeSize(self->surface->w, self->surface->h);
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
  ((ImageInterface *) clazz->interface)->initWithBytes = initWithBytes;
  ((ImageInterface *) clazz->interface)->initWithData = initWithData;
  ((ImageInterface *) clazz->interface)->initWithResource = initWithResource;
  ((ImageInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
  ((ImageInterface *) clazz->interface)->initWithSurface = initWithSurface;
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
      .interfaceOffset = offsetof(Image, interface),
      .interfaceSize = sizeof(ImageInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
