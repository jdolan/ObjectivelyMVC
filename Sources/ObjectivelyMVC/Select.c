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

#include <ObjectivelyMVC/Select.h>

#define _Class _Select

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Select *this = (Select *) self;
	
	release(this->options);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static _Bool respondToEvent(View *self, const SDL_Event *event) {
	
	return super(View, self, respondToEvent, event);
}

#pragma mark - Select

/**
 * @fn void Select::addOption(Select *self, const char *title, ident value)
 *
 * @memberof Select
 */
static void addOption(const Select *self, const char *title, ident value) {
	
	assert(title);
	assert(value);
	
	Option *option = $(alloc(Option), initWithTitle, title, value);
	assert(option);
	
	$(self->options, addObject, option);
	
	release(option);
}

/**
 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof Select
 */
static Select *initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (Select *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		
		self->options = $$(MutableArray, array);
		assert(self->options);
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = BevelTypeInset;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_SELECT_WIDTH;
			}
		}
	}
	
	return self;
}

/**
 * @fn void Select::removeOptionWithValue(Select *self, const ident value)
 *
 * @memberof Select
 */
static void removeOptionWithValue(const Select *self, const ident value) {

	Array *options = (Array *) self->options;
	
	for (size_t i = 0; i < options->count; i++) {
		Option *option = $(options, objectAtIndex, i);
		if (option->value == value) {
			return $(self->options, removeObject, option);
		}
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
	
	((SelectInterface *) clazz->interface)->addOption = addOption;
	((SelectInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((SelectInterface *) clazz->interface)->removeOptionWithValue = removeOptionWithValue;
}

Class _Select = {
	.name = "Select",
	.superclass = &_Control,
	.instanceSize = sizeof(Select),
	.interfaceOffset = offsetof(Select, interface),
	.interfaceSize = sizeof(SelectInterface),
	.initialize = initialize,
};

#undef _Class

