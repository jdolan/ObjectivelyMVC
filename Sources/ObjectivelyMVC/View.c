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

#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/View.h>

#define _Class _View

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	View *this = (View *) self;

	$(this, removeFromSuperview);

	$(this->subviews, removeAllObjects);

	release(this->subviews);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @fn void View::addSubview(View *self, View *subview)
 *
 * @memberof View
 */
static void addSubview(View *self, View *subview) {

	assert(subview);
	
	if ($((Array *) self->subviews, indexOfObject, subview) == -1) {
		$(self->subviews, addObject, subview);
		subview->superview = self;
	}
}

/**
 * @fn _Bool View::containsPoint(const View *self, const SDL_Point *point)
 *
 * @memberof View
 */
static _Bool containsPoint(const View *self, const SDL_Point *point) {
	
	const SDL_Rect frame = $(self, renderFrame);
	
	return (_Bool) SDL_PointInRect(point, &frame);
}

/**
 * @brief ArrayEnumerator for draw.
 */
static _Bool drawSubviews(const Array *array, ident obj, ident data) {
	$((View *) obj, draw, (SDL_Renderer *) data); return false;
}

/**
 * @fn void View::draw(View *self, SDL_Renderer *renderer)
 *
 * @memberof View
 */
static void draw(View *self, SDL_Renderer *renderer) {
	
	assert(renderer);
	
	if (self->hidden) {
		return;
	}
	
	$(self, render, renderer);
	
	$((Array *) self->subviews, enumerateObjects, drawSubviews, renderer);
}

/**
 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
 *
 * @memberof View
 */
static View *initWithFrame(View *self, const SDL_Rect *frame) {
	
	self = (View *) super(Object, self, init);
	if (self) {
		self->subviews = $$(MutableArray, array);
		
		if (frame) {
			self->frame = *frame;
		} else {
			String *description = $((Object *) self, description);
			LogDebug("%s: %s with NULL frame", __func__, description->chars);
			release(description);
		}
		
		self->backgroundColor = Colors.backgroundColor;
	}
	
	return self;
}

/**
 * @fn void View::removeFromSuperview(View *self)
 *
 * @memberof View
 */
static void removeFromSuperview(View *self) {
	
	if (self->superview) {
		$(self->superview, removeSubview, self);
	}
}

/**
 * @fn void View::removeSubview(View *self, View *subview)
 *
 * @memberof View
 */
static void removeSubview(View *self, View *subview) {

	assert(subview);
	
	const int index = $((Array *) self->subviews, indexOfObject, subview);
	if (index == -1) {
		$(self->subviews, removeObjectAtIndex, index);
		subview->superview = NULL;
	}
}

/**
 * @fn void View::render(View *self, SDL_Renderer *renderer)
 *
 * @memberof View
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	if (self->backgroundColor.a) {
		SDL_Color c = self->backgroundColor;
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		
		SDL_Rect frame = $(self, renderFrame);
		SDL_RenderFillRect(renderer, &frame);
		
		c = Colors.white;
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	}
}

/**
 * @fn SDL_Rect View::renderFrame(const View *self)
 *
 * @memberof View
 */
static SDL_Rect renderFrame(const View *self) {
	
	SDL_Rect frame = { .w = self->frame.w, .h = self->frame.h };
	
	const View *view = self;
	while (true) {
		
		frame.x += view->frame.x;
		frame.y += view->frame.y;
		
		if (view->superview) {
			view = view->superview;
		} else {
			break;
		}
	}
	
	const int maxWidth = view->frame.w - frame.x;
	if (frame.w > maxWidth || frame.w == 0) {
		frame.w = maxWidth;
	}
	
	const int maxHeight = view->frame.h - frame.y;
	if (frame.h > maxHeight || frame.h == 0) {
		frame.h = maxHeight;
	}
	
	return frame;
}

/**
 * @fn _Bool View::respondToEvent(View *self, const SDL_Event *event)
 *
 * @memberof View
 */
static _Bool respondToEvent(View *self, const SDL_Event *event) {
	
	Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *view = (View *) $(subviews, objectAtIndex, i);
		if ($(view, respondToEvent, event)) {
			return true;
		}
	}
	
	return false;
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	ViewInterface *view = (ViewInterface *) clazz->interface;

	view->addSubview = addSubview;
	view->containsPoint = containsPoint;
	view->draw = draw;
	view->initWithFrame = initWithFrame;
	view->removeFromSuperview = removeFromSuperview;
	view->removeSubview = removeSubview;
	view->render = render;
	view->renderFrame = renderFrame;
	view->respondToEvent = respondToEvent;
}

Class _View = {
	.name = "View",
	.superclass = &_Object,
	.instanceSize = sizeof(View),
	.interfaceOffset = offsetof(View, interface),
	.interfaceSize = sizeof(ViewInterface),
	.initialize = initialize,
};

#undef _Class
