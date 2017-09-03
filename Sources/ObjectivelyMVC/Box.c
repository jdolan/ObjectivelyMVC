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

#include <ObjectivelyMVC/Box.h>

#define _Class _Box

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Box *this = (Box *) self;

	release(this->contentView);
	release(this->label);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionar(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Box *this = (Box *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("contentView", InletTypeView, &this->contentView, NULL),
		MakeInlet("label", InletTypeView, &this->label, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Box *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	super(View, self, layoutSubviews);

	View *label = (View *) ((Box *) self)->label;
	if (label->hidden == false) {

		const SDL_Size size = $(label, sizeThatContains);
		label->frame.y = -size.h * 0.5;
	}
}

#pragma mark - Box

/**
 * @fn Box *Box::initWithFrame(Box *self, const SDL_Rect *frame)
 * @memberof Box
 */
static Box *initWithFrame(Box *self, const SDL_Rect *frame) {

	self = (Box *) super(View, self, initWithFrame, frame);
	if (self) {

		self->contentView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->spacing = DEFAULT_BOX_SPACING;
		self->contentView->view.autoresizingMask |= ViewAutoresizingWidth;

		$((View *) self, addSubview, (View *) self->contentView);

		Font *font = $$(Font, defaultFont, FontCategorySecondaryLabel);

		self->label = $(alloc(Label), initWithText, NULL, font);
		assert(self->label);

		View *label = (View *) self->label;

		label->alignment = ViewAlignmentInternal;
		label->backgroundColor = Colors.Gray;
		label->frame.x = DEFAULT_BOX_LABEL_X;

		label->padding.right = DEFAULT_BOX_LABEL_PADDING;
		label->padding.left = DEFAULT_BOX_LABEL_PADDING;

		$((View *) self, addSubview, (View *) self->label);

		self->view.autoresizingMask = ViewAutoresizingContain;

		self->view.borderColor = Colors.Gray;
		self->view.borderWidth = 1;

		self->view.padding.top = DEFAULT_BOX_PADDING;
		self->view.padding.right = DEFAULT_BOX_PADDING;
		self->view.padding.bottom = DEFAULT_BOX_PADDING;
		self->view.padding.left = DEFAULT_BOX_PADDING;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;

	((BoxInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *Box::_Box(void)
 * @memberof Box
 */
Class *_Box(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Box";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(Box);
		clazz.interfaceOffset = offsetof(Box, interface);
		clazz.interfaceSize = sizeof(BoxInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
