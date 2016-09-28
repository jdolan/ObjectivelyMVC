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

#include <ObjectivelyMVC/Config.h>

#include <assert.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <SDL2/SDL_image.h>

#include <Objectively/Config.h>

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/Log.h>

#ifndef PKGDATADIR
 #define PKGDATADIR "."
#endif

#define _Class _Image

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Image *this = (Image *) self;

	SDL_FreeSurface(this->surface);

	super(Object, self, dealloc);
}

#pragma mark - Image

/**
 * @fn Image *Image::initWithData(Image *self, const Data *data)
 *
 * @memberof Image
 */
static Image *initWithData(Image *self, const Data *data) {

	SDL_RWops *ops = SDL_RWFromConstMem(data->bytes, data->length);
	if (ops) {
		self = $(self, initWithSurface, IMG_Load_RW(ops, 0));
		if (self) {
			SDL_FreeSurface(self->surface);
		}
	}
	
	SDL_FreeRW(ops);
	return self;
}

/**
 * @fn Image *Image::initWithName(Image *self, const char *name)
 *
 * @memberof Image
 */
static Image *initWithName(Image *self, const char *name) {

	char *path, *datadir = getenv("OBJECTIVELYMVC_DATA_DIR");
	asprintf(&path, "%s/%s", datadir ? datadir : PKGDATADIR, name);

	MVC_LogDebug("%s\n", path);

	self = $(self, initWithSurface, IMG_Load(path));
	if (self) {
		SDL_FreeSurface(self->surface);
	}

	free(path);
	return self;
}

/**
 * @fn Image *Image::initWithSurface(Image *self, SDL_Surface *surface)
 *
 * @memberof Image
 */
Image *initWithSurface(Image *self, SDL_Surface *surface) {

	self = (Image *) super(Object, self, init);
	if (self) {
		self->surface = surface;
		assert(self->surface);

		self->surface->refcount++;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ImageInterface *) clazz->interface)->initWithData = initWithData;
	((ImageInterface *) clazz->interface)->initWithName = initWithName;
	((ImageInterface *) clazz->interface)->initWithSurface = initWithSurface;
}

Class _Image = {
	.name = "Image",
	.superclass = &_Object,
	.instanceSize = sizeof(Image),
	.interfaceOffset = offsetof(Image, interface),
	.interfaceSize = sizeof(ImageInterface),
	.initialize = initialize,
};

#undef _Class
