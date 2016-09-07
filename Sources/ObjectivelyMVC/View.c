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

const EnumName ViewAlignmentNames[] = {
	NameEnum(ViewAlignmentNone),
	NameEnum(ViewAlignmentTopLeft),
	NameEnum(ViewAlignmentTopCenter),
	NameEnum(ViewAlignmentTopRight),
	NameEnum(ViewAlignmentMiddleLeft),
	NameEnum(ViewAlignmentMiddleCenter),
	NameEnum(ViewAlignmentMiddleRight),
	NameEnum(ViewAlignmentBottomLeft),
	NameEnum(ViewAlignmentBottomCenter),
	NameEnum(ViewAlignmentBottomRight),
	NameEnum(ViewAlignmentInternal),
	EnumNameLast
};

static const EnumName ViewAutoresizingNames[] = {
	NameEnum(ViewAutoresizingNone),
	NameEnum(ViewAutoresizingWidth),
	NameEnum(ViewAutoresizingHeight),
	NameEnum(ViewAutoresizingFill),
	NameEnum(ViewAutoresizingContain),
	EnumNameLast
};

Uint32 MVC_EVENT_RENDER_DEVICE_RESET;
Uint32 MVC_EVENT_UPDATE_BINDINGS;

static View *_firstResponder;

static __thread Outlet *_outlets;

#define _Class _View

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	View *this = (View *) self;

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
 * @return The SDL_Color for `array`.
 */
static SDL_Color bindColor(const Array *array) {

	assert(array);
	assert(array->count == 4);

	const Number *r = $(array, objectAtIndex, 0);
	const Number *g = $(array, objectAtIndex, 1);
	const Number *b = $(array, objectAtIndex, 2);
	const Number *a = $(array, objectAtIndex, 3);

#define ScaleColor(c) (c > 0.0 && c < 1.0 ? c * 255 : c)

	SDL_Color color = {
		ScaleColor(r->value),
		ScaleColor(g->value),
		ScaleColor(b->value),
		ScaleColor(a->value)
	};

	return color;
}

/**
 * @return The SDL_Size for `array`.
 */
static SDL_Size bindSize(const Array *array) {

	assert(array);
	assert(array->count == 2);

	const Number *w = $(array, objectAtIndex, 0);
	const Number *h = $(array, objectAtIndex, 1);

	SDL_Size size = { w->value, h->value };
	return size;
}

/**
 * @return The SDL_Rect for `array`.
 */
static SDL_Rect bindRect(const Array *array) {

	assert(array);
	assert(array->count == 4);

	const Number *x = $(array, objectAtIndex, 0);
	const Number *y = $(array, objectAtIndex, 1);
	const Number *w = $(array, objectAtIndex, 2);
	const Number *h = $(array, objectAtIndex, 3);

	SDL_Rect rect = { x->value, y->value, w->value, h->value };
	return rect;
}

/**
 * @brief ArrayEnumerator for bind subview recursion.
 */
static _Bool bindSubviews(const Array *array, ident obj, ident data) {

	View *subview = $$(View, viewWithDictionary, cast(Dictionary, obj), _outlets);

	$(cast(View, data), addSubview, subview);

	release(subview);

	return false;
}

/**
 * @fn void View::bind(View *self, const Dictionary *dictionary, const Inlet *inlets)
 *
 * @memberof View
 */
