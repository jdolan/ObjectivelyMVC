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

_Bool ViewWindowUsesHighDPI = false;

#define _Class _View

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	View *this = (View *) self;
	
	$(this, removeFromSuperview);

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
 * @fn SDL_Rect View::bounds(const View *self)
 *
 * @memberof View
 */
static SDL_Rect bounds(const View *self) {
	
	SDL_Rect bounds = {
		.x = self->padding.left,
		.y = self->padding.top,
		.w = self->frame.w - self->padding.left - self->padding.right,
		.h = self->frame.h - self->padding.top - self->padding.bottom,
	};
	
	return bounds;
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
 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
 *
 * @memberof View
 */
static _Bool didReceiveEvent(const View *self, const SDL_Event *event) {
	
	if (self->hidden == false) {
		if (event->type == SDL_MOUSEMOTION) {
			const SDL_Point point = { .x = event->motion.x, .y = event->motion.y };
			if ($(self, containsPoint, &point)) {
				return true;
			}
		} else if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
			const SDL_Point point = { .x = event->button.x, .y = event->button.y };
			if ($(self, containsPoint, &point)) {
				return true;
			}
		} else if (event->type == SDL_MOUSEWHEEL) {
			const SDL_Point point = { .x = event->wheel.x, .y = event->wheel.y };
			if ($(self, containsPoint, &point)) {
				return true;
			}
		} else if (event->type == SDL_FINGERDOWN || event->type == SDL_FINGERUP) {
			const SDL_Point point = { .x = event->tfinger.x, .y = event->tfinger.y };
			if ($(self, containsPoint, &point)) {
				return true;
			}
		}
	}
	
	return false;
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
		
		self->backgroundColor = Colors.Clear;
		self->borderColor = Colors.White;
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
	
	return false;
}

/**
 * @brief ArrayEnumerator for layoutIfNeeded recursion.
 */
static _Bool layoutIfNeeded_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, layoutIfNeeded); return false;
}

/**
 * @fn void View::layoutIfNeeded(View *self)
 *
 * @memberof View
 */
static void layoutIfNeeded(View *self) {
	
	if (self->needsLayout) {
		self->needsLayout = false;
		
		$(self, layoutSubviews);
	}
	
	const Array *subviews = (Array *) self->subviews;
	$(subviews, enumerateObjects, layoutIfNeeded_recurse, NULL);
}

/**
 * @fn View::layoutSubviews(View *self)
 *
 * @memberof View
 */
static void layoutSubviews(View *self) {
	
	const SDL_Rect bounds = $(self, bounds);
	
	const Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {
			
		View *subview = (View *) $(subviews, objectAtIndex, i);
		
		if (subview->autoresizingMask & ViewAutoresizingWidth) {
			subview->frame.w = bounds.w;
		}
		
		if (subview->autoresizingMask & ViewAutoresizingHeight) {
			subview->frame.h = bounds.h;
		}
		
		switch (subview->alignment) {
			case ViewAlignmentNone:
				break;
			
			case ViewAlignmentTopLeft:
				subview->frame.x = 0;
				subview->frame.y = 0;
				break;
			case ViewAlignmentTopCenter:
				subview->frame.x = (bounds.w - subview->frame.w) * 0.5;
				subview->frame.y = 0;
				break;
			case ViewAlignmentTopRight:
				subview->frame.x = bounds.w - subview->frame.w;
				subview->frame.y = 0;
				break;
			
			case ViewAlignmentMiddleLeft:
				subview->frame.x = 0;
				subview->frame.y = (bounds.h - subview->frame.h) * 0.5;
				break;
			case ViewAlignmentMiddleCenter:
				subview->frame.x = (bounds.w - subview->frame.w) * 0.5;
				subview->frame.y = (bounds.h - subview->frame.h) * 0.5;
				break;
			case ViewAlignmentMiddleRight:
				subview->frame.x = bounds.w - subview->frame.w;
				subview->frame.y = (bounds.h - subview->frame.h) * 0.5;
				break;
			
			case ViewAlignmentBottomLeft:
				subview->frame.x = 0;
				subview->frame.y = bounds.h - subview->frame.h;
				break;
			case ViewAlignmentBottomCenter:
				subview->frame.x = (bounds.w - subview->frame.w) * 0.5;
				subview->frame.y = bounds.h - subview->frame.h;
				break;
			case ViewAlignmentBottomRight:
				subview->frame.x = bounds.w - subview->frame.w;
				subview->frame.y = bounds.h - subview->frame.h;
				break;
		}
	}
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
	
	if (self->borderWidth && self->borderColor.a) {
		SetRenderDrawColor(renderer, self->borderColor);
		
		SDL_Rect frame = $(self, renderFrame);
		for (int i = 0; i < self->borderWidth; i++) {
			frame.x -= 1;
			frame.y -= 1;
			frame.w += 2;
			frame.h += 2;
			SDL_RenderDrawRect(renderer, &frame);
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
		
		frame.x += superview->frame.x + superview->padding.left;
		frame.y += superview->frame.y + superview->padding.top;
		
		superview = superview->superview;
	}
	
	return frame;
}

/**
 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
 *
 * @memberof View
 */
static void respondToEvent(View *self, const SDL_Event *event) {
	
	Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *subview = (View *) $(subviews, objectAtIndex, i);
		$(subview, respondToEvent, event);
	}
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
	
	$(self, layoutIfNeeded);
	
	Array *subviews = $(self, visibleSubviews);
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *subview = (View *) $(subviews, objectAtIndex, i);
		
		int w, h;
		$(subview, sizeThatFits, &w, &h);
		
		const int sx = subview->frame.x + w + self->padding.left + self->padding.right;
		const int sy = subview->frame.y + h + self->padding.top + self->padding.bottom;

		if (sx > self->frame.w) {
			self->frame.w = sx;
		}
		
		if (sy > self->frame.h) {
			self->frame.h = sy;
		}
	}

	release(subviews);
}

/**
 * @brief ArrayEnumerator for visibleSubviews.
 */
static _Bool visibleSubviews_filter(const Array *array, ident obj, ident data) {
	return ((View *) obj)->hidden == false;
}

/**
 * @fn Array *View::visibleSubviews(const View *self)
 *
 * @memberof View
 */
static Array *visibleSubviews(const View *self) {
	return $((Array *) self->subviews, filterObjects, visibleSubviews_filter, NULL);
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->interface)->bounds = bounds;
	((ViewInterface *) clazz->interface)->containsPoint = containsPoint;
	((ViewInterface *) clazz->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->interface)->draw = draw;
	((ViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ViewInterface *) clazz->interface)->isDescendantOfView = isDescendantOfView;
	((ViewInterface *) clazz->interface)->layoutIfNeeded = layoutIfNeeded;
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->removeFromSuperview = removeFromSuperview;
	((ViewInterface *) clazz->interface)->removeSubview = removeSubview;
	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->renderFrame = renderFrame;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	((ViewInterface *) clazz->interface)->sizeToFit = sizeToFit;
	((ViewInterface *) clazz->interface)->visibleSubviews = visibleSubviews;
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
