/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <SDL3/SDL_timer.h>

#include "SlideShowView.h"

#define _Class _SlideShowView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	SlideShowView *this = (SlideShowView *) self;

	release(this->images);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

	super(View, self, render, renderer);

	SlideShowView *this = (SlideShowView *) self;

	const Array *images = (Array *) this->images;
	if (images->count == 0) {
		return;
	}

	const Uint64 now = SDL_GetTicks();

	if (this->current->image == NULL) {
		Image *first = $(images, firstObject);
		$(this->current, setImage, first);
		this->fadeEndedAt = now;
		return;
	}

	if (images->count < 2) {
		return;
	}

	if (this->fadeStartedAt) {
		const float frac = (float) (now - this->fadeStartedAt) / (float) this->fadeDuration;
		this->next->color.a = (Uint8) (SDL_min(frac, 1.f) * 255.f);
		if (frac >= 1.f) {
			$(this->current, setImage, this->next->image);
			$(this->next, setImage, NULL);
			this->next->color.a = 0;
			this->fadeStartedAt = 0;
			this->fadeEndedAt = now;
		}
	} else if (now - this->fadeEndedAt >= this->slideDuration) {
		this->index = (this->index + 1) % images->count;
		Image *image = $(images, objectAtIndex, this->index);
		$(this->next, setImage, image);
		this->fadeStartedAt = now;
	}
}

#pragma mark - SlideShowView

/**
 * @fn void SlideShowView::addImage(SlideShowView *self, Image *image)
 * @memberof SlideShowView
 */
static void addImage(SlideShowView *self, Image *image) {

	assert(image);

	$(self->images, addObject, image);
}

/**
 * @fn SlideShowView *SlideShowView::initWithFrame(SlideShowView *self, const SDL_Rect *frame)
 * @memberof SlideShowView
 */
static SlideShowView *initWithFrame(SlideShowView *self, const SDL_Rect *frame) {

	self = (SlideShowView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->images = $(alloc(MutableArray), init);
		assert(self->images);

		self->slideDuration = 5000;
		self->fadeDuration = 1000;

		self->current = $(alloc(ImageView), initWithFrame, NULL);
		assert(self->current);
		self->current->view.autoresizingMask = ViewAutoresizingFill;
		$((View *) self, addSubview, (View *) self->current);
		release(self->current);

		self->next = $(alloc(ImageView), initWithFrame, NULL);
		assert(self->next);
		self->next->view.autoresizingMask = ViewAutoresizingFill;
		self->next->color.a = 0;
		$((View *) self, addSubview, (View *) self->next);
		release(self->next);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->render = render;

	((SlideShowViewInterface *) clazz->interface)->addImage = addImage;
	((SlideShowViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *_SlideShowView(void)
 * @memberof SlideShowView
 */
Class *_SlideShowView(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "SlideShowView",
			.superclass = _View(),
			.instanceSize = sizeof(SlideShowView),
			.interfaceOffset = offsetof(SlideShowView, interface),
			.interfaceSize = sizeof(SlideShowViewInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
