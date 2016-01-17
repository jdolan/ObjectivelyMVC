/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

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

#pragma mark - View initializers

/**
 * @see ViewInterface::init(View *, SDL_Rect *)
 */
static View *initWithFrame(View *self, SDL_Rect *frame) {

	self = (View *) super(Object, self, init);
	if (self) {
		self->subviews = $$(MutableArray, array);

		self->window = SDL_GL_GetCurrentWindow();
		self->renderer = SDL_GetRenderer(self->window);
		if (self->renderer == NULL) {
			self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
			if (self->renderer == NULL) {
				SDL_SetError("%s: Failed to create renderer", __func__);
			}
		}

		if (frame) {
			self->frame = *frame;
		} else {
			SDL_LogDebug(0, "%s: NULL frame", __func__);
			SDL_GetWindowSize(self->window, &self->frame.w, &self->frame.h);
		}

		self->backgroundColor = Colors.clear;
	}

	return self;
}

#pragma mark - ViewInterface

/**
 * @see ViewInterface::addSubview(View *, View *)
 */
static void addSubview(View *self, View *subview) {

	if (subview) {
		if ($((Array *) self->subviews, indexOfObject, subview) == -1) {
			$(self->subviews, addObject, subview);
			subview->superview = self;
		}
	}
}

/**
 * @see ViewInterface::removeSubview(View *, View *)
 */
static void removeSubview(View *self, View *subview) {

	if (subview) {
		if ($((Array *) self->subviews, indexOfObject, subview) != -1) {
			$(self->subviews, removeObject, subview);
			subview->superview = NULL;
		}
	}
}

/**
 * @see ViewInterface::removeFromSuperview(View *)
 */
static void removeFromSuperview(View *self) {

	if (self->superview) {
		$(self->superview, removeSubview, self);
	}
}

/**
 * @see ViewInterface::draw(View *)
 */
static void draw(View *self) {

	if (self->backgroundColor.a) {
		const SDL_Color c = self->backgroundColor;
		SDL_SetRenderDrawColor(self->renderer, c.r, c.g, c.b, c.a);

		SDL_RenderFillRect(self->renderer, &self->frame);

		const SDL_Color w = Colors.white;
		SDL_SetRenderDrawColor(self->renderer, w.r, w.g, w.b, w.a);
	}
}

/**
 * @see ViewInterface::activate(View *)
 */
static void activate(View *self) {
	// TODO
}

/**
 * @see ViewInterface::deactivate(View *)
 */
static void deactivate(View *self) {
	// TODO
}

/**
 * @see ViewInterface::respondToEvent(View *, SDL_Event *, SDL_bool *)
 */
static void respondToEvent(View *self, SDL_Event *event, SDL_bool *cancel) {
	// TODO
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	ViewInterface *view = (ViewInterface *) clazz->interface;

	view->initWithFrame = initWithFrame;

	view->addSubview = addSubview;
	view->removeSubview = removeSubview;
	view->removeFromSuperview = removeFromSuperview;

	view->draw = draw;

	view->activate = activate;
	view->deactivate = deactivate;
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
