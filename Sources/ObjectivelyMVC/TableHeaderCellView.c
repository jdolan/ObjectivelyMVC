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

#include "TableHeaderCellView.h"

#define _Class _TableHeaderCellView

#pragma mark - TableHeaderCellView

/**
 * @fn TableHeaderCellView *TableHeaderCellView::initWithFrame(TableHeaderCellView *self, const SDL_Rect *frame)
 * @memberof TableHeaderCellView
 */
static TableHeaderCellView *initWithFrame(TableHeaderCellView *self, const SDL_Rect *frame) {
	return (TableHeaderCellView *) super(TableCellView, self, initWithFrame, frame);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	((TableHeaderCellViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *TableHeaderCellView::_TableHeaderCellView(void)
 * @memberof TableHeaderCellView
 */
Class *_TableHeaderCellView(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "TableHeaderCellView",
			.superclass = _TableCellView(),
			.instanceSize = sizeof(TableHeaderCellView),
			.interfaceOffset = offsetof(TableHeaderCellView, interface),
			.interfaceSize = sizeof(TableHeaderCellViewInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

