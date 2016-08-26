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

#include <ObjectivelyMVC/CollectionItemView.h>

#define _Class _CollectionItemView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	CollectionItemView *this = (CollectionItemView *) self;

	release(this->imageView);
	release(this->text);

	super(Object, self, dealloc);
}

#pragma mark - CollectionItemView

/**
 * @fn CollectionItemView *CollectionItemView::initWithFrame(CollectionItemView *self)
 *
 * @memberof CollectionItemView
 */
static CollectionItemView *initWithFrame(CollectionItemView *self, const SDL_Rect *frame) {
	
	self = (CollectionItemView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->imageView = $(alloc(ImageView), initWithFrame, frame);
		assert(self->imageView);

		self->imageView->view.autoresizingMask = ViewAutoresizingFill;

		$((View *) self, addSubview, (View *) self->imageView);

		self->text = $(alloc(Text), initWithText, NULL, NULL);
		assert(self->text);

		self->text->view.alignment = ViewAlignmentMiddleCenter;

		$((View *) self, addSubview, (View *) self->text);

		self->view.backgroundColor = Colors.Black;
		self->view.backgroundColor.a = 48;

		self->view.borderColor = Colors.SelectedColor;
	}
	
	return self;
}

/**
 * @fn void CollectionItemView::setSelected(CollectionItemView *self, _Bool selected)
 *
 * @memberof CollectionItemView
 */
static void setSelected(CollectionItemView *self, _Bool selected) {

	self->isSelected = selected;
	if (self->isSelected) {
		self->view.borderWidth = 4;
	} else {
		self->view.borderWidth = 0;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((CollectionItemViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((CollectionItemViewInterface *) clazz->interface)->setSelected = setSelected;
}

Class _CollectionItemView = {
	.name = "CollectionItemView",
	.superclass = &_View,
	.instanceSize = sizeof(CollectionItemView),
	.interfaceOffset = offsetof(CollectionItemView, interface),
	.interfaceSize = sizeof(CollectionItemViewInterface),
	.initialize = initialize,
};

#undef _Class

