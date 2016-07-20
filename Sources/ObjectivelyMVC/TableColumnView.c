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

#include <ObjectivelyMVC/TableColumnView.h>

#define _Class _TableColumnView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	//..
	
	super(Object, self, dealloc);
}

#pragma mark - TableColumnView

/**
 * @fn TableColumnView *TableColumnView::init(TableColumnView *self)
 *
 * @memberof TableColumnView
 */
static TableColumnView *initWithIdentifier(TableColumnView *self, const char *identifier) {
	
	self = (TableColumnView *) super(Control, self, initWithFrame, NULL, ControlStyleDefault);
	if (self) {
		
		self->identifier = identifier;
		assert(self->identifier);
		
		self->control.view.frame.w = DEFAULT_TABLE_VIEW_COLUMN_WIDTH;
	}
	
	return self;
}

//..

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((TableColumnViewInterface *) clazz->interface)->initWithIdentifier = initWithIdentifier;
}

Class _TableColumnView = {
	.name = "TableColumnView",
	.superclass = &_Control,
	.instanceSize = sizeof(TableColumnView),
	.interfaceOffset = offsetof(TableColumnView, interface),
	.interfaceSize = sizeof(TableColumnViewInterface),
	.initialize = initialize,
};

#undef _Class

