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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/Panel.h>

#define _Class _Panel

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Panel *this = (Panel *) self;

	release(this->resizeHandle);

	super(Object, self, dealloc);
}

#pragma mark - View

static Image *_resize;

/**
 * @return True if a descendant Control received the given event, false otherwise.
 */
static _Bool controlReceivedEvent(const View *view, const SDL_Event *event) {

	Array *subviews = (Array *) view->subviews;
	for (size_t i = 0; i < subviews->count; i++) {

		View *subview = $(subviews, objectAtIndex, i);
		if (controlReceivedEvent(subview, event)) {
			return true;
		}

		if ($((Object *) subview, isKindOfClass, &_Control)) {
			if ($(subview, didReceiveEvent, event)) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	super(View, self, respondToEvent, event);

	switch (event->type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			break;
		default:
			return;
	}

	Panel *this = (Panel *) self;

	if (this->isDragging == false && this->isResizing == false) {
		if (controlReceivedEvent(self, event)) {
			return;
		}
	}

	if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == 1) {
		if (this->isResizable && $((View *) this->resizeHandle, didReceiveEvent, event)) {
			this->isResizing = true;
		} else if (this->isDraggable) {
			this->isDragging = true;
		}
	} else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
		if (this->isResizing) {
			this->isResizing = false;
		} else if (this->isDragging) {
			this->isDragging = false;
		}
	} else if (event->type == SDL_MOUSEMOTION) {
		if (this->isResizing) {

			int w, h;
			$(self, sizeThatFits, &w, &h);

			self->frame.w = clamp(self->frame.w + event->motion.xrel, w, INT32_MAX);
			self->frame.h = clamp(self->frame.h + event->motion.yrel, h, INT32_MAX);

			self->needsLayout = true;
		} else if (this->isDragging) {
			self->frame.x += event->motion.xrel;
			self->frame.y += event->motion.yrel;
		}
	}
}

/**
 * @see View::sizeThatFits(const View *, int *, int *)
 */
static void sizeThatFits(const View *self, int *w, int *h) {

	*w = *h = 0;

	Array *subviews = $(self, visibleSubviews);

	for (size_t i = 0; i < subviews->count; i++) {

		const View *subview = $(subviews, objectAtIndex, i);

		int sw, sh;
		$(subview, sizeThatFits, &sw, &sh);

		*w = max(*w, sw);
		*h = max(*h, sh);
	}

	*w += self->padding.left + self->padding.right;
	*h += self->padding.top + self->padding.bottom;

	release(subviews);
}

#pragma mark - Panel

/**
 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
 *
 * @memberof Panel
 */
static Panel *initWithFrame(Panel *self, const SDL_Rect *frame) {
	
	self = (Panel *) super(View, self, initWithFrame, frame);
	if (self) {

		self->isDraggable = true;
		self->isResizable = true;

		self->resizeHandle = $(alloc(ImageView), initWithImage, _resize);
		assert(self->resizeHandle);

		self->resizeHandle->view.alignment = ViewAlignmentBottomRight;

		self->resizeHandle->view.frame.w = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;
		self->resizeHandle->view.frame.h = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;

		$((View *) self, addSubview, (View *) self->resizeHandle);

		self->view.padding.top = self->view.padding.bottom = DEFAULT_PANEL_PADDING;
		self->view.padding.left = self->view.padding.right = DEFAULT_PANEL_PADDING;

		self->view.backgroundColor = Colors.DefaultColor;
		self->view.borderWidth = 1;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

	((PanelInterface *) clazz->interface)->initWithFrame = initWithFrame;

	_resize = $(alloc(Image), initWithName, "resize.png");
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_resize);
}

Class _Panel = {
	.name = "Panel",
	.superclass = &_View,
	.instanceSize = sizeof(Panel),
	.interfaceOffset = offsetof(Panel, interface),
	.interfaceSize = sizeof(PanelInterface),
	.initialize = initialize,
	.destroy = destroy
};

#undef _Class

