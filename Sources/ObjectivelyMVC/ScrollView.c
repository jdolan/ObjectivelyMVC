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

#include <ObjectivelyMVC/ScrollView.h>

#define _Class _ScrollView

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	super(View, self, layoutSubviews);

	ScrollView *this = (ScrollView *) self;

	$(this->contentView, sizeToFit);
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	ScrollView *this = (ScrollView *) self;

	if (event->type == SDL_MOUSEWHEEL) {
		if ($(self, didReceiveEvent, event)) {
			SDL_Point offset = this->contentOffset;

			offset.x += event->wheel.x;
			offset.y += event->wheel.y;

			$(this, scrollToOffset, &offset);
		}
	}

	super(View, self, respondToEvent, event);
}

#pragma mark - ScrollView

/**
 * @fn ScrollView *ScrollView::initWithFrame(ScrollView *self, const SDL_Rect *frame)
 *
 * @memberof ScrollView
 */
static ScrollView *initWithFrame(ScrollView *self, const SDL_Rect *frame) {
	
	self = (ScrollView *) super(View, self, initWithFrame, frame);
	if (self) {
		self->scrollEnabled = true;
	}
	
	return self;
}

/**
 * @brief Scrolls the content View to the specified offset.
 *
 * @memberof ScrollView
 */
static void scrollToOffset(ScrollView *self, const SDL_Point *offset) {

	if (self->contentView) {
		const SDL_Size contentSize = $(self->contentView, size);
		const SDL_Rect bounds = $((View *) self, bounds);

		if (contentSize.w > bounds.w) {
			self->contentOffset.x = min(offset->x, contentSize.w - bounds.w);
		} else {
			self->contentOffset.x = 0;
		}

		if (contentSize.h > bounds.h) {
			self->contentOffset.y = min(offset->y, contentSize.h - bounds.h);
		} else {
			self->contentOffset.y = 0;
		}
	}
}

/**
 * @fn void ScrollView::setContentView(ScrollView *self, View *contentView)
 *
 * @memberof ScrollView
 */
static void setContentView(ScrollView *self, View *contentView) {

	if (self->contentView) {
		$((View *) self, removeSubview, self->contentView);
	}

	if (contentView) {
		$((View *) self, addSubview, contentView);
	}

	self->contentView = contentView;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

	((ScrollViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ScrollViewInterface *) clazz->interface)->scrollToOffset = scrollToOffset;
	((ScrollViewInterface *) clazz->interface)->setContentView = setContentView;
}

Class _ScrollView = {
	.name = "ScrollView",
	.superclass = &_View,
	.instanceSize = sizeof(ScrollView),
	.interfaceOffset = offsetof(ScrollView, interface),
	.interfaceSize = sizeof(ScrollViewInterface),
	.initialize = initialize,
};

#undef _Class
