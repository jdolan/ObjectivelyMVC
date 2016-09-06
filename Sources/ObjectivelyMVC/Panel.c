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
#include <ObjectivelyMVC/Panel.h>

#define _Class _Panel

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Panel *this = (Panel *) self;

	release(this->accessoryView);
	release(this->contentView);
	release(this->resizeHandle);

	super(Object, self, dealloc);
}

#pragma mark - View

static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Panel *this = (Panel *) self;

	const Inlet *inlets = MakeInlets(
		MakeInlet("accessoryView", InletTypeSubviews, &this->accessoryView, NULL),
		MakeInlet("contentView", InletTypeSubviews, &this->contentView, NULL),
		MakeInlet("isDraggable", InletTypeBool, &this->isDraggable, NULL),
		MakeInlet("isResiable", InletTypeBool, &this->isResizable, NULL)
	);

	$$(JSONView, applyInlets, self, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Panel *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	super(View, self, layoutSubviews);

	const Panel *this = (Panel *) self;

	View *resizeHandle = (View *) this->resizeHandle;

	resizeHandle->frame.x = self->frame.w - resizeHandle->frame.w;
	resizeHandle->frame.y = self->frame.h - resizeHandle->frame.h;

	resizeHandle->isHidden = !this->isResizable;
}

/**
 * @return True if a descendant Control received the given event, false otherwise.
 */
static _Bool controlReceivedEvent(const View *view, const SDL_Event *event) {

	Array *subviews = $(view, visibleSubviews);

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

	release(subviews);

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
		} else if (this->isDraggable && $(self, didReceiveEvent, event)) {
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

			const SDL_Size size = $(self, sizeThatFits);

			self->frame.w = max(self->frame.w + event->motion.xrel, size.w);
			self->frame.h = max(self->frame.h + event->motion.yrel, size.h);

			self->needsLayout = true;
		} else if (this->isDragging) {
			self->frame.x += event->motion.xrel;
			self->frame.y += event->motion.yrel;
		}
	}
}

#pragma mark - Panel

static Image *_resize;

/**
 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
 *
 * @memberof Panel
 */
static Panel *initWithFrame(Panel *self, const SDL_Rect *frame) {
	
	self = (Panel *) super(View, self, initWithFrame, frame);
	if (self) {

		View *this = (View *) self;

		self->isDraggable = true;
		self->isResizable = true;

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->spacing = DEFAULT_PANEL_STACK_VIEW_SPACING;
		$(this, addSubview, (View *) self->contentView);

		self->accessoryView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->accessoryView);

		self->accessoryView->axis = StackViewAxisHorizontal;
		self->accessoryView->spacing = DEFAULT_PANEL_STACK_VIEW_SPACING;

		$(this, addSubview, (View *) self->accessoryView);

		self->resizeHandle = $(alloc(ImageView), initWithImage, _resize);
		assert(self->resizeHandle);

		self->resizeHandle->view.alignment = ViewAlignmentInternal;

		self->resizeHandle->view.frame.w = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;
		self->resizeHandle->view.frame.h = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;

		$((View *) self, addSubview, (View *) self->resizeHandle);

		this->autoresizingMask = ViewAutoresizingContain;
		
		this->backgroundColor = Colors.DefaultColor;
		this->borderColor = Colors.DarkGray;
		this->borderWidth = 1;

		this->padding.top = this->padding.bottom = DEFAULT_PANEL_PADDING;
		this->padding.left = this->padding.right = DEFAULT_PANEL_PADDING;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->interface)->init = init;
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

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

