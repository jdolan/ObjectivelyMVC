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

#include <ObjectivelyMVC/PageView.h>

#define _Class _PageView

#pragma mark - View

/**
 * @see View::addSubview(View *, View *)
 */
static void addSubview(View *self, View *subview) {

	super(View, self, addSubview, subview);

	PageView *this = (PageView *) self;

	if (this->currentPage == NULL) {
		$(this, setCurrentPage, subview);
	} else {
		subview->hidden = true;
	}
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((PageView *) self, initWithFrame, NULL);
}

/**
 * @see View::removeSubview(View *, View *)
 */
static void removeSubview(View *self, View *subview) {

	PageView *this = (PageView *) self;

	retain(subview);

	super(View, self, removeSubview, subview);

	if (subview == this->currentPage) {
		$(this, setCurrentPage, NULL);
	}

	release(subview);
}

/**
 * @see View::visibleSubviews(const View *)
 */
static Array *visibleSubviews(const View *self) {
	return $$(Array, arrayWithArray, (Array *) self->subviews);
}

#pragma mark - PageView

/**
 * @fn PageView *PageView::initWithFrame(PageView *self, const SDL_Rect *frame)
 * @memberof PageView
 */
static PageView *initWithFrame(PageView *self, const SDL_Rect *frame) {

	self = (PageView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->view.autoresizingMask = ViewAutoresizingContain;

		self->view.padding.top = self->view.padding.bottom = DEFAULT_PAGE_VIEW_PADDING;
		self->view.padding.left = self->view.padding.right = DEFAULT_PAGE_VIEW_PADDING;
	}

	return self;
}

/**
 * @brief ArrayEnumerator for setCurrentPage.
 */
static void setCurrentPage_enumerate(const Array *array, ident obj, ident data) {

	View *subview = obj;

	if (subview == ((PageView *) data)->currentPage) {
		subview->hidden = false;
	} else {
		subview->hidden = true;
	}
}

/**
 * @fn void PageView::setCurrentPage(PageView *self, View *currentPage)
 * @memberof PageView
 */
static void setCurrentPage(PageView *self, View *currentPage) {

	if (currentPage != self->currentPage) {

		const Array *subviews = (Array *) self->view.subviews;

		if (currentPage) {
			self->currentPage = currentPage;
		} else {
			self->currentPage = $(subviews, firstObject);
		}

		$(subviews, enumerateObjects, setCurrentPage_enumerate, self);

		if (self->currentPage) {

			if (self->delegate.didSetCurrentPage) {
				self->delegate.didSetCurrentPage(self);
			}
		}

		self->view.needsLayout = true;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewInterface *) clazz->def->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->removeSubview = removeSubview;
	((ViewInterface *) clazz->def->interface)->visibleSubviews = visibleSubviews;

	((PageViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((PageViewInterface *) clazz->def->interface)->setCurrentPage = setCurrentPage;
}

/**
 * @fn Class *PageView::_PageView(void)
 * @memberof PageView
 */
Class *_PageView(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "PageView";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(PageView);
		clazz.interfaceOffset = offsetof(PageView, interface);
		clazz.interfaceSize = sizeof(PageViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
