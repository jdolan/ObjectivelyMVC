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

#include <ObjectivelyMVC/TableView.h>

#define _Class _TableView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	TableView *this = (TableView *) self;

	release(this->columns);
	
	super(Object, self, dealloc);
}

#pragma mark - TableView

/**
 * @fn TableView *TableView::initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof TableView
 */
static TableView *initWithFrame(TableView *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (TableView *) super(ScrollView, self, initWithFrame, frame, style);
	if (self) {
		
		//..
	}
	
	return self;
}

/**
 * @fn void TableView::reloadData(TableView *self)
 *
 * @brief Reloads this TableView's visible rows.
 *
 * @memberof TableView
 */
static void reloadData(TableView *self) {

}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	
	((TableViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
	
	((TableViewInterface *) clazz->interface)->reloadData = reloadData;
}

Class _TableView = {
	.name = "TableView",
	.superclass = &_ScrollView,
	.instanceSize = sizeof(TableView),
	.interfaceOffset = offsetof(TableView, interface),
	.interfaceSize = sizeof(TableViewInterface),
	.initialize = initialize,
};

#undef _Class

