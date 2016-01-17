/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <ObjectivelyMVC/ImageView.h>

#include <assert.h>


#define _Class _ImageView

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ImageView *this = (ImageView *) self;

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

#pragma mark - ImageView initializers

/**
 * @see ImageViewInterface::init(ImageView *)
 */
static ImageView *initWithFrameAndTexture(ImageView *self, SDL_Rect *frame, SDL_Texture *texture) {

	self = (ImageView *) super(View, self, initWithFrame, frame);
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
	((ImageViewInterface *) clazz->interface)->initWithFrameAndTexture = initWithFrameAndTexture;
}

Class _ImageView = {
	.name = "ImageView",
	.superclass = &_View,
	.instanceSize = sizeof(ImageView),
	.interfaceOffset = offsetof(ImageView, interface),
	.interfaceSize = sizeof(ImageViewInterface),
	.initialize = initialize,
};

#undef _Class
