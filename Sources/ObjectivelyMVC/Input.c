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

const EnumName InputOrientationNames[] = MakeEnumNames(
	MakeEnumName(InputOrientationLeft),
	MakeEnumName(InputOrientationRight),
	MakeEnumName(InputOrientationAbove),
	MakeEnumName(InputOrientationBelow)
);

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

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Input *this = (Input *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("control", InletTypeView, &this->control, NULL),
		MakeInlet("label", InletTypeView, &this->label, NULL),
		MakeInlet("orientation", InletTypeEnum, &this->orientation, (ident) InputOrientationNames)
	);

	$(self, bind, dictionary, inlets);

	$(this, setOrientation, this->orientation);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Input *) self, initWithFrame, NULL);
}

#pragma mark - Input

/**
 * @fn Input *Input::initWithOrientation(Input *self, Control *control)
 * @memberof Input
 */
static Input *initWithFrame(Input *self, const SDL_Rect *frame) {
	
	self = (Input *) super(StackView, self, initWithFrame, frame);
	if (self) {

		self->control = $(alloc(Control), initWithFrame, NULL, ControlStyleDefault);
		assert(self->control);

		self->label = $(alloc(Label), initWithText, NULL, NULL);
		assert(self->label);

		$((View *) self, addSubview, (View *) self->control);
		$((View *) self, addSubview, (View *) self->label);

		$(self, setOrientation, InputOrientationLeft);

		self->stackView.spacing = DEFAULT_INPUT_SPACING;
	}
	
	return self;
}

/**
 * @fn void Input::setControl(Input *self, Control *control)
 * @memberof Input
 */
static void setControl(Input *self, Control *control) {

	assert(control);

	$((View *) self, removeSubview, (View *) self->control);
	release(self->control);

	self->control = retain(control);
	$((View *) self, addSubview, (View *) self->control);

	$(self, setOrientation, self->orientation);
}

/**
 * @fn void Input::setLabel(Input *self, Label *label)
 * @memberof Input
 */
static void setLabel(Input *self, Label *label) {

	assert(label);

	$((View *) self, removeSubview, (View *) self->label);
	release(self->label);

	self->label = retain(label);
	$((View *) self, addSubview, (View *) self->label);

	$(self, setOrientation, self->orientation);
}

/**
 * @fn void Input::setOrientation(Input *self, InputOrientation orientation)
 * @memberof Input
 */
static void setOrientation(Input *self, InputOrientation orientation) {

	self->orientation = orientation;

	$((View *) self->control, removeFromSuperview);
	$((View *) self->label, removeFromSuperview);

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

	switch (self->orientation) {
		case InputOrientationLeft:
			self->stackView.axis = StackViewAxisHorizontal;
			self->label->view.alignment = ViewAlignmentMiddleLeft;
			self->control->view.alignment = ViewAlignmentMiddleRight;
			break;

		case InputOrientationRight:
			self->stackView.axis = StackViewAxisHorizontal;
			self->label->view.alignment = ViewAlignmentMiddleRight;
			self->control->view.alignment = ViewAlignmentMiddleLeft;
			break;

		case InputOrientationAbove:
		case InputOrientationBelow:
			self->stackView.axis = StackViewAxisVertical;
			self->label->view.alignment = ViewAlignmentMiddleCenter;
			self->control->view.alignment = ViewAlignmentMiddleCenter;
			break;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;

	((InputInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((InputInterface *) clazz->def->interface)->setControl = setControl;
	((InputInterface *) clazz->def->interface)->setLabel = setLabel;
	((InputInterface *) clazz->def->interface)->setOrientation = setOrientation;
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

