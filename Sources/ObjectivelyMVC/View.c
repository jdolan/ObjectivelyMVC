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
 * @brief ArrayEnumerator for Constraints dealloc.
 *
 * @remarks If we are the source View, release the Constraint.
 */
static _Bool dealloc_constraints(const Array *array, ident obj, ident data) {
	
	View *self = (View *) data;
	Constraint *constraint = (Constraint *) obj;
	
	$(constraint, unbind);
	
	if (array == (const Array *) self->constraints) {
		release(constraint);
	}
	
	return false;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	View *this = (View *) self;
	
	$((Array *) this->constraints, enumerateObjects, dealloc_constraints, self);
	$((Array *) this->targetConstraints, enumerateObjects, dealloc_constraints, self);
	
	release(this->constraints);
	release(this->targetConstraints);

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
 * @fn Constraint *View::constrain(View *self, Attribute sourceAttribute, int min, int max)
*
 * @memberof View
 */
static Constraint *constrain(View *self, Attribute sourceAttribute, int min, int max) {
	
	Constraint *constraint = $(self, constraintForAttribute, sourceAttribute);
	
	$(constraint, limit, min, max);
	
	$(self, updateConstraint, constraint);
	
	return constraint;
}

/**
 * @fn Constraint *View::constrainTo(View *self, Attribute sourceAttribute, View *target, Attribute targetAttribute, int offset)
 *
 * @memberof View
 */
static Constraint *constrainTo(View *self, Attribute sourceAttribute, View *target, Attribute targetAttribute, int offset) {
	
	assert(target);
	
	Constraint *constraint = $(self, constraintForAttribute, sourceAttribute);
	
	$(constraint, bind, target, targetAttribute, offset);
	
	$(self, updateConstraint, constraint);
	
	return constraint;
}

/**
 * @fn Constraint *View::constraintForAttribute(View *self, Attribute sourceAttribute)
 *
 * @memberof View
 */
static Constraint *constraintForAttribute(View *self, Attribute sourceAttribute) {
	
	Array *constraints = (Array *) self->constraints;
	for (size_t i = 0; i < constraints->count; i++) {
		
		Constraint *constraint = (Constraint *) $(constraints, objectAtIndex, i);
		if (constraint->sourceAttribute == sourceAttribute) {
			return constraint;
		}
	}
	
	return $(alloc(Constraint), initWithSource, self, sourceAttribute);
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
 * @fn View::deleteConstraint(View *self, Attribute sourceAttribute)
 *
 * @memberof View
 */
static void deleteConstraint(View *self, Attribute sourceAttribute) {
	
	Constraint *constraint = $(self, constraintForAttribute, sourceAttribute);
	
	$(constraint, unbind);
	$(self->constraints, removeObject, constraint);
	
	release(constraint);
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
		
		if (frame) {
			self->frame = *frame;
		}
		
		self->constraints = $$(MutableArray, array);
		assert(self->constraints);
		
		self->subviews = $$(MutableArray, array);
		assert(self->subviews);
		
		self->targetConstraints = $$(MutableArray, array);
		assert(self->targetConstraints);
		
		self->backgroundColor = Colors.BackgroundColor;
		self->borderColor = Colors.ForegroundColor;
	}
	
	return self;
}

/**
 * @fn View::layoutSubviews(View *self)
 *
 * @memberof View
 */
static void layoutSubviews(View *self) {
	
	self->needsLayout = false;
	
	Array *constraints = (Array *) self->constraints;
	for (size_t i = 0; i < constraints->count; i++) {
		
		Constraint *constraint = $(constraints, objectAtIndex, i);
		if (constraint->target) {
			if (constraint->target->needsLayout) {
				$(constraint->target, layoutSubviews);
			}
		}
		
		$(constraint, apply);
	}
	
	Array *subviews = (Array *) self->subviews;
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *subview = $(subviews, objectAtIndex, i);
		$(subview, layoutSubviews);
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
	
	SDL_Rect frame = { .w = self->frame.w, .h = self->frame.h };
	
	const View *view = self;
	while (true) {
		
		frame.x += view->frame.x;
		frame.y += view->frame.y;
		
		if (view->superview) {
			view = view->superview;
			
			frame.x += view->padding.left;
			frame.y += view->padding.top;
		} else {
			break;
		}
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
		$(subview, sizeToFit);
		
		int sw, sh;
		$(subview, sizeThatFits, &sw, &sh);

		sw += subview->frame.x + subview->margin.left + subview->margin.right;
		sw += self->padding.left + self->padding.right;
		
		sh += subview->frame.y + subview->margin.top + subview->margin.bottom;
		sh += self->padding.top + self->padding.bottom;
		
		if (sw > self->frame.w) {
			self->frame.w = sw;
		}
		
		if (sh > self->frame.h) {
			self->frame.h = sh;
		}
	}
}

/**
 * @fn void View::updateConstraint(View *self, const Constraint *constraint)
 *
 * @memberof View
 */
static void updateConstraint(View *self, const Constraint *constraint) {
	
	Array *targetConstraints = (Array *) self->targetConstraints;
	for (size_t i = 0; i < targetConstraints->count; i++) {
		
		Constraint *targetConstraint = $(targetConstraints, objectAtIndex, i);
		targetConstraint->source->needsLayout = true;
	}
	
	self->needsLayout = true;
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
	view->constrain = constrain;
	view->constrainTo = constrainTo;
	view->constraintForAttribute = constraintForAttribute;
	view->containsPoint = containsPoint;
	view->deleteConstraint = deleteConstraint;
	view->draw = draw;
	view->initWithFrame = initWithFrame;
	view->layoutSubviews = layoutSubviews;
	view->removeFromSuperview = removeFromSuperview;
	view->removeSubview = removeSubview;
	view->render = render;
	view->renderFrame = renderFrame;
	view->respondToEvent = respondToEvent;
	view->sizeThatFits = sizeThatFits;
	view->sizeToFit = sizeToFit;
	view->updateConstraint = updateConstraint;
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
