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
#include <string.h>

#include <Objectively.h>

#include <ObjectivelyMVC.h>

#define MVC_FIRST_RESPONDER "firstResponder"

const EnumName ViewAlignmentNames[] = MakeEnumNames(
	MakeEnumName(ViewAlignmentNone),
	MakeEnumName(ViewAlignmentTopLeft),
	MakeEnumName(ViewAlignmentTopCenter),
	MakeEnumName(ViewAlignmentTopRight),
	MakeEnumName(ViewAlignmentMiddleLeft),
	MakeEnumName(ViewAlignmentMiddleCenter),
	MakeEnumName(ViewAlignmentMiddleRight),
	MakeEnumName(ViewAlignmentBottomLeft),
	MakeEnumName(ViewAlignmentBottomCenter),
	MakeEnumName(ViewAlignmentBottomRight),
	MakeEnumName(ViewAlignmentInternal)
);

const EnumName ViewAutoresizingNames[] = MakeEnumNames(
	MakeEnumName(ViewAutoresizingNone),
	MakeEnumName(ViewAutoresizingWidth),
	MakeEnumName(ViewAutoresizingHeight),
	MakeEnumName(ViewAutoresizingFill),
	MakeEnumName(ViewAutoresizingContain)
);

static __thread Outlet *_outlets;

#define _Class _View

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	View *this = (View *) self;

	release(this->constraints);

	free(this->identifier);

	$(this, removeFromSuperview);

	release(this->subviews);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	View *this = (View *) self;

	const SDL_Rect *f = &this->frame;

	return str("%s@%p (%d,%d) %dx%d", this->identifier ?: self->clazz->name, self, f->x, f->y, f->w, f->h);
}

#pragma mark - View

/**
 * @fn _Bool View::acceptsFirstResponder(const View *self)
 * @memberof View
 */
static _Bool acceptsFirstResponder(const View *self) {
	return false;
}

/**
 * @fn void View::addConstraint(View *self, Constraint *constraint)
 * @memberof View
 */
static void addConstraint(View *self, Constraint *constraint) {

	$(self->constraints, addObject, constraint);

	self->needsLayout = true;
	self->needsApplyConstraints = true;
}

/**
 * @fn void View::addConstraintWithDescriptor(View *self, const char *descriptor)
 * @memberof View
 */
static void addConstraintWithDescriptor(View *self, const char *descriptor) {

	Constraint *constraint = $(alloc(Constraint), initWithDescriptor, descriptor);
	assert(constraint);

	$(self, addConstraint, constraint);
	release(constraint);
}

/**
 * @fn void View::addSubview(View *self, View *subview)
 * @memberof View
 */
static void addSubview(View *self, View *subview) {
	$(self, addSubviewRelativeTo, subview, NULL, ViewPositionAfter);
}

/**
 * @fn void View::addSubviewRelativeTo(View *self, View *subview, View *other, ViewPosition position)
 * @memberof View
 */
static void addSubviewRelativeTo(View *self, View *subview, View *other, ViewPosition position) {

	assert(subview);
	assert(subview != other);

	retain(subview);

	$(subview, removeFromSuperview);

	if (other && other->superview == self) {

		const Array *subviews = (Array *) self->subviews;
		const ssize_t index = $(subviews, indexOfObject, other);

		if (position == ViewPositionAfter) {
			if (index == subviews->count - 1) {
				$(self->subviews, addObject, subview);
			} else {
				$(self->subviews, insertObjectAtIndex, subview, index + 1);
			}
		} else {
			$(self->subviews, insertObjectAtIndex, subview, index);
		}
	} else {
		$(self->subviews, addObject, subview);
	}

	release(subview);

	subview->superview = self;

	$(subview, setWindow, self->window);

	self->needsLayout = true;
}

/**
 * @fn View::ancestorWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *ancestorWithIdentifier(const View *self, const char *identifier) {

	assert(identifier);

	View *view = self->superview;
	while (view) {
		if (view->identifier) {
			if (strcmp(identifier, view->identifier) == 0) {
				return view;
			}
		}
		view = view->superview;
	}

	return NULL;
}

/**
 * @brief Comparator for applyConstraints sorting.
 */
static Order applyConstraints_comparator(const ident obj1, const ident obj2) {
	return ((Constraint *) obj1)->priority - ((Constraint *) obj2)->priority;
}

/**
 * @fn void View::applyConstraints(View *self)
 * @memberof View
 */
