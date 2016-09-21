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

#include <ObjectivelyMVC/Label.h>

#define _Class _Label

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Label *this = (Label *) self;

	release(this->text);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Label *this = (Label *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("text", InletTypeView, &this->text, NULL)
	);

	$(self, bind, dictionary, inlets);

	self->needsLayout = true;
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Label *) self, initWithText, NULL, NULL);
}

#pragma mark - Label

/**
 * @fn Label *Label::initWithText(Label *self, const char *text, Font *font)
 *
 * @memberof Label
 */
static Label *initWithText(Label *self, const char *text, Font *font) {

	self = (Label *) super(View, self, initWithFrame, NULL);
	if (self) {

		self->text = alloc(Text, initWithText, text, font);
		assert(self->text);

		self->text->view.alignment = ViewAlignmentMiddleLeft;

		$((View *) self, addSubview, (View *) self->text);

		self->view.autoresizingMask = ViewAutoresizingContain;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->interface)->init = init;

	((LabelInterface *) clazz->interface)->initWithText = initWithText;
}

Class _Label = {
	.name = "Label",
	.superclass = &_View,
	.instanceSize = sizeof(Label),
	.interfaceOffset = offsetof(Label, interface),
	.interfaceSize = sizeof(LabelInterface),
	.initialize = initialize,
};

#undef _Class
