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
 * @see View::sizeThatFits(View *, int *, int *)
 */
static void sizeThatFits(const View *self, int *w, int *h) {
	
	const Label *this = (Label *) self;
	$((View *) this->text, sizeThatFits, w, h);
	
	*w += self->padding.left + self->padding.right;
	*h += self->padding.top + self->padding.bottom;
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

		self->text = $(alloc(Text), initWithText, text, font);
		assert(self->text);
				
		$((View *) self, addSubview, (View *) self->text);
		$((View *) self, sizeToFit);
	}
	
	return self;
}

/**
 * @fn void Label::setFont(Label *self, Font *font)
 *
 * @memberof Label
 */
static void setFont(Label *self, Font *font) {
	
	$(self->text, setFont, font);
	
	$((View *) self, sizeToFit);
}

/**
 * @fn void Label::setText(Label *self, const char *text)
 *
 * @memberof Label
 */
static void setText(Label *self, const char *text) {
	
	$(self->text, setText, text);
	
	$((View *) self, sizeToFit);
}


#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	
	((LabelInterface *) clazz->interface)->initWithText = initWithText;
	((LabelInterface *) clazz->interface)->setFont = setFont;
	((LabelInterface *) clazz->interface)->setText = setText;
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