static void applyConstraints(View *self) {

	$(self->constraints, sort, applyConstraints_comparator);

	const Array *constraints = (Array *) self->constraints;
	for (size_t i = 0; i < constraints->count; i++) {

		const Constraint *constraint = $(constraints, objectAtIndex, i);
		if (constraint->enabled) {
			$(constraint, apply, self);
		}
	}
}

/**
 * @brief ArrayEnumerator for applyConstraints recursion.
 */
static void applyConstraintsIfNeeded_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, applyConstraintsIfNeeded);
}

/**
 * @fn void View::applyConstraintsIfNeeded(View *self)
 * @memberof View
 */
static void applyConstraintsIfNeeded(View *self) {

	if (self->needsApplyConstraints) {
		$(self, applyConstraints);
	}

	self->needsApplyConstraints = false;

	$((Array *) self->subviews, enumerateObjects, applyConstraintsIfNeeded_recurse, NULL);
}

/**
 * @fn void Viem::awakeWithDictionary(View *self, const Dictionary *dictionary, Outlet *outlets)
 * @memberof View
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	const Inlet inlets[] = MakeInlets(
		MakeInlet("identifier", InletTypeCharacters, &self->identifier, NULL),
		MakeInlet("alignment", InletTypeEnum, &self->alignment, (ident) ViewAlignmentNames),
		MakeInlet("autoresizingMask", InletTypeEnum, &self->autoresizingMask, (ident) ViewAutoresizingNames),
		MakeInlet("backgroundColor", InletTypeColor, &self->backgroundColor, NULL),
		MakeInlet("borderColor", InletTypeColor, &self->borderColor, NULL),
		MakeInlet("borderWidth", InletTypeInteger, &self->borderWidth, NULL),
		MakeInlet("constraints", InletTypeConstraints, &self, NULL),
		MakeInlet("frame", InletTypeRectangle, &self->frame, NULL),
		MakeInlet("hidden", InletTypeBool, &self->hidden, NULL),
		MakeInlet("padding", InletTypeRectangle, &self->padding, NULL),
		MakeInlet("subviews", InletTypeSubviews, &self, NULL)
	);

	$(self, bind, inlets, dictionary);

	if (self->identifier) {
		for (Outlet *outlet = _outlets; outlet->identifier; outlet++) {
			if (strcmp(outlet->identifier, self->identifier) == 0) {
				*outlet->view = self;
			}
		}
	}
}

/**
 * @fn void View::becomeFirstResponder(View *self)
 * @memberof View
 */
static void becomeFirstResponder(View *self) {

	if (self->window) {
		MVC_MakeFirstResponder(self->window, self);
	} else {
		MVC_LogWarn("%s: window is NULL\n", (self->identifier ?: self->object.clazz->name));
	}
}

/**
 * @fn void View::bind(View *self, const Inlet *inlets, const Dictionary *dictionary)
 * @memberof View
 */
static void _bind(View *self, const Inlet *inlets, const Dictionary *dictionary) {

	if (inlets) {
		bindInlets(inlets, dictionary);
	}

	$(self, updateBindings);
}

/**
 * @fn SDL_Rect View::bounds(const View *self)
 * @memberof View
 */
static SDL_Rect bounds(const View *self) {

	const SDL_Size size = $(self, size);

	const SDL_Rect bounds = {
		.x = self->padding.left,
		.y = self->padding.top,
		.w = size.w - self->padding.left - self->padding.right,
		.h = size.h - self->padding.top - self->padding.bottom,
	};

	return bounds;
}

/**
 * @fn void View::bringSubviewToFront(View *self, View *subview)
 * @memberof View
 */
static void bringSubviewToFront(View *self, View *subview) {

	assert(subview);

	if (subview->superview == self) {

		View *last = $((Array *) self->subviews, lastObject);
		if (last != subview) {
			$(self, addSubviewRelativeTo, subview, last, ViewPositionAfter);
		}
	}
}

/**
 * @fn SDL_Rect View::clippingFrame(const View *self)
 * @memberof View
 */
