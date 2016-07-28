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

	release(this->image);

	glDeleteTextures(1, &this->texture);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see ViewInterface::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

	super(View, self, render, renderer);
	
	ImageView *this = (ImageView *) self;
	
	if (this->texture == 0) {
		this->texture = $(renderer, createTexture, this->image->surface);
	}
	
	assert(this->texture);
	
	const SDL_Rect frame = $(self, renderFrame);
	$(renderer, drawTexture, this->texture, &frame);
}

/**
 * @see View::renderDeviceDidReset(View *)
 */
static void renderDeviceDidReset(View *self) {

	super(View, self, renderDeviceDidReset);

	ImageView *this = (ImageView *) self;

	if (this->texture) {
		glDeleteTextures(1, &this->texture);
		this->texture = 0;
	}
}

#pragma mark - ImageView

/**
 * @fn ImageView *ImageView::initWithImage(ImageView *self, Image *image)
 *
 * @memberof ImageView
 */
static ImageView *initWithImage(ImageView *self, Image *image) {
	
	self = (ImageView *) super(View, self, initWithFrame, NULL);
	if (self) {
		
		self->alpha = 1.0;
		
		self->blend.src = GL_SRC_ALPHA;
		self->blend.dst = GL_ONE_MINUS_SRC_ALPHA;
		
		self->image = retain(image);
		assert(self->image);
		
		self->view.frame.w = image->surface->w;
		self->view.frame.h = image->surface->h;
		
		self->view.backgroundColor = Colors.Clear;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
	
	((ImageViewInterface *) clazz->interface)->initWithImage = initWithImage;
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
