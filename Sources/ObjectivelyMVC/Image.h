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

#include <ObjectivelyMVC/Types.h>

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
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ImageInterface *interface;

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
	 * @return The new Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*imageWithBytes)(const uint8_t *bytes, size_t length);

	/**
	 * @static
	 * @fn Image *Image::imageWithData(const Data *data)
	 * @brief Instantiates an Image with the specified Data.
	 * @param data The encoded image Data.
	 * @return The new Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*imageWithData)(const Data *data);

	/**
	 * @static
	 * @fn Image *Image::imageWithResource(const Resource *resource)
	 * @brief Instantiates an Image with the specified Resource.
	 * @param resource The Resource containing encoded image data.
	 * @return The new Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*imageWithResource)(const Resource *resource);

	/**
	 * @static
	 * @fn Image *Image::imageWithResourceName(const char *name)
	 * @brief Instantiates an Image with the specified Resource name.
	 * @param name The name of a Resource containing encoded image data.
	 * @return The new Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*imageWithResourceName)(const char *name);

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
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithBytes)(Image *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Image *Image::initWithData(Image *self, const Data *data)
	 * @brief Initializes this Image with the specified Data.
	 * @param self The Image.
	 * @param data The encoded image Data.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithData)(Image *self, const Data *data);

	/**
	 * @fn Image *Image::initWithResource(Image *self, const Resource *resource)
	 * @brief Initializes this Image with the specified Resource.
	 * @param self The Image.
	 * @param resource The Resource containing encoded image data.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithResource)(Image *self, const Resource *resource);

	/**
	 * @fn Image *Image::initWithResourceName(Image *self, const char *name)
	 * @brief Initializes this Image, loading the Resource by the given name.
	 * @param self The Image.
	 * @param name The Resource name.
	 * @return The initialized Image, or `NULL` on error.
	 * @memberof Image
	 */
	Image *(*initWithResourceName)(Image *self, const char *name);

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
