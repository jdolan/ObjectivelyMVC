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

#include <assert.h>

#include <ObjectivelyMVC/ImageView.h>

const EnumName GLBlendNames[] = MakeEnumNames(
	MakeEnumName(GL_CONSTANT_ALPHA),
	MakeEnumName(GL_CONSTANT_COLOR),
	MakeEnumName(GL_DST_ALPHA),
	MakeEnumName(GL_DST_COLOR),
	MakeEnumName(GL_ONE),
	MakeEnumName(GL_ONE_MINUS_DST_ALPHA),
	MakeEnumName(GL_ONE_MINUS_DST_COLOR),
	MakeEnumName(GL_ONE_MINUS_SRC_ALPHA),
	MakeEnumName(GL_ONE_MINUS_SRC_COLOR),
	MakeEnumName(GL_SRC_ALPHA),
	MakeEnumName(GL_SRC_COLOR),
	MakeEnumName(GL_ZERO)
);

#define _Class _ImageView

#pragma mark - Object

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	ImageView *this = (ImageView *) self;

	if (this->image) {
		release(this->image);
	}

	if (this->image_renderer) {

		$(this->image_renderer, destroyTexture, this->texture);
		release(this->image_renderer);
	}

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	ImageView *this = (ImageView *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("blend.src", InletTypeEnum, &this->blend.src, (ident) GLBlendNames),
		MakeInlet("blend.dst", InletTypeEnum, &this->blend.dst, (ident) GLBlendNames),
		MakeInlet("color", InletTypeColor, &this->color, NULL),
		MakeInlet("image", InletTypeImage, &this->image, NULL)
	);

	$(self, bind, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((ImageView *) self, initWithImage, NULL);
}

/**
 * @see ViewInterface::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

	super(View, self, render, renderer);
	
	ImageView *this = (ImageView *) self;

	if (this->texture == 0) {
		if (this->image) {
			this->texture = $(renderer, createTexture, this->image->surface);
			assert(this->texture);
		}
	}

	if (this->texture) {

		// TODO: Actually use self->blend

		$(renderer, setDrawColor, &this->color);
		const SDL_Rect frame = $(self, renderFrame);
		$(renderer, drawTexture, this->texture, &frame);
		$(renderer, setDrawColor, &Colors.White);
	}
}

/**
 * @see View::renderDeviceDidReset(View *)
 */
static void renderDeviceDidReset(View *self) {

	super(View, self, renderDeviceDidReset);

	ImageView *this = (ImageView *) self;

	this->texture = 0;
}

#pragma mark - ImageView

/**
 * @fn ImageView *ImageView::initWithImage(ImageView *self, const SDL_Rect *frame)
 * @memberof ImageView
 */
static ImageView *initWithFrame(ImageView *self, const SDL_Rect *frame) {

	self = (ImageView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->blend.src = GL_SRC_ALPHA;
		self->blend.dst = GL_ONE_MINUS_SRC_ALPHA;

		self->color = Colors.White;
	}

	return self;
}

/**
 * @fn ImageView *ImageView::initWithImage(ImageView *self, Image *image)
 * @memberof ImageView
 */
static ImageView *initWithImage(ImageView *self, Image *image) {
	
	self = (ImageView *) $(self, initWithFrame, NULL);
	if (self) {

		$(self, setImage, image);

		if (self->image) {
			self->view.frame.w = image->surface->w;
			self->view.frame.h = image->surface->h;
		}
	}
	
	return self;
}

/**
 * @fn void ImageView::setImage(ImageView *self, Image *image);
 * @memberof ImageView
 */
static void setImage(ImageView *self, Image *image) {

	release(self->image);

	if (image) {
		self->image = retain(image);
	} else {
		self->image = NULL;
	}

	self->texture = 0;
}

/**
 * @fn void ImageView::setImageWithSurface(ImageView *self, SDL_Surface *surface)
 * @memberof ImageView
 */
static void setImageWithSurface(ImageView *self, SDL_Surface *surface) {

	$(self, setImage, NULL);

	if (surface) {
		Image *image = $(alloc(Image), initWithSurface, surface);
		assert(image);

		$(self, setImage, image);

		release(image);
	}
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->render = render;
	((ViewInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;

	((ImageViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ImageViewInterface *) clazz->def->interface)->initWithImage = initWithImage;
	((ImageViewInterface *) clazz->def->interface)->setImage = setImage;
	((ImageViewInterface *) clazz->def->interface)->setImageWithSurface = setImageWithSurface;
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
