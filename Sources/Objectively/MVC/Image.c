/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <assert.h>

#include <Objectively/MVC/Image.h>

#define _Class _Image

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Image *this = (Image *) self;

	SDL_DestroyTexture(this->texture);

	super(Object, self, dealloc);
}

#pragma mark - ViewInterface

/**
 * @see ViewInterface::draw(View *)
 */
static void draw(View *self) {

	// TODO

	super(View, self, draw);
}

#pragma mark - Image initializers

/**
 * @see ImageInterface::init(Image *)
 */
static Image *initWithFrameAndTexture(Image *self, SDL_Rect *frame, SDL_Texture *texture) {

	self = (Image *) super(View, self, initWithFrame, frame);
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
	((ViewInterface *) clazz->interface)->draw = draw;
	((ImageInterface *) clazz->interface)->initWithFrameAndTexture = initWithFrameAndTexture;
}

Class _Image = {
	.name = "Image",
	.superclass = &_View,
	.instanceSize = sizeof(Image),
	.interfaceOffset = offsetof(Image, interface),
	.interfaceSize = sizeof(ImageInterface),
	.initialize = initialize,
};

#undef _Class
