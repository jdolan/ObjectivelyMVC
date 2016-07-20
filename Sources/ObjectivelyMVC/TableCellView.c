//
//  TableCellView.c
//  ObjectivelyMVC
//
//  Created by Jay Dolan on 2/29/16.
//  Copyright © 2016 https://github.com/jdolan. All rights reserved.
//

#include <assert.h>

#include <ObjectivelyMVC/TableCellView.h>

#define _Class _TableCellView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	//..
	
	super(Object, self, dealloc);
}

#pragma mark - TableCellView

/**
 * @fn TableCellView *TableCellView::initWithValue(TableCellView *self, ident value)
 *
 * @memberof TableCellView
 */
static TableCellView *initWithValue(TableCellView *self, ident value) {
	
	self = (TableCellView *) super(Control, self, initWithFrame, NULL, ControlStyleDefault);
	if (self) {
		
		//..
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
	
	((TableCellViewInterface *) clazz->interface)->initWithValue = initWithValue;
	
	//..
}

Class _TableCellView = {
	.name = "TableCellView",
	.superclass = &_Control,
	.instanceSize = sizeof(TableCellView),
	.interfaceOffset = offsetof(TableCellView, interface),
	.interfaceSize = sizeof(TableCellViewInterface),
	.initialize = initialize,
};

#undef _Class

