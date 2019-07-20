/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include <Objectively/String.h>

#include "TableColumn.h"

#define _Class _TableColumn

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TableColumn *this = (TableColumn *) self;

	release(this->headerCell);
	
	free(this->identifier);

	super(Object, self, dealloc);
}

#pragma mark - TableColumn

/**
 * @fn TableColumn *TableColumn::init(TableColumn *self)
 * @memberof TableColumn
 */
static TableColumn *initWithIdentifier(TableColumn *self, const char *identifier) {

	self = (TableColumn *) super(Object, self, init);
	if (self) {

		self->headerCell = $(alloc(TableHeaderCellView), initWithFrame, NULL);
		assert(self->headerCell);

		self->identifier = strdup(identifier);
		assert(self->identifier);

		self->headerCell->tableCellView.view.identifier = strdup(self->identifier);

		$(((TableCellView *) self->headerCell)->text, setText, self->identifier);
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

/**
 * @fn Class *TableColumn::_TableColumn(void)
 * @memberof TableColumn
 */
Class *_TableColumn(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "TableColumn",
			.superclass = _Object(),
			.instanceSize = sizeof(TableColumn),
			.interfaceOffset = offsetof(TableColumn, interface),
			.interfaceSize = sizeof(TableColumnInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