static SDL_Rect clippingFrame(const View *self) {

	SDL_Rect frame = $(self, renderFrame);

	if (self->borderWidth && self->borderColor.a) {
		for (int i = 0; i < self->borderWidth; i++) {
			frame.x -= 1;
			frame.y -= 1;
			frame.w += 2;
			frame.h += 2;
		}
	}

	const View *superview = self->superview;
	while (superview) {
		if (superview->clipsSubviews) {
			const SDL_Rect clippingFrame = $(superview, clippingFrame);
			if (SDL_IntersectRect(&clippingFrame, &frame, &frame) == false) {

				if (MVC_LogEnabled(SDL_LOG_PRIORITY_VERBOSE)) {
					String *desc = $((Object *) self, description);
					String *superdesc = $((Object *) superview, description);

					MVC_LogVerbose("%s is clipped by %s\n", desc->chars, superdesc->chars);

					release(desc);
					release(superdesc);
				}

				frame.w = frame.h = 0;
				break;
			}
		}
		superview = superview->superview;
	}

	return frame;
}

/**
 * @fn _Bool View::containsPoint(const View *self, const SDL_Point *point)
 * @memberof View
 */
static _Bool containsPoint(const View *self, const SDL_Point *point) {

	const SDL_Rect frame = $(self, clippingFrame);

	return (_Bool) SDL_PointInRect(point, &frame);
}

/**
 * @fn void View::createConstraint(View *self, const char *descriptor)
 * @memberof View
 */
static void createConstraint(View *self, const char *descriptor) {

	Constraint *constraint = $(alloc(Constraint), initWithDescriptor, descriptor);

	$(self, addConstraint, constraint);

	release(constraint);
}

/**
 * @fn int View::depth(const View *self)
 * @memberof View
 */
static int depth(const View *self) {
	return (self->superview ? $(self->superview, depth) + 1 : 0);
}

/**
 * @fn View::descendantWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *descendantWithIdentifier(const View *self, const char *identifier) {

	assert(identifier);

	const Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {

		View *view = $(subviews, objectAtIndex, i);
		if (view->identifier) {
			if (strcmp(identifier, view->identifier)) {
				return view;
			}
		}

		view = $(view, descendantWithIdentifier, identifier);
		if (view) {
			return view;
		}
	}

	return NULL;
}

/**
 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
 * @memberof View
 */
static _Bool didReceiveEvent(const View *self, const SDL_Event *event) {

	if ($(self, isVisible)) {

		SDL_Point point;

		if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
			point = MakePoint(event->button.x, event->button.y);
		} else if (event->type == SDL_MOUSEMOTION) {
			point = MakePoint(event->motion.x, event->motion.y);
		} else if (event->type == SDL_MOUSEWHEEL) {
			SDL_GetMouseState(&point.x, &point.y);
		} else {
			return false;
		}

		return $(self, containsPoint, &point);
	}

	return false;
}

/**
 * @fn Array *View::draw(View *self)
 * @memberof View
 */
static void draw(View *self, Renderer *renderer) {

	assert(self->window);

	if (self->hidden == false) {
		$(renderer, drawView, self);

		const Array *subviews = (Array *) self->subviews;
		for (size_t i = 0; i < subviews->count; i++) {
			View *subview = $(subviews, objectAtIndex, i);
			if ($(subview, isFirstResponder) == false) {
				$(subview, draw, renderer);
			}
		}
	}
}

/**
 * @fn View *View::hitTest(const View *self, const SDL_Point *point)
 * @memberof View
 */
static View *hitTest(const View *self, const SDL_Point *point) {

	if (self->hidden == false) {

		if ($(self, containsPoint, point)) {

			const Array *subviews = (Array *) self->subviews;
			for (size_t i = subviews->count; i; i--) {

				const View *subview = $(subviews, objectAtIndex, i - 1);
				const View *view = $(subview, hitTest, point);
				if (view) {
					return (View *) view;
				}
			}

			return (View *) self;
		}
	}

	return NULL;
}

/**
 * @fn View *View::init(View *self)
 * @memberof View
 */
static View *init(View *self) {
	return $(self, initWithFrame, NULL);
}

/**
 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
 * @memberof View
 */
static View *initWithFrame(View *self, const SDL_Rect *frame) {

	self = (View *) super(Object, self, init);
	if (self) {

		if (frame) {
			self->frame = *frame;
		}

		self->constraints = $$(MutableArray, array);
		assert(self->constraints);

		self->subviews = $$(MutableArray, array);
		assert(self->subviews);

		self->backgroundColor = Colors.Clear;
		self->borderColor = Colors.White;
	}

	return self;
}

/**
 * @fn _Bool View::isDescendantOfView(const View *self, const View *view)
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
 * @memberof View
 */
