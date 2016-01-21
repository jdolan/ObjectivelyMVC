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
 */

#ifndef _ObjectivelyMVC_Image_h_
#define _ObjectivelyMVC_Image_h_

#include <Objectively/Object.h>

#include <SDL2/SDL_surface.h>

/**
 * @file
 * 
 * @brief Images.
 */

typedef struct Image Image;
typedef struct ImageInterface ImageInterface;

/**
 * @brief The Image type.
 *
 * @extends Object
 */
struct Image {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	ImageInterface *interface;

	/**
	 * @brief The backing surface.
	 */
	SDL_Surface *surface;
};

/**
 * @brief The Image interface.
 */
struct ImageInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Image *Image::initWithName(Image *self, const char *name)
	 *
	 * @brief Initializes this Image, loading it from disk by the given name.
	 *
	 * @param name The image name.
	 *
	 * @return The initialized Image, or `NULL` on error.
	 *
	 * @memberof Image
	 */
	Image *(*initWithName)(Image *self, const char *name);

	/**
	 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
	 *
	 * @brief Initializes this Image with the given surface.
	 *
	 * @param surface The backing surface.
	 *
	 * @return The initialized Image, or `NULL` on error.
	 *
	 * @memberof Image
	 */
	Image *(*initWithSurface)(Image *self, SDL_Surface *surface);
};

/**
 * @brief The Image Class.
 */
extern Class _Image;

#endif
