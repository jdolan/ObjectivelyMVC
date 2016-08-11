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

Uint32 MVC_EVENT_RENDER_DEVICE_RESET;
Uint32 MVC_EVENT_UPDATE_BINDINGS;

static View *_firstResponder;

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
 * @fn void View::becomeFirstResponder(View *self)
 *
 * @memberof View
 */
static void becomeFirstResponder(View *self) {
	_firstResponder = self;
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
 * @fn _Bool View::canBecomeFirstResponder(const View *self)
 *
 * @memberof View
 */
static _Bool canBecomeFirstResponder(const View *self) {

	if (_firstResponder == NULL) {
		return true;
	}

	const View *view = self;
	while (view) {

		if ($(view, isFirstResponder)) {
			return true;
		}

		view = view->superview;
	}

	return false;
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
 * @fn int View::depth(const View *self)
 *
 * @memberof View
 */
static int depth(const View *self) {

	return self->zIndex + (self->superview ? $(self->superview, depth) + 1 : 0);
}

/**
 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
 *
 * @memberof View
 */
static _Bool didReceiveEvent(const View *self, const SDL_Event *event) {

	if ($(self, isVisible)) {

		if ($(self, canBecomeFirstResponder)) {

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
			} else if (event->type == SDL_FINGERDOWN || event->type == SDL_FINGERUP) {
				const SDL_Point point = { .x = event->tfinger.x, .y = event->tfinger.y };
				if ($(self, containsPoint, &point)) {
					return true;
				}
			}
		}
	}

	return false;
}

/**
 * @brief ArrayEnumerator for draw recursion.
 */
static _Bool draw_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, draw, (Renderer *) data); return false;
}

/**
 * @fn void View::draw(View *self, Renderer *renderer)
 *
 * @memberof View
 */
static void draw(View *self, Renderer *renderer) {
	
	assert(renderer);
	
	if (self->hidden == false) {

		$(renderer, addView, self);

		$((Array *) self->subviews, enumerateObjects, draw_recurse, renderer);
	}
}

/**
 * @fn View *View::firstResponder(void)
 *
 * @memberof View
 */
static View *firstResponder(void) {
	return _firstResponder;
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
 * @fn _Bool View::isFirstResponder(const View *self)
 *
 * @memberof View
 */
static _Bool isFirstResponder(const View *self) {
	return _firstResponder == self;
}

/**
 * @fn _Bool View::isVisible(const View *self)
 *
 * @memberof View
 */
static _Bool isVisible(const View *self) {

	for (const View *view = self; view; view = view->superview) {
		if (view->hidden) {
			return false;
		}
	}

	return true;
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
			case ViewAlignmentInternal:
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
 * @fn void View::render(View *self, Renderer *renderer)
 *
 * @memberof View
 */
static void render(View *self, Renderer *renderer) {
	
	if (self->backgroundColor.a) {

		SetColor(self->backgroundColor);
		
		const SDL_Rect frame = $(self, renderFrame);
		$(renderer, fillRect, &frame);
	}
	
	if (self->borderWidth && self->borderColor.a) {

		SetColor(self->borderColor);
		
		SDL_Rect frame = $(self, renderFrame);
		for (int i = 0; i < self->borderWidth; i++) {
			frame.x -= 1;
			frame.y -= 1;
			frame.w += 2;
			frame.h += 2;
			$(renderer, drawRect, &frame);
		}
	}

	SetColor(Colors.White);
}

/**
 * @fn void View::renderDeviceDidReset(View *self)
 *
 * @memberof View
 */
static void renderDeviceDidReset(View *self) {

}

/**
 * @fn SDL_Rect View::renderFrame(const View *self)
 *
 * @memberof View
 */
static SDL_Rect renderFrame(const View *self) {
	
	SDL_Rect frame = self->frame;

	const View *view = self;
	const View *superview = view->superview;
	while (superview) {
		
		frame.x += superview->frame.x;
		frame.y += superview->frame.y;

		if (view->alignment != ViewAlignmentInternal) {
			frame.x += superview->padding.left;
			frame.y += superview->padding.top;
		}
		
		view = superview;
		superview = view->superview;
	}
	
	return frame;
}

/**
 * @fn void View::resignFirstResponder(View *self)
 *
 * @memberof View
 */
static void resignFirstResponder(View *self) {

	if ($(self, isFirstResponder)) {
		_firstResponder = NULL;
	}
}

/**
 * @brief ArrayEnumerator for respondToEvent recursion.
 */
static _Bool respondToEvent_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, respondToEvent, (const SDL_Event *) data); return false;
}

/**
 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
 *
 * @memberof View
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	assert(event);
	
	if (event->type == MVC_EVENT_RENDER_DEVICE_RESET) {
		$(self, renderDeviceDidReset);
	} else if (event->type == MVC_EVENT_UPDATE_BINDINGS) {
		$(self, updateBindings);
	}

	$((Array *) self->subviews, enumerateObjects, respondToEvent_recurse, (ident) event);
}

/**
 * @fn SDL_Size View::size(const View *self)
 *
 * @memberof View
 */
