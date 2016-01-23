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
	
	if (subview->superview != self) {
		subview->superview = self;

		$(self->subviews, addObject, subview);
		
		self->needsLayout = true;
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
 * @brief ArrayEnumerator for draw recursion.
 */
static _Bool draw_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, draw, (SDL_Renderer *) data); return false;
}

/**
 * @fn void View::draw(View *self, SDL_Renderer *renderer)
 *
 * @memberof View
 */
static void draw(View *self, SDL_Renderer *renderer) {
	
	assert(renderer);
	
	if (self->hidden == false) {
		
		$(self, render, renderer);
		
		$((Array *) self->subviews, enumerateObjects, draw_recurse, renderer);
	}
}

/**
 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
 *
 * @memberof View
 */
static View *initWithFrame(View *self, const SDL_Rect *frame) {
	
	self = (View *) super(Object, self, init);
	if (self) {
		
		if (frame) {
			self->frame = *frame;
		}
		
		self->subviews = $$(MutableArray, array);
		assert(self->subviews);
		
		self->backgroundColor = Colors.BackgroundColor;
		self->borderColor = Colors.ForegroundColor;
	}
	
	return self;
}

/**
 * @fn _Bool View::isDescendantOfView(const View *self, const View *view)
 *
 * @memberof View
 */
static _Bool isDescendantOfView(const View *self, const View *view) {
	
	while (self) {
		if (self == view) {
			return true;
		}
		self = self->superview;
	}
	printf("What the fuck\n");
	
	return false;
}

/**
 * @brief ArrayEnumerator for layoutSubviews autoresizing.
 */
static _Bool layoutSubviews_autoresize(const Array *array, ident obj, ident data) {
	
	View *subview = (View *) obj;
	
	if (subview->autoresizingMask & ViewAutoresizingWidth) {
		subview->frame.w = subview->superview->frame.w;
	}
	
	if (subview->autoresizingMask & ViewAutoResizingHeight) {
		subview->frame.h = subview->superview->frame.h;
	}
	
	return false;
}

/**
 * @brief ArrayEnumerator for layoutSubviews recursion.
 */
static _Bool layoutSubviews_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, layoutSubviews); return false;
}

/**
 * @fn View::layoutSubviews(View *self)
 *
 * @memberof View
 */
static void layoutSubviews(View *self) {
	
	Array *subviews = (Array *) self->subviews;
	
	if (self->needsLayout) {
		self->needsLayout = false;
		
		$(self, sizeToFit);
		
		$(subviews, enumerateObjects, layoutSubviews_autoresize, NULL);
	}
	
	$(subviews, enumerateObjects, layoutSubviews_recurse, NULL);
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
	
	if (subview->superview == self) {
		subview->superview = NULL;
		
		$(self->subviews, removeObject, subview);
		
		self->needsLayout = true;
	}
}

/**
 * @fn void View::render(View *self, SDL_Renderer *renderer)
 *
 * @memberof View
 */
static void render(View *self, SDL_Renderer *renderer) {
	
	if (self->backgroundColor.a) {
		SetRenderDrawColor(renderer, self->backgroundColor);
		
		const SDL_Rect frame = $(self, renderFrame);
		SDL_RenderFillRect(renderer, &frame);
		
		SetRenderDrawColor(renderer, Colors.White);
	}
	
	if (self->borderWidth) {
		SetRenderDrawColor(renderer, self->borderColor);
		
		SDL_Rect frame = $(self, renderFrame);
		for (int i = 0; i < self->borderWidth; i++) {
			SDL_RenderDrawRect(renderer, &frame);
			frame.x += 1;
			frame.y += 1;
			frame.w -= 2;
			frame.h -= 2;
		}
		
		SetRenderDrawColor(renderer, Colors.White);
	}
}

/**
 * @fn SDL_Rect View::renderFrame(const View *self)
 *
 * @memberof View
 */
static SDL_Rect renderFrame(const View *self) {
	
	SDL_Rect frame = self->frame;
	
	const View *superview = self->superview;
	while (superview) {
		
		frame.x += superview->frame.x;
		frame.y += superview->frame.y;
		
		superview = superview->superview;
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

/**
 * @fn void View::sizeThatFits(const View *self, int *w, int *h)
 *
 * @memberof View
 */
static void sizeThatFits(const View *self, int *w, int *h) {
	
	*w = self->frame.w;
	*h = self->frame.h;
}

/**
 * @fn void View::sizeToFit(View *self)
 *
 * @memberof View
 */
static void sizeToFit(View *self) {
	
	$(self, sizeThatFits, &self->frame.w, &self->frame.h);
	
	Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *subview = (View *) $(subviews, objectAtIndex, i);
		
		int w, h;
		$(subview, sizeThatFits, &w, &h);
		
		const int sx = subview->frame.x + w;
		const int sy = subview->frame.y + h;

		if (sx > self->frame.w) {
			self->frame.w = sx;
		}
		
		if (sy > self->frame.h) {
			self->frame.h = sy;
		}
	}
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
	view->isDescendantOfView = isDescendantOfView;
	view->layoutSubviews = layoutSubviews;
	view->removeFromSuperview = removeFromSuperview;
	view->removeSubview = removeSubview;
	view->render = render;
	view->renderFrame = renderFrame;
	view->respondToEvent = respondToEvent;
	view->sizeThatFits = sizeThatFits;
	view->sizeToFit = sizeToFit;
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
