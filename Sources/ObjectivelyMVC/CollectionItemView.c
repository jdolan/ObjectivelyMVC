/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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
	release(this->selectionOverlay);
	release(this->text);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const CollectionItemView *this = (CollectionItemView *) self;
	const char *pattern = simpleSelector->pattern;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("selected", pattern) == 0) {
				return this->isSelected;
			}
			break;
		default:
			break;
	}

	return super(View, self, matchesSelector, simpleSelector);
}

#pragma mark - CollectionItemView

/**
 * @fn CollectionItemView *CollectionItemView::initWithFrame(CollectionItemView *self)
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

		self->selectionOverlay = $(alloc(View), initWithFrame, NULL);
		assert(self->selectionOverlay);

		$(self->selectionOverlay, addClassName, "selectionOverlay");

		self->selectionOverlay->autoresizingMask = ViewAutoresizingFill;

		$((View *) self, addSubview, self->selectionOverlay);

		self->view.clipsSubviews = true;
	}

	return self;
}

/**
 * @fn void CollectionItemView::setSelected(CollectionItemView *self, _Bool isSelected)
 * @memberof CollectionItemView
 */
static void setSelected(CollectionItemView *self, _Bool isSelected) {
	self->isSelected = isSelected;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;
	
	((CollectionItemViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((CollectionItemViewInterface *) clazz->interface)->setSelected = setSelected;
}

/**
 * @fn Class *CollectionItemView::_CollectionItemView(void)
 * @memberof CollectionItemView
 */
Class *_CollectionItemView(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "CollectionItemView",
			.superclass = _View(),
			.instanceSize = sizeof(CollectionItemView),
			.interfaceOffset = offsetof(CollectionItemView, interface),
			.interfaceSize = sizeof(CollectionItemViewInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

