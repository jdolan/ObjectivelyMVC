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

#include <ObjectivelyMVC/TableHeaderView.h>

#define _Class _TableHeaderView

#pragma mark - TableHeaderView

/**
 * @fn TableHeaderView *TableHeaderView::initWithFrame(TableHeaderView *self, const SDL_Rect *frame)
 *
 * @memberof TableHeaderView
 */
static TableHeaderView *initWithFrame(TableHeaderView *self, const SDL_Rect *frame) {
	
	self = (TableHeaderView *) super(View, self, initWithFrame, frame);
	if (self) {
		if (((View *) self)->frame.h == 0) {
			((View *) self)->frame.h = DEFAULT_TABLE_HEADER_VIEW_HEIGHT;
		}
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((TableHeaderViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _TableHeaderView = {
	.name = "TableHeaderView",
	.superclass = &_View,
	.instanceSize = sizeof(TableHeaderView),
	.interfaceOffset = offsetof(TableHeaderView, interface),
	.interfaceSize = sizeof(TableHeaderViewInterface),
	.initialize = initialize,
};

#undef _Class

