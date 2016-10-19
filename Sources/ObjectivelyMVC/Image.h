/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#include <SDL2/SDL_video.h>

#include <Objectively/Resource.h>

/**
 * @file
 * @brief Image loading.
 */

typedef struct Image Image;
typedef struct ImageInterface ImageInterface;

/**
 * @brief Image loading.
 * @extends Object
 */
struct Image {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @protected
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
	 * @fn Image *Image::initWithBytes(Image *self, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Image with the specified bytes.
	 * @param self The Image.
	 * @param bytes The raw image bytes.
	 * @param length The length of `bytes`.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithBytes)(Image *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Image *Image::initWithData(Image *self, const Data *data)
	 * @brief Initializes this Image with the specified Data.
	 * @param self The Image.
	 * @param data The raw image data.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithData)(Image *self, const Data *data);

	/**
	 * @fn Image *Image::initWithName(Image *self, const char *name)
	 * @brief Initializes this Image, loading the Resource by the given name.
	 * @param self The Image.
	 * @param name The image name.
	 * @return The initialized Image, or `NULL` on error.
	 * @remarks
	 * @memberof Image
	 */
	Image *(*initWithName)(Image *self, const char *name);

	/**
	 * @fn Image *Image::initWithResource(Image *self, const Resource *resource)
	 * @brief Initializes this Image with the specified Resource.
	 * @param self The Image.
	 * @param data The Resource containing the image data.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithResource)(Image *self, const Resource *resource);

	/**
	 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
	 * @brief Initializes this Image with the given surface.
	 * @param self The Image.
	 * @param surface The backing surface.
	 * @return The initialized Image, or `NULL` on error.
	 * @remarks The surface's reference count is incremented, so that you can (and should) call
	 * `SDL_FreeSurface` when you no longer need it. The Image will also free the surface on
	 * `dealloc`.
	 * @remarks Designated initializer.
	 * @memberof Image
	 */
	Image *(*initWithSurface)(Image *self, SDL_Surface *surface);
};

/**
 * @brief The Image Class.
 */
extern Class _Image;