static SDL_Size size(const View *self) {
	return MakeSize(self->frame.w, self->frame.h);
}

/**
 * @fn void View::sizeThatFits(const View *self)
 *
 * @memberof View
 */
static SDL_Size sizeThatFits(const View *self) {

	SDL_Size size = $(self, size);

	if (self->autoresizingMask & ViewAutoresizingContain) {
		size = MakeSize(0, 0);

		Array *subviews = $(self, visibleSubviews);

		for (size_t i = 0; i < subviews->count; i++) {

			const View *subview = $(subviews, objectAtIndex, i);

			if (subview->alignment != ViewAlignmentInternal) {

				SDL_Size subviewSize = $(subview, sizeThatFits);

				size.w = max(size.w, subview->frame.x + subviewSize.w);
				size.h = max(size.h, subview->frame.y + subviewSize.h);
			}
		}

		size.w += self->padding.left + self->padding.right;
		size.h += self->padding.top + self->padding.bottom;
		
		release(subviews);
	}

	return size;
}

/**
 * @fn void View::sizeToFit(View *self)
 *
 * @memberof View
 */
static void sizeToFit(View *self) {

	const SDL_Size size = $(self, sizeThatFits);

	self->frame.w = size.w;
	self->frame.h = size.h;

	$(self, layoutIfNeeded);
	
	Array *subviews = $(self, visibleSubviews);
	for (size_t i = 0; i < subviews->count; i++) {
		
		const View *subview = (View *) $(subviews, objectAtIndex, i);

		if (subview->alignment != ViewAlignmentInternal) {

			const SDL_Size size = $(subview, sizeThatFits);

			const int sx = subview->frame.x + size.w + self->padding.left + self->padding.right;
			const int sy = subview->frame.y + size.h + self->padding.top + self->padding.bottom;

			self->frame.w = max(self->frame.w, sx);
			self->frame.h = max(self->frame.h, sy);
		}
	}

	release(subviews);
}

/**
 * @fn void View::updateBindings(View *self)
 *
 * @memberof View
 */
static void updateBindings(View *self) {

}

/**
 * @fn SDL_Rect View::viewport(const View *self)
 *
 * @memberof View
 */
static SDL_Rect viewport(const View *self) {

	SDL_Window *window = $(self, window);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	int dw, dh;
	SDL_GL_GetDrawableSize(window, &dw, &dh);

	SDL_Rect viewport = $(self, renderFrame);

	const float xScale = dw / (float) w;
	const float yScale = dh / (float) h;

	viewport.x *= xScale;
	viewport.y *= yScale;
	viewport.w *= xScale;
	viewport.h *= yScale;

	viewport.y = dh - viewport.y - viewport.h;

	return viewport;
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

/**
 * @fn SDL_Window *View::window(const View *self)
 *
 * @memberof View
 */
static SDL_Window *window(const View *self) {

	SDL_Window *window = SDL_GL_GetCurrentWindow();
	assert(window);

	return window;
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->interface)->becomeFirstResponder = becomeFirstResponder;
	((ViewInterface *) clazz->interface)->bounds = bounds;
	((ViewInterface *) clazz->interface)->canBecomeFirstResponder = canBecomeFirstResponder;
	((ViewInterface *) clazz->interface)->containsPoint = containsPoint;
	((ViewInterface *) clazz->interface)->depth = depth;
	((ViewInterface *) clazz->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->interface)->draw = draw;
	((ViewInterface *) clazz->interface)->firstResponder = firstResponder;
	((ViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ViewInterface *) clazz->interface)->isDescendantOfView = isDescendantOfView;
	((ViewInterface *) clazz->interface)->isFirstResponder = isFirstResponder;
	((ViewInterface *) clazz->interface)->isVisible = isVisible;
	((ViewInterface *) clazz->interface)->layoutIfNeeded = layoutIfNeeded;
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->removeFromSuperview = removeFromSuperview;
	((ViewInterface *) clazz->interface)->removeSubview = removeSubview;
	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((ViewInterface *) clazz->interface)->renderFrame = renderFrame;
	((ViewInterface *) clazz->interface)->resignFirstResponder = resignFirstResponder;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
	((ViewInterface *) clazz->interface)->size = size;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	((ViewInterface *) clazz->interface)->sizeToFit = sizeToFit;
	((ViewInterface *) clazz->interface)->updateBindings = updateBindings;
	((ViewInterface *) clazz->interface)->viewport = viewport;
	((ViewInterface *) clazz->interface)->visibleSubviews = visibleSubviews;
	((ViewInterface *) clazz->interface)->window = window;

	MVC_EVENT_RENDER_DEVICE_RESET = SDL_RegisterEvents(1);
	MVC_EVENT_UPDATE_BINDINGS = SDL_RegisterEvents(1);
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
