/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief ImageView implementation.
 */

extern const EnumName GLBlendNames[];

typedef struct ImageView ImageView;
typedef struct ImageViewInterface ImageViewInterface;

/**
 * @brief The ImageView type.
 * @extends View
 */
struct ImageView {

	/**
	 * @brief The parent.
	 */
	View view;
	
	/**
	 * @brief The typed interface.
	 * @private
	 */
	ImageViewInterface *interface;

	/**
	 * @brief The alpha.
	 */
	GLfloat alpha;

	/**
	 * @brief The blend function.
	 */
	struct {
		GLenum src, dst;
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
	 * @brief The texture.
	 */
	GLuint texture;
};

/**
 * @brief The ImageView interface.
 */
struct ImageViewInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn ImageView *ImageView::initWithImage(ImageView *self, const SDL_Rect *frame)
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
	 * @fn void ImageView::setImageWithSurface(ImageView *self, SDL_Surface *surface)
	 * @brief A convenience method to set this view's Image with a surface.
	 * @param self The ImageView.
	 * @param surface The surface.
	 * @memberof ImageView
	 */
	void (*setImageWithSurface)(ImageView *image, SDL_Surface *surface);
};

extern Class _ImageView;
