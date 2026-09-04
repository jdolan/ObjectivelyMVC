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

#include <ObjectivelyMVC/Image.h>

/**
 * @file
 * @brief An Image packed into an ImageAtlas.
 */

typedef struct ImageAtlas ImageAtlas;

typedef struct AtlasImage AtlasImage;
typedef struct AtlasImageInterface AtlasImageInterface;

/**
 * @brief An Image packed into an ImageAtlas.
 * @details An AtlasImage keeps its source surface, so Image::size and anything else that
 * reads `surface` behave as for a standalone Image. What it adds is where those pixels
 * landed in the atlas sheet, so that a View drawing it samples the atlas Texture rather than
 * a Texture of its own, and consecutive draws from the same atlas merge into one call.
 * @extends Image
 */
struct AtlasImage {

  /**
   * @brief The superclass.
   */
  Image image;

  /**
   * @brief The interface type.
   * @protected
   */
  AtlasImageInterface *interface[0];

  /**
   * @brief The ImageAtlas this AtlasImage belongs to, or `NULL` once that atlas has been
   * released, after which this AtlasImage draws nothing.
   * @remarks Weak: the atlas owns its AtlasImages, and clears this on dealloc.
   */
  ImageAtlas *atlas;

  /**
   * @brief This AtlasImage's rectangle within the atlas sheet, in texels.
   * @remarks Assigned by ImageAtlas::compile, and reassigned by every subsequent compile,
   * so it MUST be read at draw time rather than cached.
   */
  SDL_Rect rect;
};

/**
 * @brief The AtlasImage interface.
 */
struct AtlasImageInterface {

  /**
   * @brief The superclass interface.
   */
  ImageInterface imageInterface;

  /**
   * @fn AtlasImage *AtlasImage::initWithImage(AtlasImage *self, ImageAtlas *atlas, Image *image)
   * @brief Initializes this AtlasImage with the given Image's surface.
   * @param self The AtlasImage.
   * @param atlas The ImageAtlas.
   * @param image The Image.
   * @return The initialized AtlasImage, or `NULL` on error.
   * @remarks Use ImageAtlas::addImage; the atlas creates and owns its AtlasImages.
   * @memberof AtlasImage
   */
  AtlasImage *(*initWithImage)(AtlasImage *self, ImageAtlas *atlas, Image *image);

  /**
   * @fn SDL_FRect AtlasImage::texcoords(const AtlasImage *self)
   * @brief Resolves this AtlasImage's normalized texture coordinates within the atlas Texture.
   * @param self The AtlasImage.
   * @return The texture coordinates, or an empty rectangle if the atlas has not been compiled
   * or has been released.
   * @memberof AtlasImage
   */
  SDL_FRect (*texcoords)(const AtlasImage *self);
};

OBJECTIVELYMVC_EXPORT Class *_AtlasImage(void);