static void bind(View *self, const Dictionary *dictionary, const Inlet *inlets) {

	const Inlet *inlet = inlets;
	while (inlet->name) {

		const ident obj = $(dictionary, objectForKeyPath, inlet->name);
		if (obj) {

			switch (inlet->type) {
				case InletTypeBool:
					*((_Bool *) inlet->dest) = cast(Boole, obj)->value;
					break;
				case InletTypeCharacters:
					*((char **) inlet->dest) = strdup(cast(String, obj)->chars);
					break;
				case InletTypeColor:
					*((SDL_Color *) inlet->dest) = bindColor(cast(Array, obj));
					break;
				case InletTypeDouble:
					*((double *) inlet->dest) = cast(Number, obj)->value;
					break;
				case InletTypeFloat:
					*((float *) inlet->dest) = cast(Number, obj)->value;
					break;
				case InletTypeFont:
					*((Font **) inlet->dest) = $(alloc(Font), initWithName, cast(String, obj)->chars);
					break;
				case InletTypeEnum:
					*((int *) inlet->dest) = valueof(inlet->data, (cast(String, obj))->chars);
					break;
				case InletTypeImage:
					*((Image **) inlet->dest) = $(alloc(Image), initWithName, cast(String, obj)->chars);
					break;
				case InletTypeInteger:
					*((int *) inlet->dest) = cast(Number, obj)->value;
					break;
				case InletTypeRectangle:
					*((SDL_Rect *) inlet->dest) = bindRect(cast(Array, obj));
					break;
				case InletTypeSize:
					*((SDL_Size *) inlet->dest) = bindSize(cast(Array, obj));
					break;
				case InletTypeSubviews:
					$(cast(Array, obj), enumerateObjects, bindSubviews, *(View **) inlet->dest);
					break;
				case InletTypeView:
					if (inlet->data) {

						$(*(View **) inlet->dest, removeFromSuperview);
						release(*(View **) inlet->dest);

						*(View **) inlet->dest = $$(View, viewWithDictionary, cast(Dictionary, obj), _outlets);
					} else {
						$(*(View **) inlet->dest, awakeWithDictionary, cast(Dictionary, obj));
					}
					break;
			}
		}
		
		inlet++;
	}
}

/**
 * @fn void Viem::awakeWithDictionary(View *self, const Dictionary *dictionary, Outlet *outlets)
 *
 * @memberof View
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	const Inlet *inlets = MakeInlets(
		MakeInlet("identifier", InletTypeCharacters, &self->identifier, NULL),
		MakeInlet("alignment", InletTypeEnum, &self->alignment, (ident) ViewAlignmentNames),
		MakeInlet("autoresizingMask", InletTypeEnum, &self->autoresizingMask, (ident) ViewAutoresizingNames),
		MakeInlet("backgroundColor", InletTypeColor, &self->backgroundColor, NULL),
		MakeInlet("borderColor", InletTypeColor, &self->borderColor, NULL),
		MakeInlet("borderWidth", InletTypeInteger, &self->borderWidth, NULL),
		MakeInlet("frame", InletTypeRectangle, &self->frame, NULL),
		MakeInlet("isHidden", InletTypeBool, &self->isHidden, NULL),
		MakeInlet("padding", InletTypeRectangle, &self->padding, NULL),
		MakeInlet("subviews", InletTypeSubviews, &self, NULL),
		MakeInlet("zIndex", InletTypeInteger, &self->zIndex, NULL)
	);

	$(self, bind, dictionary, inlets);

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
 * @fn SDL_Rect View::clippingFrame(const View *self)
 *
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
	
	if (self->isHidden == false) {

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
 * @fn View *View::init(View *self)
 *
 * @memberof View
 */
