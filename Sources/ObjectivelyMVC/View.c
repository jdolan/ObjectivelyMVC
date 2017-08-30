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

#include <Objectively.h>

#include <ObjectivelyMVC.h>

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

static View *_firstResponder;

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

	const SDL_Rect *f = &((View *) self)->frame;

	return str("%s@%p (%d,%d) %dx%d", self->clazz->name, self, f->x, f->y, f->w, f->h);
}

#pragma mark - View

/**
 * @fn void View::addConstraint(View *self, Constraint *constraint)
 * @memberof View
 */
static void addConstraint(View *self, Constraint *constraint) {

	$(self->constraints, addObject, constraint);

	self->needsLayout = true;
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

	self->needsLayout = true;
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
		MakeInlet("subviews", InletTypeSubviews, &self, NULL),
		MakeInlet("zIndex", InletTypeInteger, &self->zIndex, NULL)
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
	_firstResponder = self;
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
 * @fn _Bool View::canBecomeFirstResponder(const View *self)
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
	return self->zIndex + (self->superview ? $(self->superview, depth) + 1 : 0);
}

/**
 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
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
			} else if (event->type == SDL_MOUSEWHEEL) {
				SDL_Point point;
				SDL_GetMouseState(&point.x, &point.y);
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
static void draw_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, draw, (Renderer *) data);
}

/**
 * @fn void View::draw(View *self, Renderer *renderer)
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
 * @memberof View
 */
static View *firstResponder(void) {
	return _firstResponder;
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
	return _firstResponder == self;
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
		self->needsLayout = false;

		$(self, layoutSubviews);
	}

	const Array *subviews = (Array *) self->subviews;
	$(subviews, enumerateObjects, layoutIfNeeded_recurse, NULL);
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

	self->needsLayout = true;
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

		self->needsLayout = true;
	}
}

/**
 * @fn void View::render(View *self, Renderer *renderer)
 * @memberof View
 */
static void render(View *self, Renderer *renderer) {

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
		_firstResponder = NULL;
	}
}

/**
 * @brief ArrayEnumerator for resize recursion.
 */
static void resize_recurse(const Array *array, ident obj, ident data) {

	View *subview = (View *) obj;

	SDL_Size size = $(subview, size);

	if (subview->autoresizingMask & ViewAutoresizingWidth) {
		size = MakeSize(0, size.h);
	}

	if (subview->autoresizingMask & ViewAutoresizingHeight) {
		size = MakeSize(size.w, 0);
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
 * @brief ArrayEnumerator for respondToEvent recursion.
 */
static void respondToEvent_recurse(const Array *array, ident obj, ident data) {
	$((View *) obj, respondToEvent, (const SDL_Event *) data);
}

/**
 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
 * @memberof View
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	assert(event);

	if (self->superview == NULL) {
		if (event->type == SDL_WINDOWEVENT) {
			if (event->window.event == SDL_WINDOWEVENT_SHOWN
				|| event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {

				if (self->autoresizingMask & ViewAutoresizingFill) {

					SDL_Size size;
					SDL_GetWindowSize($(self, window), &size.w, &size.h);

					$(self, resize, &size);
				}
			}
		}
	}

	$((Array *) self->subviews, enumerateObjects, respondToEvent_recurse, (ident) event);
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

	const SDL_Rect frame =  $(self, renderFrame);

	return MVC_TransformToWindow($(self, window), &frame);
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
		_initialize(_ColorSelect());
		_initialize(_ImageView());
		_initialize(_Input());
		_initialize(_Label());
		_initialize(_PageView());
		_initialize(_Panel());
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

/**
 * @fn SDL_Window *View::window(const View *self)
 * @memberof View
 */
static SDL_Window *window(const View *self) {
	return SDL_GL_GetCurrentWindow();
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->description = description;

	((ViewInterface *) clazz->def->interface)->addConstraint = addConstraint;
	((ViewInterface *) clazz->def->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->def->interface)->addSubviewRelativeTo = addSubviewRelativeTo;
	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->becomeFirstResponder = becomeFirstResponder;
	((ViewInterface *) clazz->def->interface)->bind = _bind;
	((ViewInterface *) clazz->def->interface)->bounds = bounds;
	((ViewInterface *) clazz->def->interface)->canBecomeFirstResponder = canBecomeFirstResponder;
	((ViewInterface *) clazz->def->interface)->clippingFrame = clippingFrame;
	((ViewInterface *) clazz->def->interface)->containsPoint = containsPoint;
	((ViewInterface *) clazz->def->interface)->createConstraint = createConstraint;
	((ViewInterface *) clazz->def->interface)->depth = depth;
	((ViewInterface *) clazz->def->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->def->interface)->draw = draw;
	((ViewInterface *) clazz->def->interface)->firstResponder = firstResponder;
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
	((ViewInterface *) clazz->def->interface)->size = size;
	((ViewInterface *) clazz->def->interface)->sizeThatContains = sizeThatContains;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;
	((ViewInterface *) clazz->def->interface)->sizeToContain = sizeToContain;
	((ViewInterface *) clazz->def->interface)->sizeToFit = sizeToFit;
	((ViewInterface *) clazz->def->interface)->updateBindings = updateBindings;
	((ViewInterface *) clazz->def->interface)->viewport = viewport;
	((ViewInterface *) clazz->def->interface)->viewWithContentsOfFile = viewWithContentsOfFile;
	((ViewInterface *) clazz->def->interface)->viewWithData = viewWithData;
	((ViewInterface *) clazz->def->interface)->viewWithDictionary = viewWithDictionary;
	((ViewInterface *) clazz->def->interface)->visibleSubviews = visibleSubviews;
	((ViewInterface *) clazz->def->interface)->window = window;
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

SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect) {

	assert(rect);

	SDL_Rect transformed = *rect;

	int dh = 0;
	const double scale = MVC_WindowScale(window, NULL, &dh);

	transformed.x *= scale;
	transformed.y *= scale;
	transformed.w *= scale;
	transformed.h *= scale;

	transformed.y = dh - transformed.h - transformed.y;

	return transformed;
}

double MVC_WindowScale(SDL_Window *window, int *height, int *drawableHeight) {

	window = window ?: SDL_GL_GetCurrentWindow();
	assert(window);

	int h;
	SDL_GetWindowSize(window, NULL, &h);

	if (height) {
		*height = h;
	}

	if (h) {

		int dh;
		SDL_GL_GetDrawableSize(window, NULL, &dh);

		if (drawableHeight) {
			*drawableHeight = dh;
		}

		if (dh) {
			return dh / (double) h;
		}
	}

	return 1.0;
}