static _Bool isFirstResponder(const View *self) {

	if (self->window) {
		return MVC_FirstResponder(self->window) == self;
	} else {
		return false;
	}
}

/**
 * @fn _Bool View::isVisible(const View *self)
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
static void layoutIfNeeded_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, layoutIfNeeded);
}

/**
 * @fn void View::layoutIfNeeded(View *self)
 * @memberof View
 */
static void layoutIfNeeded(View *self) {

	if (self->needsLayout) {
		$(self, layoutSubviews);
	}

	self->needsLayout = false;

	$(self, applyConstraintsIfNeeded);

	$((Array *) self->subviews, enumerateObjects, layoutIfNeeded_recurse, NULL);
}

/**
 * @fn View::layoutSubviews(View *self)
 * @memberof View
 */
static void layoutSubviews(View *self) {

	if (self->autoresizingMask & ViewAutoresizingContain) {
		$(self, sizeToContain);
	}

	const SDL_Rect bounds = $(self, bounds);

	const Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {

		View *subview = (View *) $(subviews, objectAtIndex, i);

		SDL_Size subviewSize = $(subview, sizeThatContains);

		if (subview->autoresizingMask & ViewAutoresizingWidth) {
			subviewSize.w = bounds.w;
		}

		if (subview->autoresizingMask & ViewAutoresizingHeight) {
			subviewSize.h = bounds.h;
		}

		$(subview, resize, &subviewSize);

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
				subview->frame.x = bounds.w -subview->frame.w;
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
 * @brief ArrayEnumerator for removeAllConstraints.
 */
static void removeAllConstraints_enumerate(const Array *array, ident obj, ident data) {
	$((View *) data, removeConstraint, obj);
}

/**
 * @fn void View::removeAllConstraints(View *self)
 * @memberof View
 */
static void removeAllConstraints(View *self) {
	$((Array *) self->constraints, enumerateObjects, removeAllConstraints_enumerate, self);
}

/**
 * @brief ArrayEnumerator for removeAllSubviews.
 */
static void removeAllSubviews_enumerate(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, obj);
}

/**
 * @fn void View::removeAllSubviews(View *self)
 * @memberof View
 */
static void removeAllSubviews(View *self) {
	$((Array *) self->subviews, enumerateObjects, removeAllSubviews_enumerate, self);
}

/**
 * @fn void View::removeConstraint(View *self, Constraint *constraint)
 * @memberof View
 */
static void removeConstraint(View *self, Constraint *constraint) {

	$(self->constraints, removeObject, constraint);

	self->needsApplyConstraints = true;
}

/**
 * @fn void View::removeFromSuperview(View *self)
 * @memberof View
 */
static void removeFromSuperview(View *self) {

	if (self->superview) {
		$(self->superview, removeSubview, self);
	}
}

/**
 * @fn void View::removeSubview(View *self, View *subview)
 * @memberof View
 */
static void removeSubview(View *self, View *subview) {

	assert(subview);

	if (subview->superview == self) {

		$(self->subviews, removeObject, subview);

		subview->superview = NULL;

		$(subview, setWindow, NULL);

		self->needsLayout = true;
	}
}

/**
 * @fn void View::render(View *self, Renderer *renderer)
 * @memberof View
 */
static void render(View *self, Renderer *renderer) {

	assert(self->window);
	
	if (self->backgroundColor.a) {

		$(renderer, setDrawColor, &self->backgroundColor);

		const SDL_Rect frame = $(self, renderFrame);
		$(renderer, drawRectFilled, &frame);
	}

	if (self->borderWidth && self->borderColor.a) {

		$(renderer, setDrawColor, &self->borderColor);

		SDL_Rect frame = $(self, renderFrame);
		for (int i = 0; i < self->borderWidth; i++) {
			frame.x -= 1;
			frame.y -= 1;
			frame.w += 2;
			frame.h += 2;
			$(renderer, drawRect, &frame);
		}
	}

	$(renderer, setDrawColor, &Colors.White);
}

/**
 * @brief ArrayEnumerator for renderDeviceDidReset recursion.
 */
static void renderDeviceDidReset_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, renderDeviceDidReset);
}

/**
 * @fn void View::renderDeviceDidReset(View *self)
 * @memberof View
 */
static void renderDeviceDidReset(View *self) {
	$((Array *) self->subviews, enumerateObjects, renderDeviceDidReset_recurse, NULL);
}

