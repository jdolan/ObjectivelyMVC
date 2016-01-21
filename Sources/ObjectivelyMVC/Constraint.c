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

#include <ObjectivelyMVC/Constraint.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Constraint

#pragma mark - Constraint

#define Clamp(val) (val < self->min ? self->min : (val > self->max ? self->max : val))

/**
 * @fn void Constraint::apply(const Constraint *self)
 *
 * @memberof Constraint
 */
static void apply(const Constraint *self) {
	
	SDL_Rect *sf = &self->source->frame;
	
	if (self->target) {
		const SDL_Rect *tf = &self->target->frame;
		switch (self->sourceAttribute) {
			case Top:
				switch (self->targetAttribute) {
					case Top:
						sf->y = Clamp(tf->y) + self->offset;
						break;
					case Bottom:
						sf->y = Clamp(tf->y + tf->h) + self->offset;
						break;
					default:
						break;
				}
				break;
			
			case Right:
				switch (self->targetAttribute) {
					case Right:
						sf->x = Clamp(tf->x + tf->w - sf->w) - self->offset;
						break;
					case Left:
						sf->x = Clamp(tf->x - sf->w) - self->offset;
						break;
					default:
						break;
				}
				break;
				
			case Bottom:
				switch (self->targetAttribute) {
					case Top:
						sf->y = Clamp(tf->y - sf->h) - self->offset;
						break;
					case Bottom:
						sf->y = Clamp(tf->y + tf->h - sf->h) - self->offset;
						break;
					default:
						break;
				}
				break;
			
			case Left:
				switch (self->targetAttribute) {
					case Right:
						sf->x = Clamp(tf->x + tf->w) + self->offset;
						break;
					case Left:
						sf->x = Clamp(tf->x) + self->offset;
						break;
					default:
						break;
				}
				break;
			
			case Width:
				switch (self->targetAttribute) {
					case Width:
						sf->w = Clamp(tf->w);
						break;
					case Height:
						sf->w = Clamp(tf->h);
						break;
					default:
						break;
				}
				break;
				
			case Height:
				switch (self->targetAttribute) {
					case Width:
						sf->h = Clamp(tf->w);
						break;
					case Height:
						sf->h = Clamp(tf->h);
						break;
					default:
						break;
				}
				break;
		}
	} else {
		assert(self->source->superview);
		const SDL_Rect *svf = &self->source->superview->frame;
		switch (self->sourceAttribute) {
			case Top:
				sf->y = Clamp(sf->y) + self->offset;
				break;
			case Right:
				sf->x = Clamp(svf->w - sf->w) - self->offset;
				break;
			case Bottom:
				sf->y = Clamp(svf->h - sf->h) - self->offset;
				break;
			case Left:
				sf->x = Clamp(sf->x) + self->offset;
				break;
			case Width:
				sf->w = Clamp(sf->w);
				break;
			case Height:
				sf->h = Clamp(sf->h);
				break;
		}
	}
}

/**
 * @fn void Constraint::bind(Constraint *self, View *target, Attribute targetAttribute, int offset)
 *
 * @memberof Constraint
 */
static void bind(Constraint *self, View *target, Attribute targetAttribute, int offset) {
	
	assert(target);

	$(self, unbind);
	
	self->target = retain(target);
	self->targetAttribute = targetAttribute;
	self->offset = offset;
	
	$(self->target->targetConstraints, addObject, self);
}

/**
 * @fn Constraint *Constraint::initWithSource(Constraint *self, View *source, Attribute sourceAttribute)
 *
 * @memberof Constraint
 */
static Constraint *initWithSource(Constraint *self, View *source, Attribute sourceAttribute) {

	self = (Constraint *) super(Object, self, init);
	if (self) {

		self->source = retain(source);
		self->sourceAttribute = sourceAttribute;
		
		self->min = INT32_MIN;
		self->max = INT32_MAX;
		
		$(source->constraints, addObject, self);		
	}

	return self;
}

/**
 * @fn void Constraint::limit(Constraint *self, int min, int max)
 *
 * @memberof Constraint
 */
static void limit(Constraint *self, int min, int max) {
	
	self->min = min;
	self->max = max;
	
	if (self->max > 0 && self->max < self->min) {
		self->max = self->min;
	}
}

/**
 * @fn void Constraint::unbind(Constraint *self)
 *
 * @memberof Constraint
 */
static void unbind(Constraint *self) {
	
	if (self->target) {
		$(self->target->targetConstraints, removeObject, self);
	}
	
	release(self->target);
	self->target = NULL;
	
	self->min = INT32_MIN;
	self->max = INT32_MAX;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ConstraintInterface *) clazz->interface)->apply = apply;
	((ConstraintInterface *) clazz->interface)->bind = bind;
	((ConstraintInterface *) clazz->interface)->initWithSource = initWithSource;
	((ConstraintInterface *) clazz->interface)->limit = limit;
	((ConstraintInterface *) clazz->interface)->unbind = unbind;
}

Class _Constraint = {
	.name = "Constraint",
	.superclass = &_Object,
	.instanceSize = sizeof(Constraint),
	.interfaceOffset = offsetof(Constraint, interface),
	.interfaceSize = sizeof(ConstraintInterface),
	.initialize = initialize,
};

#undef _Class
