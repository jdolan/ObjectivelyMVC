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

#include <SDL2/SDL_log.h>

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
 * @fn void View::draw(View *self, SDL_Renderer *renderer)
 *
 * @memberof View
 */
static void draw(View *self, SDL_Renderer *renderer) {
	
	if (self->backgroundColor.a) {
		const SDL_Color c = self->backgroundColor;
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		
		SDL_RenderFillRect(renderer, &self->frame);
		
		const SDL_Color w = Colors.white;
		SDL_SetRenderDrawColor(renderer, w.r, w.g, w.b, w.a);
	}
}

/**
 * @see ViewInterface::init(View *, SDL_Rect *)
 */
static View *initWithFrame(View *self, SDL_Rect *frame) {
	
	self = (View *) super(Object, self, init);
	if (self) {
		self->subviews = $$(MutableArray, array);
		
		if (frame) {
			self->frame = *frame;
		} else {
			SDL_LogDebug(0, "%s: NULL frame", __func__);
		}
		
		self->backgroundColor = Colors.clear;
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
 * @fn _Bool View::respondToEvent(View *self, SDL_Event *event)
 *
 * @memberof View
 */
static _Bool respondToEvent(View *self, SDL_Event *event) {
	
	assert(event);
	
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
	view->draw = draw;
	view->initWithFrame = initWithFrame;
	view->removeFromSuperview = removeFromSuperview;
	view->removeSubview = removeSubview;
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
