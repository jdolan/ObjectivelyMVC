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
#include <stdlib.h>
#include <string.h>

#include <Objectively/Config.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/TableColumn.h>

#define _Class _TableColumn

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TableColumn *this = (TableColumn *) self;

	free(this->identifier);

	super(Object, self, dealloc);
}

#pragma mark - TableColumn

/**
 * @fn TableColumn *TableColumn::init(TableColumn *self)
 *
 * @memberof TableColumn
 */
static TableColumn *initWithIdentifier(TableColumn *self, const char *identifier) {
	
	self = (TableColumn *) super(Object, self, init);
	if (self) {

		self->headerCell = $(alloc(TableHeaderCellView), initWithFrame, NULL);
		assert(self->headerCell);
		
		self->identifier = strdup(identifier);
		assert(self->identifier);

		$(self->headerCell->tableCellView.text, setText, self->identifier);

		self->width = DEFAULT_TABLE_COLUMN_WIDTH;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((TableColumnInterface *) clazz->interface)->initWithIdentifier = initWithIdentifier;
}

Class _TableColumn = {
	.name = "TableColumn",
	.superclass = &_Object,
	.instanceSize = sizeof(TableColumn),
	.interfaceOffset = offsetof(TableColumn, interface),
	.interfaceSize = sizeof(TableColumnInterface),
	.initialize = initialize,
};

#undef _Class