static View *init(View *self) {
	return $(self, initWithFrame, NULL);
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
		if (view->isHidden) {
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

		if (self->autoresizingMask & ViewAutoresizingContain) {
			$(self, sizeToFit);
		}

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
	return MakeSize(
		max(self->frame.w, self->padding.left + self->padding.right),
		max(self->frame.h, self->padding.top + self->padding.bottom)
	);
}

/**
 * @fn void View::sizeThatFits(const View *self)
 *
 * @memberof View
 */
static SDL_Size sizeThatFits(const View *self) {

	SDL_Size size = $(self, size);

	if (self->autoresizingMask & ViewAutoresizingContain) {
		SDL_Size minSize = MakeSize(0, 0);

		Array *subviews = $(self, visibleSubviews);
		for (size_t i = 0; i < subviews->count; i++) {

			const View *subview = $(subviews, objectAtIndex, i);
			const SDL_Size subviewSize = $(subview, sizeThatFits);

			minSize.w = max(minSize.w, subview->frame.x + subviewSize.w);
			minSize.h = max(minSize.h, subview->frame.y + subviewSize.h);
		}

		minSize.w += self->padding.left + self->padding.right;
		minSize.h += self->padding.top + self->padding.bottom;
		
		release(subviews);

		size.w = max(size.w, minSize.w);
		size.h = max(size.h, minSize.h);
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

	const SDL_Rect frame =  $(self, renderFrame);

	return MVC_TransformToWindow($(self, window), &frame);
}

/**
 * @fn View *View::viewWithContentsOfFile(const char *path, Outlet *outlets)
 *
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
 *
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
 *
 * @memberof View
 */
static View *viewWithDictionary(const Dictionary *dictionary, Outlet *outlets) {
	static Once once;

	do_once(&once, {
		_initialize(&_Box);
		_initialize(&_Button);
		_initialize(&_Checkbox);
		_initialize(&_CollectionView);
		_initialize(&_ImageView);
		_initialize(&_Input);
		_initialize(&_Panel);
		_initialize(&_ScrollView);
		_initialize(&_Select);
		_initialize(&_Slider);
		_initialize(&_StackView);
		_initialize(&_TableView);
		_initialize(&_Text);
		_initialize(&_TextView);
	});

	_outlets = outlets;

	String *clazzName = $(dictionary, objectForKeyPath, "class");
	if (clazzName) {
		Class *clazz = classForName(clazzName->chars);
		if (clazz) {
			const Class *c = clazz;
			while (c) {
				if (c == &_View) {
					View *view = $((View *) _alloc(clazz), init);

					$(view, awakeWithDictionary, dictionary);

					return view;
				}
				c = c->superclass;
			}
		}
	}

	assert(false);
	return NULL;
}

/**
 * @brief Predicate for visibleSubviews.
 */
static _Bool visibleSubviews_filter(ident obj, ident data) {

	const View *view = (View *) obj;

	return view->isHidden == false && view->alignment != ViewAlignmentInternal;
}

/**
 * @fn Array *View::visibleSubviews(const View *self)
 *
 * @memberof View
 */
static Array *visibleSubviews(const View *self) {
	return $((Array *) self->subviews, filteredArray, visibleSubviews_filter, NULL);
}

/**
 * @fn SDL_Window *View::window(const View *self)
 *
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

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->description = description;

	((ViewInterface *) clazz->interface)->addSubview = addSubview;
	((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->interface)->becomeFirstResponder = becomeFirstResponder;
	((ViewInterface *) clazz->interface)->bind = bind;
	((ViewInterface *) clazz->interface)->bounds = bounds;
	((ViewInterface *) clazz->interface)->canBecomeFirstResponder = canBecomeFirstResponder;
	((ViewInterface *) clazz->interface)->clippingFrame = clippingFrame;
	((ViewInterface *) clazz->interface)->containsPoint = containsPoint;
	((ViewInterface *) clazz->interface)->depth = depth;
	((ViewInterface *) clazz->interface)->didReceiveEvent = didReceiveEvent;
	((ViewInterface *) clazz->interface)->draw = draw;
	((ViewInterface *) clazz->interface)->firstResponder = firstResponder;
	((ViewInterface *) clazz->interface)->init = init;
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
	((ViewInterface *) clazz->interface)->viewWithContentsOfFile = viewWithContentsOfFile;
	((ViewInterface *) clazz->interface)->viewWithData = viewWithData;
	((ViewInterface *) clazz->interface)->viewWithDictionary = viewWithDictionary;
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

SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect) {

	if (window == NULL) {
		window = SDL_GL_GetCurrentWindow();
	}

	assert(window);
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

	if (window == NULL) {
		window = SDL_GL_GetCurrentWindow();
	}

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