/**
 * @fn SDL_Rect View::renderFrame(const View *self)
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
 * @fn void View::replaceSubview(View *self, View *subview, View *replacement)
 * @memberof View
 */
static void replaceSubview(View *self, View *subview, View *replacement) {

	assert(subview);
	assert(replacement);

	$(self, addSubviewRelativeTo, replacement, subview, ViewPositionAfter);
	$(self, removeSubview, subview);
}

/**
 * @fn void View::resignFirstResponder(View *self)
 * @memberof View
 */
static void resignFirstResponder(View *self) {

	if ($(self, isFirstResponder)) {
		MVC_MakeFirstResponder(self->window, NULL);
	}
}

/**
 * @brief ArrayEnumerator for resize recursion.
 */
static void resize_recurse(const Array *array, ident obj, ident data) {

	View *subview = (View *) obj;

	SDL_Size size = $(subview, size);

	if (subview->autoresizingMask & ViewAutoresizingWidth) {
		size.w = 0;
	}

	if (subview->autoresizingMask & ViewAutoresizingHeight) {
		size.h = 0;
	}

	if (subview->autoresizingMask == ViewAutoresizingContain) {
		size = MakeSize(0, 0);
	}

	$(subview, resize, &size);
}

/**
 * @fn void View::resize(View *self, const SDL_Size *size)
 * @memberof View
 */
static void resize(View *self, const SDL_Size *size) {

	if (self->frame.w != size->w || self->frame.h != size->h) {

		self->frame.w = size->w;
		self->frame.h = size->h;

		self->needsLayout = true;

		$((Array *) self->subviews, enumerateObjects, resize_recurse, NULL);
	}
}

/**
 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
 * @memberof View
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	assert(event);

	if (self->viewController) {
		$(self->viewController, respondToEvent, event);
	}

	if (self->superview) {
		$(self->superview, respondToEvent, event);
	}
}

/**
 * @brief ArrayEnumerator for setWindow recursion.
 */
static void setWindow_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, setWindow, data);
}

/**
 * @fn void View::setWindow(View *self, SDL_Window *window)
 * @memberof View
 */
static void setWindow(View *self, SDL_Window *window) {

	$(self, resignFirstResponder);

	self->window = window;

	if (self->window && self->superview == NULL) {
		if (self->autoresizingMask & ViewAutoresizingFill) {

			SDL_Size size;
			SDL_GetWindowSize(self->window, &size.w, &size.h);

			$(self, resize, &size);
		}
	}

	$((Array *) self->subviews, enumerateObjects, setWindow_recurse, window);
}

/**
 * @fn SDL_Size View::size(const View *self)
 * @memberof View
 */
static SDL_Size size(const View *self) {
	return MakeSize(self->frame.w, self->frame.h);
}

/**
 * @fn SDL_Size View::sizeThatContains(const View *self)
 * @memberof View
 */
static SDL_Size sizeThatContains(const View *self) {

	const SDL_Size size = $(self, size);
	const SDL_Size sizeThatFits = $(self, sizeThatFits);

	return MakeSize(max(size.w, sizeThatFits.w), max(size.h, sizeThatFits.h));
}

/**
 * @fn void View::sizeThatFits(const View *self)
 * @memberof View
 */
static SDL_Size sizeThatFits(const View *self) {

	SDL_Size size = $(self, size);

	if (self->autoresizingMask & ViewAutoresizingWidth) {
		size.w = 0;
	}

	if (self->autoresizingMask & ViewAutoresizingHeight) {
		size.h = 0;
	}

	if (self->autoresizingMask & ViewAutoresizingContain) {
		size = MakeSize(0, 0);

		Array *subviews = $(self, visibleSubviews);
		for (size_t i = 0; i < subviews->count; i++) {

			const View *subview = $(subviews, objectAtIndex, i);
			const SDL_Size subviewSize = $(subview, sizeThatContains);

			SDL_Point subviewOrigin = MakePoint(0, 0);
			switch (subview->alignment) {
				case ViewAlignmentNone:
					subviewOrigin = MakePoint(subview->frame.x, subview->frame.y);
					break;
				default:
					break;
			}

			size.w = max(size.w, subviewOrigin.x + subviewSize.w);
			size.h = max(size.h, subviewOrigin.y + subviewSize.h);
		}

		size.w += self->padding.left + self->padding.right;
		size.h += self->padding.top + self->padding.bottom;

		release(subviews);
	}

	return size;
}

