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

#pragma once

#include <SDL3/SDL_video.h>

#include <Objectively/Resource.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Image loading.
 */

typedef struct Image Image;
typedef struct ImageInterface ImageInterface;

/**
 * @brief Image loading.
 * @details Raster formats load at their native size. SVG, recognized by type or by sniffing,
 * rasterizes at its intrinsic size times a `scale`, so that a vector asset drawn into a frame
 * of its intrinsic size stays sharp at any pixel density; Image::size reports points.
 * @extends Object
 */
struct Image {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  ImageInterface *interface[0];

  /**
   * @brief Pixels of `surface` per point: `1` for raster images, the requested scale for SVG.
   */
  float scale;

  /**
   * @brief The backing surface.
   */
  SDL_Surface *surface;

  /**
   * @brief The image type, inferred if instantiated with a Resource.
   */
  const char *type;
};

/**
 * @brief The Image interface.
 */
struct ImageInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @static
   * @fn Image *Image::imageWithBytes(const uint8_t *bytes, size_t length)
   * @brief Instantiates an Image with the specified bytes.
   * @param bytes The encoded image bytes.
   * @param length The length of `bytes`.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithBytes)(const uint8_t *bytes, size_t length, float pixelDensity);

  /**
   * @static
   * @fn Image *Image::imageWithData(const Data *data)
   * @brief Instantiates an Image with the specified Data.
   * @param data The encoded image Data.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithData)(const Data *data, float pixelDensity);

  /**
   * @static
   * @fn Image *Image::imageWithResource(const Resource *resource)
   * @brief Instantiates an Image with the specified Resource.
   * @param resource The Resource containing encoded image data.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithResource)(const Resource *resource, float pixelDensity);

  /**
   * @static
   * @fn Image *Image::imageWithResourceName(const char *name)
   * @brief Instantiates an Image with the specified Resource name.
   * @param name The name of a Resource containing encoded image data.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithResourceName)(const char *name, float pixelDensity);

  /**
   * @static
   * @fn Image *Image::imageWithSVG(const uint8_t *bytes, size_t length, float scale)
   * @brief Instantiates an Image by rasterizing the specified SVG.
   * @param bytes The SVG document.
   * @param length The length of `bytes`.
   * @param scale Pixels per point, e.g. the window's pixel density.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithSVG)(const uint8_t *bytes, size_t length, float scale);

  /**
   * @static
   * @fn Image *Image::imageWithSurface(SDL_Surface *surface)
   * @brief Instantiates an Image with the specified surface.
   * @param surface The surface.
   * @return The new Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*imageWithSurface)(SDL_Surface *surface);

  /**
   * @fn Image *Image::initWithBytes(Image *self, const uint8_t *bytes, size_t length)
   * @brief Initializes this Image with the specified bytes.
   * @param self The Image.
   * @param bytes The encoded image bytes.
   * @param length The length of `bytes`.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithBytes)(Image *self, const uint8_t *bytes, size_t length, float pixelDensity);

  /**
   * @fn Image *Image::initWithData(Image *self, const Data *data)
   * @brief Initializes this Image with the specified Data.
   * @param self The Image.
   * @param data The encoded image Data.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithData)(Image *self, const Data *data, float pixelDensity);

  /**
   * @fn Image *Image::initWithResource(Image *self, const Resource *resource)
   * @brief Initializes this Image with the specified Resource.
   * @param self The Image.
   * @param resource The Resource containing encoded image data.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithResource)(Image *self, const Resource *resource, float pixelDensity);

  /**
   * @fn Image *Image::initWithResourceName(Image *self, const char *name)
   * @brief Initializes this Image, loading the Resource by the given name.
   * @details An `@` decoration is interpreted, and where it sits says what it is. Trailing the
   * whole name, `heart.svg@24x24` rasterizes a vector at that size in points; it is a request
   * rather than part of any file's name, so `heart.svg` is what loads. Inside the name, before
   * the extension, `heart@2x.png` declares that a raster carries that many pixels per point,
   * following Apple's convention, and names the file to load. An undecorated raster prefers the
   * variant matching the pixel density where one exists, so `heart.png` loads `heart@2x.png` on
   * a 2x display; an undecorated vector simply rasterizes at the pixel density.
   * @param self The Image.
   * @param name The Resource name.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithResourceName)(Image *self, const char *name, float pixelDensity);

  /**
   * @fn Image *Image::initWithSVG(Image *self, const uint8_t *bytes, size_t length, float scale)
   * @brief Initializes this Image by rasterizing the specified SVG at its intrinsic size times
   * `scale`.
   * @param self The Image.
   * @param bytes The SVG document.
   * @param length The length of `bytes`.
   * @param scale Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithSVG)(Image *self, const uint8_t *bytes, size_t length, float scale);

  /**
   * @fn Image *Image::initWithSVGSize(Image *self, const uint8_t *bytes, size_t length, SDL_Size points)
   * @brief Initializes this Image by rasterizing the specified SVG at `points` times the pixel
   * density, so that Image::size reports `points`.
   * @param self The Image.
   * @param bytes The SVG document.
   * @param length The length of `bytes`.
   * @param points The desired size, in points.
   * @param pixelDensity Pixels per point, e.g. the window's pixel density.
   * @return The initialized Image, or `NULL` on error.
   * @memberof Image
   */
  Image *(*initWithSVGSize)(Image *self, const uint8_t *bytes, size_t length, SDL_Size points, float pixelDensity);

  /**
   * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
   * @brief Initializes this Image with the given surface.
   * @param self The Image.
   * @param surface The backing surface.
   * @return The initialized Image, or `NULL` on error.
   * @remarks The surface's reference count is incremented, so that you can (and should) call
   * `SDL_DestroySurface` when you no longer need it. The Image will also free the surface on
   * `dealloc`.
   * @remarks Designated initializer.
   * @memberof Image
   */
  Image *(*initWithSurface)(Image *self, SDL_Surface *surface);

  /**
   * @fn SDL_Size Image::size(const Image *self)
   * @param self The Image.
   * @return The Image size.
   * @memberof Image
   */
  SDL_Size (*size)(const Image *self);
};

/**
 * @fn Class *Image::_Image(void)
 * @brief The Image archetype.
 * @return The Image Class.
 * @memberof Image
 */
OBJECTIVELYMVC_EXPORT Class *_Image(void);
