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

#include <ObjectivelyMVC/TableCellView.h>

#define _Class _TableCellView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	TableCellView *this = (TableCellView *) self;

	release(this->text);
	
	super(Object, self, dealloc);
}

#pragma mark - TableCellView

/**
 * @fn TableCellView *TableCellView::initWithFrame(TableCellView *self, const SDL_Rect *frame)
 *
 * @memberof TableCellView
 */
static TableCellView *initWithFrame(TableCellView *self, const SDL_Rect *frame) {
	
	self = (TableCellView *) super(View, self, initWithFrame, NULL);
	if (self) {
		self->text = $alloc(Text, initWithText, NULL, NULL);
		assert(self->text);

		self->text->view.alignment = ViewAlignmentMiddleLeft;

		$((View *) self, addSubview, (View *) self->text);

		self->view.autoresizingMask = ViewAutoresizingHeight;

		self->view.padding.top = DEFAULT_TABLE_CELL_VIEW_PADDING;
		self->view.padding.left = DEFAULT_TABLE_CELL_VIEW_PADDING;
		self->view.padding.right = DEFAULT_TABLE_CELL_VIEW_PADDING;
		self->view.padding.bottom = DEFAULT_TABLE_CELL_VIEW_PADDING;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((TableCellViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _TableCellView = {
	.name = "TableCellView",
	.superclass = &_View,
	.instanceSize = sizeof(TableCellView),
	.interfaceOffset = offsetof(TableCellView, interface),
	.interfaceSize = sizeof(TableCellViewInterface),
	.initialize = initialize,
};

#undef _Class