/**
 * @fn void View::sizeToContain(View *self)
 * @memberof View
 */
static void sizeToContain(View *self) {

	const SDL_Size size = $(self, sizeThatContains);

	$(self, resize, &size);
}

/**
 * @fn void View::sizeToFit(View *self)
 * @memberof View
 */
static void sizeToFit(View *self) {

	const SDL_Size size = $(self, sizeThatFits);

	$(self, resize, &size);
}

/**
 * @fn View::subviewWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *subviewWithIdentifier(const View *self, const char *identifier) {

	assert(identifier);

	const Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {

		View *subview = $(subviews, objectAtIndex, i);
		if (subview->identifier) {

			if (strcmp(identifier, subview->identifier) == 0) {
				return subview;
			}
		}
	}

	return NULL;
}

/**
 * @brief ArrayEnumerator for updateBindings recursion.
 */
static void updateBindings_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, updateBindings);
}

/**
 * @fn void View::updateBindings(View *self)
 * @memberof View
 */
static void updateBindings(View *self) {
	$((Array *) self->subviews, enumerateObjects, updateBindings_recurse, NULL);
}

/**
 * @fn SDL_Rect View::viewport(const View *self)
 * @memberof View
 */
static SDL_Rect viewport(const View *self) {

	assert(self->window);

	const SDL_Rect frame =  $(self, renderFrame);

	return MVC_TransformToWindow(self->window, &frame);
}

/**
 * @fn View *View::viewWithContentsOfFile(const char *path, Outlet *outlets)
 * @memberof View
 */
static View *viewWithContentsOfFile(const char *path, Outlet *outlets) {

	Data *data = $$(Data, dataWithContentsOfFile, path);

	View *view = $$(View, viewWithData, data, outlets);

	release(data);

	return view;
}

/**
 * @fn View *View::viewWithData(const Data *data, Outlet *outlets)
 * @memberof View
 */
static View *viewWithData(const Data *data, Outlet *outlets) {

	Dictionary *dictionary = $$(JSONSerialization, objectFromData, data, 0);

	View *view = $$(View, viewWithDictionary, dictionary, outlets);

	release(dictionary);

	return view;
}

/**
 * @fn View *View::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
 * @memberof View
 */
static View *viewWithDictionary(const Dictionary *dictionary, Outlet *outlets) {
	static Once once;

	do_once(&once, {
		_initialize(_Box());
		_initialize(_Button());
		_initialize(_Checkbox());
		_initialize(_CollectionView());
		_initialize(_HSVColorPicker());
		_initialize(_HueColorPicker());
		_initialize(_ImageView());
		_initialize(_Input());
		_initialize(_Label());
		_initialize(_PageView());
		_initialize(_Panel());
		_initialize(_RGBColorPicker());
		_initialize(_ScrollView());
		_initialize(_Select());
		_initialize(_Slider());
		_initialize(_StackView());
		_initialize(_TableView());
		_initialize(_TabView());
		_initialize(_Text());
		_initialize(_TextView());
	});

	_outlets = outlets;

	View *view = NULL;

	BindInlet(&MakeInlet(NULL, InletTypeView, &view, NULL), dictionary);

	if (outlets) {
		for (const Outlet *outlet = outlets; outlet->identifier; outlet++) {
			assert(*outlet->view);
		}
	}

	return view;
}

/**
 * @brief Predicate for visibleSubviews.
 */
static _Bool visibleSubviews_filter(ident obj, ident data) {

	const View *view = (View *) obj;

	return view->hidden == false && view->alignment != ViewAlignmentInternal;
}

/**
 * @fn Array *View::visibleSubviews(const View *self)
 * @memberof View
 */
