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

#include <ObjectivelyMVC/TableRowView.h>

#define _Class _TableRowView

#pragma mark - TableRowView

/**
 * @fn TableRowView *TableRowView::initWithFrame(TableRowView *self, const SDL_Rect *frame)
 *
 * @memberof TableRowView
 */
static TableRowView *initWithFrame(TableRowView *self, const SDL_Rect *frame) {
	
	self = (TableRowView *) super(StackView, self, initWithFrame, frame);
	if (self) {
		self->stackView.axis = StackViewAxisHorizontal;

		self->stackView.view.autoresizingMask = ViewAutoresizingWidth;
		self->stackView.view.backgroundColor = Colors.DefaultColor;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((TableRowViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _TableRowView = {
	.name = "TableRowView",
	.superclass = &_StackView,
	.instanceSize = sizeof(TableRowView),
	.interfaceOffset = offsetof(TableRowView, interface),
	.interfaceSize = sizeof(TableRowViewInterface),
	.initialize = initialize,
};

#undef _Class

