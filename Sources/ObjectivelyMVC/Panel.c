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

static Image *_resize;
static const unsigned char _resizeData[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x00,
	0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9,
	0x43, 0xbb, 0x7f, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00,
	0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b, 0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18,
	0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xe0, 0x07, 0x16,
	0x13, 0x18, 0x19, 0xe5, 0xd2, 0x48, 0xc1, 0x00, 0x00, 0x00, 0x1d, 0x69,
	0x54, 0x58, 0x74, 0x43, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x77,
	0x69, 0x74, 0x68, 0x20, 0x47, 0x49, 0x4d, 0x50, 0x64, 0x2e, 0x65, 0x07,
	0x00, 0x00, 0x00, 0x94, 0x49, 0x44, 0x41, 0x54, 0x58, 0xc3, 0xc5, 0xce,
	0xdb, 0x11, 0xc0, 0x20, 0x0c, 0x03, 0x41, 0xd1, 0x59, 0xfa, 0x6f, 0x8a,
	0xfc, 0x65, 0xf2, 0x00, 0x62, 0x1b, 0xc9, 0xbe, 0x06, 0xf6, 0x80, 0xc2,
	0x7a, 0xef, 0x47, 0xab, 0xc4, 0x01, 0xa0, 0x55, 0xe2, 0x25, 0x03, 0x77,
	0x3c, 0x7d, 0xe0, 0x8d, 0xa7, 0x0e, 0x8c, 0xf0, 0xb4, 0x81, 0x19, 0x9e,
	0x32, 0xb0, 0xc2, 0xe5, 0x03, 0x7f, 0xb8, 0x74, 0xc0, 0x82, 0xcb, 0x06,
	0xac, 0xb8, 0x64, 0xc0, 0x83, 0xd3, 0x07, 0xbc, 0x38, 0x75, 0x20, 0x82,
	0xd3, 0x06, 0xa2, 0x38, 0x65, 0x60, 0x07, 0xdf, 0x1e, 0xd8, 0xc5, 0xb7,
	0x06, 0x18, 0x78, 0x78, 0x80, 0x85, 0x87, 0x06, 0x98, 0xb8, 0x7b, 0x80,
	0x8d, 0xbb, 0x06, 0x14, 0xb8, 0x79, 0x40, 0x85, 0x9b, 0x06, 0x94, 0xf8,
	0xef, 0x80, 0x1a, 0x5f, 0x0e, 0x64, 0xe0, 0xd3, 0x81, 0x2c, 0x7c, 0x38,
	0x90, 0x89, 0x7f, 0x06, 0xb2, 0xf1, 0xc7, 0x40, 0x05, 0x7e, 0x0d, 0x54,
	0xe1, 0x00, 0xd0, 0x2a, 0x71, 0x00, 0x38, 0x01, 0xd8, 0xdc, 0x67, 0xc0,
	0xdc, 0x15, 0xb1, 0xea, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
	0xae, 0x42, 0x60, 0x82
};

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

	const Inlet inlets[] = MakeInlets(
		MakeInlet("accessoryView", InletTypeView, &this->accessoryView, NULL),
		MakeInlet("contentView", InletTypeView, &this->contentView, NULL),
		MakeInlet("isDraggable", InletTypeBool, &this->isDraggable, NULL),
		MakeInlet("isResiable", InletTypeBool, &this->isResizable, NULL)
	);

	$(self, bind, dictionary, inlets);
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

	const Panel *this = (Panel *) self;

	const SDL_Size size = $(this, contentSize);

	$((View *) this->contentView, resize, &size);

	super(View, self, layoutSubviews);

	View *resizeHandle = (View *) this->resizeHandle;

	resizeHandle->frame.x = self->frame.w - resizeHandle->frame.w;
	resizeHandle->frame.y = self->frame.h - resizeHandle->frame.h;

	resizeHandle->hidden = !this->isResizable;
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

			SDL_Size size = $(self, size);

			size.w = clamp(size.w + event->motion.xrel, this->minSize.w, this->maxSize.w);
			size.h = clamp(size.h + event->motion.yrel, this->minSize.h, this->maxSize.h);

			$(self, resize, &size);

		} else if (this->isDragging) {
			self->frame.x += event->motion.xrel;
			self->frame.y += event->motion.yrel;
		}
	}
}

#pragma mark - Panel

/**
 * @fn SDL_Size Panel::contentSize(const Panel *self)
 *
 * @memberof Panel
 */
static SDL_Size contentSize(const Panel *self) {

	const View *accessoryView = (View *) self->accessoryView;
	const View *contentView = (View *) self->contentView;

	SDL_Size size = $(contentView, sizeThatContains);

	if (accessoryView->hidden == false) {
		const SDL_Size accessorySize = $(accessoryView, sizeThatContains);
		size.h -= accessorySize.h + self->stackView.spacing;
	}

	return size;
}

/**
 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
 *
 * @memberof Panel
 */
static Panel *initWithFrame(Panel *self, const SDL_Rect *frame) {
	
	self = (Panel *) super(StackView, self, initWithFrame, frame);
	if (self) {

		View *this = (View *) self;

		self->isDraggable = true;
		self->isResizable = true;

		self->maxSize = MakeSize(INT32_MAX, INT32_MAX);

		self->stackView.spacing = DEFAULT_PANEL_SPACING;

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->spacing = DEFAULT_PANEL_SPACING;
		self->contentView->view.autoresizingMask |= ViewAutoresizingWidth;

		$(this, addSubview, (View *) self->contentView);

		self->accessoryView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->accessoryView);

		self->accessoryView->axis = StackViewAxisHorizontal;
		self->accessoryView->spacing = DEFAULT_PANEL_SPACING;
		self->accessoryView->view.alignment = ViewAlignmentMiddleRight;

		self->accessoryView->view.hidden = true;

		$(this, addSubview, (View *) self->accessoryView);

		self->resizeHandle = $(alloc(ImageView), initWithImage, _resize);
		assert(self->resizeHandle);

		self->resizeHandle->view.alignment = ViewAlignmentInternal;

		self->resizeHandle->view.frame.w = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;
		self->resizeHandle->view.frame.h = DEFAULT_PANEL_RESIZE_HANDLE_SIZE;

		$((View *) self, addSubview, (View *) self->resizeHandle);

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

	((PanelInterface *) clazz->interface)->contentSize = contentSize;
	((PanelInterface *) clazz->interface)->initWithFrame = initWithFrame;

	Data *resizeData = $(alloc(Data), initWithBytes, _resizeData, lengthof(_resizeData));
	_resize = $(alloc(Image), initWithData, resizeData);
	release(resizeData);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_resize);
}

Class _Panel = {
	.name = "Panel",
	.superclass = &_StackView,
	.instanceSize = sizeof(Panel),
	.interfaceOffset = offsetof(Panel, interface),
	.interfaceSize = sizeof(PanelInterface),
	.initialize = initialize,
	.destroy = destroy
};

#undef _Class

