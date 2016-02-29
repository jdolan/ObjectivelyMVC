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

#include <ObjectivelyMVC/Input.h>
#include <ObjectivelyMVC/Log.h>

#define _Class _Input

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Input *this = (Input *) self;
	
	release(this->control);
	
	release(this->label);
	
	super(Object, self, dealloc);
}

#pragma mark - Input

/**
 * @fn Input *Input::initWithOrientation(Input *self, InputOrientation orientation, Control *control, Label *label)
 *
 * @memberof Input
 */
static Input *initWithOrientation(Input *self, InputOrientation orientation, Control *control, Label *label) {
	
	self = (Input *) super(StackView, self, initWithFrame, NULL);
	if (self) {
		
		self->orientation = orientation;
		
		self->control = retain(control);

		self->label = retain(label);
		
		switch (self->orientation) {
				
			case InputOrientationLeft:
				self->stackView.axis = StackViewAxisHorizontal;
				self->label->view.alignment = ViewAlignmentMiddleLeft;
				break;
				
			case InputOrientationRight:
				self->stackView.axis = StackViewAxisHorizontal;
				self->label->view.alignment = ViewAlignmentMiddleRight;
				break;
				
			case InputOrientationAbove:
				self->stackView.axis = StackViewAxisVertical;
				self->label->view.alignment = ViewAlignmentTopCenter;
				break;
				
			case InputOrientationBelow:
				self->stackView.axis = StackViewAxisVertical;
				self->label->view.alignment = ViewAlignmentBottomCenter;
				break;
		}
		
		switch (self->orientation) {
				
			case InputOrientationLeft:
			case InputOrientationAbove:
				$((View *) self, addSubview, (View *) self->label);
				$((View *) self, addSubview, (View *) self->control);
				break;
				
			case InputOrientationRight:
			case InputOrientationBelow:
				$((View *) self, addSubview, (View *) self->control);
				$((View *) self, addSubview, (View *) self->label);
				break;
		}
		
		self->stackView.spacing = DEFAULT_INPUT_SPACING;
		
		$((View *) self, sizeToFit);
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((InputInterface *) clazz->interface)->initWithOrientation = initWithOrientation;
}

Class _Input = {
	.name = "Input",
	.superclass = &_StackView,
	.instanceSize = sizeof(Input),
	.interfaceOffset = offsetof(Input, interface),
	.interfaceSize = sizeof(InputInterface),
	.initialize = initialize,
};

#undef _Class

