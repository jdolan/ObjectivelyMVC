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

#include <ObjectivelyMVC/ImageView.h>

#include <assert.h>


#define _Class _ImageView

#pragma mark - Object

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ImageView *this = (ImageView *) self;

	SDL_DestroyTexture(this->texture);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see ViewInterface::draw(View *, SDL_Renderer *)
 */
static void draw(View *self, SDL_Renderer *renderer) {

	// TODO

	super(View, self, draw, renderer);
}

#pragma mark - ImageView

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
