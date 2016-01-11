/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <assert.h>

#include "MVC_image.h"

#define __class __MVC_Image

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	MVC_Image *this = (MVC_Image *) self;

	SDL_DestroyTexture(this->texture);

	super(Object, self, dealloc);
}

#pragma mark - ViewInterface

/**
 * @see ViewInterface::draw(View *)
 */
static void draw(MVC_View *self) {

	// TODO

	super(MVC_View, self, draw);
}

#pragma mark - Image initializers

/**
 * @see ImageInterface::init(Image *)
 */
static MVC_Image *initWithFrameAndTexture(MVC_Image *self, SDL_Rect *frame, SDL_Texture *texture) {

	self = (MVC_Image *) super(MVC_View, self, initWithFrame, frame);
	if (self) {

		self->texture = texture;
		self->alpha = 1.0;

		self->blend.src = GL_SRC_ALPHA;
		self->blend.dst = GL_ONE_MINUS_SRC_ALPHA;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((MVC_ViewInterface *) clazz->interface)->draw = draw;
	((MVC_ImageInterface *) clazz->interface)->initWithFrameAndTexture = initWithFrameAndTexture;
}

Class __MVC_Image = {
	.name = "Image",
	.superclass = &__MVC_View,
	.instanceSize = sizeof(MVC_Image),
	.interfaceOffset = offsetof(MVC_Image, interface),
	.interfaceSize = sizeof(MVC_ImageInterface),
	.initialize = initialize,
};

#undef __class