static Array *visibleSubviews(const View *self) {
	return $((Array *) self->subviews, filteredArray, visibleSubviews_filter, NULL);
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;

	((ViewInterface *) clazz->def->interface)->addConstraint = addConstraint;
	((ViewInterface *) clazz->def->interface)->addConstraintWithDescriptor = addConstraintWithDescriptor;
	((ViewInterface *) clazz->def->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->def->interface)->addSubviewRelativeTo = addSubviewRelativeTo;
	((ViewInterface *) clazz->def->interface)->applyConstraints = applyConstraints;
	((ViewInterface *) clazz->def->interface)->applyConstraintsIfNeeded = applyConstraintsIfNeeded;
	((ViewInterface *) clazz->def->interface)->ancestorWithIdentifier = ancestorWithIdentifier;
	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->becomeFirstResponder = becomeFirstResponder;
	((ViewInterface *) clazz->def->interface)->bind = _bind;
	((ViewInterface *) clazz->def->interface)->bounds = bounds;
	((ViewInterface *) clazz->def->interface)->bringSubviewToFront = bringSubviewToFront;
	((ViewInterface *) clazz->def->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->def->interface)->clippingFrame = clippingFrame;
	((ViewInterface *) clazz->def->interface)->containsPoint = containsPoint;
	((ViewInterface *) clazz->def->interface)->createConstraint = createConstraint;
	((ViewInterface *) clazz->def->interface)->depth = depth;
	((ViewInterface *) clazz->def->interface)->descendantWithIdentifier = descendantWithIdentifier;
	((ViewInterface *) clazz->def->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->def->interface)->draw = draw;
	((ViewInterface *) clazz->def->interface)->hitTest = hitTest;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ViewInterface *) clazz->def->interface)->isDescendantOfView = isDescendantOfView;
	((ViewInterface *) clazz->def->interface)->isFirstResponder = isFirstResponder;
	((ViewInterface *) clazz->def->interface)->isVisible = isVisible;
	((ViewInterface *) clazz->def->interface)->layoutIfNeeded = layoutIfNeeded;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->def->interface)->removeAllConstraints = removeAllConstraints;
	((ViewInterface *) clazz->def->interface)->removeAllSubviews = removeAllSubviews;
	((ViewInterface *) clazz->def->interface)->removeConstraint = removeConstraint;
	((ViewInterface *) clazz->def->interface)->removeFromSuperview = removeFromSuperview;
	((ViewInterface *) clazz->def->interface)->removeSubview = removeSubview;
	((ViewInterface *) clazz->def->interface)->render = render;
	((ViewInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((ViewInterface *) clazz->def->interface)->renderFrame = renderFrame;
	((ViewInterface *) clazz->def->interface)->replaceSubview = replaceSubview;
	((ViewInterface *) clazz->def->interface)->resignFirstResponder = resignFirstResponder;
	((ViewInterface *) clazz->def->interface)->resize = resize;
	((ViewInterface *) clazz->def->interface)->respondToEvent = respondToEvent;
	((ViewInterface *) clazz->def->interface)->setWindow = setWindow;
	((ViewInterface *) clazz->def->interface)->size = size;
	((ViewInterface *) clazz->def->interface)->sizeThatContains = sizeThatContains;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;
	((ViewInterface *) clazz->def->interface)->sizeToContain = sizeToContain;
	((ViewInterface *) clazz->def->interface)->sizeToFit = sizeToFit;
	((ViewInterface *) clazz->def->interface)->subviewWithIdentifier = subviewWithIdentifier;
	((ViewInterface *) clazz->def->interface)->updateBindings = updateBindings;
	((ViewInterface *) clazz->def->interface)->viewport = viewport;
	((ViewInterface *) clazz->def->interface)->viewWithContentsOfFile = viewWithContentsOfFile;
	((ViewInterface *) clazz->def->interface)->viewWithData = viewWithData;
	((ViewInterface *) clazz->def->interface)->viewWithDictionary = viewWithDictionary;
	((ViewInterface *) clazz->def->interface)->visibleSubviews = visibleSubviews;
}

/**
 * @fn Class *View::_View(void)
 * @memberof View
 */
Class *_View(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "View";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(View);
		clazz.interfaceOffset = offsetof(View, interface);
		clazz.interfaceSize = sizeof(ViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

#pragma mark - Utilities

void MVC_MakeFirstResponder(SDL_Window *window, View *view) {

	assert(window);

	if (view) {
		assert(window == view->window);
		SDL_SetWindowData(window, MVC_FIRST_RESPONDER, view);
		if (MVC_LogEnabled(SDL_LOG_PRIORITY_DEBUG)) {
			String *desc = $((Object *) view, description);
			SDL_LogDebug(LOG_CATEGORY_MVC, "%s: %s\n", __func__, desc->chars);
			release(desc);
		}
	} else {
		SDL_SetWindowData(window, MVC_FIRST_RESPONDER, NULL);
		SDL_LogDebug(LOG_CATEGORY_MVC, "%s: NULL\n", __func__);
	}
}

View *MVC_FirstResponder(SDL_Window *window) {

	assert(window);
	
	return SDL_GetWindowData(window, MVC_FIRST_RESPONDER);
}
