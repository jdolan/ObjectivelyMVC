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

#pragma once

#include <SDL3/SDL_gpu.h>

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief ImageViews render an Image in the context of a View hierarchy.
 */

OBJECTIVELYMVC_EXPORT const EnumName SDLGPUBlendFactorNames[];

typedef struct ImageView ImageView;
typedef struct ImageViewInterface ImageViewInterface;

/**
 * @brief ImageViews render an Image in the context of a View hierarchy.
 * @extends View
 */
struct ImageView {

  /**
   * @brief The superclass.
   */
  View view;

  /**
   * @brief The interface type.
   * @protected
   */
  ImageViewInterface *interface[0];

  /**
   * @brief The blend factors (currently unused; reserved for future per-pipeline support).
   */
  struct {
    SDL_GPUBlendFactor src, dst;
  } blend;

  /**
   * @brief The drawing color.
   */
  SDL_Color color;

  /**
   * @brief The image.
   */
  Image *image;

  /**
   * @brief The Resource name to resolve `image` from, if it was named rather than assigned.
   * @details A named image is resolved against the window's Theme, at the window's pixel
   * density, once this View is in a window, and re-resolved should that density change.
   * Assigning an Image directly clears this, so an explicit image is never overwritten.
   * @private
   */
  char *imageName;

  /**
   * @brief The GPU texture.
   * @details This is the cached upload of image, but it MAY also be assigned directly, with
   * no image set, to present a Texture the caller owns and updates, such as a Framebuffer's.
   */
  Texture *texture;
};

/**
 * @brief The ImageView interface.
 */
struct ImageViewInterface {

  /**
   * @brief The superclass interface.
   */
  ViewInterface viewInterface;

  /**
   * @fn ImageView *ImageView::initWithFrame(ImageView *self, const SDL_Rect *frame)
   * @brief Initializes this ImageView with the specified frame.
   * @param self The ImageView.
   * @param frame The frame.
   * @return The initialized ImageView, or `NULL` on error.
   * @memberof ImageView
   */
  ImageView *(*initWithFrame)(ImageView *self, const SDL_Rect *frame);

  /**
   * @fn ImageView *ImageView::initWithImage(ImageView *self, Image *image)
   * @brief Initializes this ImageView with the specified image.
   * @param self The ImageView.
   * @param image The Image.
   * @return The initialized ImageView, or `NULL` on error.
   * @memberof ImageView
   */
  ImageView *(*initWithImage)(ImageView *self, Image *image);

  /**
   * @fn void ImageView::setImage(ImageView *self, Image *image);
   * @brief Sets the Image for this ImageView.
   * @param self The ImageView.
   * @param image An Image.
   * @memberof ImageView
   */
  void (*setImage)(ImageView *self, Image *image);

  /**
   * @fn void ImageView::setImageWithResource(ImageView *self, const Resource *resource);
   * @brief Sets the Image for this ImageView with the given Resource.
   * @param self The ImageView.
   * @param resource An Image Resource.
   * @memberof ImageView
   */
  void (*setImageWithResource)(ImageView *self, const Resource *resource);

  /**
   * @fn void ImageView::setImageWithResourceName(ImageView *self, const char *name)
   * @brief Names the Image for this ImageView, resolved against the window's Theme at its pixel
   * density. The Image resolves immediately when this View is already in a window, and otherwise
   * on ImageView::didMoveToWindow, so a name may be set before the View is installed.
   * @param self The ImageView.
   * @param name The Resource name, which may carry an `@` decoration (see Image).
   * @memberof ImageView
   */
  void (*setImageWithResourceName)(ImageView *self, const char *name);

  /**
   * @fn void ImageView::setImageWithSurface(ImageView *self, SDL_Surface *surface)
   * @brief A convenience method to set this view's Image with a surface.
   * @param self The ImageView.
   * @param surface The surface.
   * @memberof ImageView
   */
  void (*setImageWithSurface)(ImageView *image, SDL_Surface *surface);
};

OBJECTIVELYMVC_EXPORT Class *_ImageView(void);
