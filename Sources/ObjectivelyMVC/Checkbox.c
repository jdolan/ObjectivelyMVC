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

#include <ObjectivelyMVC/Checkbox.h>

#define _Class _Checkbox

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Checkbox *this = (Checkbox *) self;

	release(this->check);

	super(Object, self, dealloc);
}

#pragma mark - Checkbox

static Image *_check;

/**
 * @fn Checkbox *Checkbox::initWithFrame(Checkbox *self, const SDL_Frame *frame)
 *
 * @memberof Checkbox
 */
static Checkbox *initWithFrame(Checkbox *self, const SDL_Rect *frame) {

	self = (Checkbox *) super(Control, self, initWithFrame, frame);
	if (self) {
		
		self->check = $(alloc(ImageView), initWithImage, _check);
		assert(self->check);
		
		$((View *) self, addSubview, (View *) self->check);
		
		Control *control = (Control *) self;
		control->labelPosition = LabelPositionRight;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((CheckboxInterface *) clazz->interface)->initWithFrame = initWithFrame;
	
	_check = $(alloc(Image), initWithName, "check.png");
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_check);
}

Class _Checkbox = {
	.name = "Checkbox",
	.superclass = &_Control,
	.instanceSize = sizeof(Checkbox),
	.interfaceOffset = offsetof(Checkbox, interface),
	.interfaceSize = sizeof(CheckboxInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
