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

#include <Objectively/Array.h>
#include <Objectively/Dictionary.h>

#include <ObjectivelyGPU/RenderDevice.h>

#include <ObjectivelyMVC/AtlasImage.h>

/**
 * @file
 * @brief Packs many Images into one sheet, backed by one Texture.
 */

#define IMAGE_ATLAS_MAX_SIZE 4096

typedef struct ImageAtlasInterface ImageAtlasInterface;

/**
 * @brief Packs many Images into one sheet, backed by one Texture.
 * @details The Renderer merges consecutive draws that share a Texture into one draw call, so
 * a UI whose images all come from one atlas draws in a handful of calls rather than one per
 * View. Add Images, then compile: every AtlasImage learns where its pixels landed, and the
 * sheet's Texture is created on first request and shared by every View that draws from it.
 *
 * Packing is by shelves: images are sorted by height, then laid out left to right in rows,
 * onto the smallest power-of-two square that holds them, up to `maxSize`. There is no
 * padding: the Renderer samples without mipmaps, so there are no mip levels to bleed across.
 * @extends Object
 */
struct ImageAtlas {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  ImageAtlasInterface *interface[0];

  /**
   * @brief Incremented by every ImageAtlas::addImageWithName and ImageAtlas::compile, so that
   * anything derived from this atlas' contents or layout can tell when it is stale.
   */
  unsigned generation;

  /**
   * @brief The compiled sheet, or `NULL` until ImageAtlas::compile.
   */
  Image *image;

  /**
   * @brief The AtlasImages, in the order added.
   */
  Array *images;

  /**
   * @brief The AtlasImages registered by name via ImageAtlas::addImageWithName.
   * @private
   */
  Dictionary *names;

  /**
   * @brief The largest sheet dimension ImageAtlas::compile will attempt.
   * @details Sheets are power-of-two squares, so this SHOULD be a power of two; a value that
   * is not rounds down to one. Defaults to `IMAGE_ATLAS_MAX_SIZE`, which every SDL_GPU
   * backend supports.
   */
  int maxSize;

  /**
   * @brief The sheet's Texture, created lazily by ImageAtlas::texture.
   * @private
   */
  Texture *texture;
};

/**
 * @brief The ImageAtlas interface.
 */
struct ImageAtlasInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn AtlasImage *ImageAtlas::addImage(ImageAtlas *self, Image *image)
   * @brief Adds the given Image to this atlas.
   * @param self The ImageAtlas.
   * @param image The Image. Its surface is shared with the returned AtlasImage.
   * @return The AtlasImage, owned by this atlas. Its `rect` is not valid until the next
   * ImageAtlas::compile.
   * @memberof ImageAtlas
   */
  AtlasImage *(*addImage)(ImageAtlas *self, Image *image);

  /**
   * @fn AtlasImage *ImageAtlas::addImageWithResourceName(ImageAtlas *self, const char *name)
   * @brief Loads the Image Resource by the given name and adds it to this atlas.
   * @param self The ImageAtlas.
   * @param name The Resource name.
   * @return The AtlasImage, owned by this atlas, or `NULL` if the Resource was not found.
   * @memberof ImageAtlas
   */
  AtlasImage *(*addImageWithResourceName)(ImageAtlas *self, const char *name);

  /**
   * @fn AtlasImage *ImageAtlas::addImageWithName(ImageAtlas *self, const char *name, Image *image)
   * @brief Adds the given Image to this atlas, registered under `name` for ImageAtlas::imageWithName.
   * @details Names are unique: registering a name again replaces the mapping, while the previously
   * registered AtlasImage remains in the atlas.
   * @param self The ImageAtlas.
   * @param name The name.
   * @param image The Image. Its surface is shared with the returned AtlasImage.
   * @return The AtlasImage, owned by this atlas. Its `rect` is not valid until the next
   * ImageAtlas::compile.
   * @memberof ImageAtlas
   */
  AtlasImage *(*addImageWithName)(ImageAtlas *self, const char *name, Image *image);

  /**
   * @fn bool ImageAtlas::compile(ImageAtlas *self)
   * @brief Packs every added Image into a new sheet.
   * @details Every AtlasImage's `rect` is reassigned, and the previous sheet and Texture are
   * released, so a View drawing from this atlas MUST read `rect` and ImageAtlas::texture at
   * draw time rather than caching either. Compiling repacks everything from scratch, so add
   * all Images first and compile once.
   * @param self The ImageAtlas.
   * @return `true` if every Image fit within `maxSize`, `false` otherwise.
   * @memberof ImageAtlas
   */
  bool (*compile)(ImageAtlas *self);

  /**
   * @fn AtlasImage *ImageAtlas::imageWithName(const ImageAtlas *self, const char *name)
   * @param self The ImageAtlas.
   * @param name The name.
   * @return The AtlasImage registered under `name` via ImageAtlas::addImageWithName, or `NULL`.
   * @memberof ImageAtlas
   */
  AtlasImage *(*imageWithName)(const ImageAtlas *self, const char *name);

  /**
   * @fn ImageAtlas *ImageAtlas::init(ImageAtlas *self)
   * @brief Initializes this ImageAtlas.
   * @param self The ImageAtlas.
   * @return The initialized ImageAtlas, or `NULL` on error.
   * @memberof ImageAtlas
   */
  ImageAtlas *(*init)(ImageAtlas *self);

  /**
   * @fn void ImageAtlas::renderDeviceWillReset(ImageAtlas *self)
   * @brief Releases this atlas' Texture ahead of a RenderDevice reset.
   * @details An ImageAtlas is not a View, so nothing calls this on its behalf: every View that
   * draws from an atlas MUST forward its own View::renderDeviceWillReset here. The call is
   * idempotent, so shared atlases are fine.
   * @param self The ImageAtlas.
   * @memberof ImageAtlas
   */
  void (*renderDeviceWillReset)(ImageAtlas *self);

  /**
   * @fn Texture *ImageAtlas::texture(ImageAtlas *self, RenderDevice *device)
   * @brief Resolves the sheet's Texture, creating it on the given device if necessary.
   * @param self The ImageAtlas.
   * @param device The RenderDevice.
   * @return The Texture, owned by this atlas, or `NULL` if the atlas has not been compiled.
   * @memberof ImageAtlas
   */
  Texture *(*texture)(ImageAtlas *self, RenderDevice *device);
};

OBJECTIVELYMVC_EXPORT Class *_ImageAtlas(void);
