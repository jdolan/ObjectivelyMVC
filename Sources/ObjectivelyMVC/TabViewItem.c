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

#include <ObjectivelyMVC/TabViewItem.h>

#define _Class _TabViewItem

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TabViewItem *this = (TabViewItem *) self;

	free(this->identifier);

	release(this->label);
	release(this->view);

	super(Object, self, dealloc);
}

#pragma mark - TabViewItem

/**
 * @fn TabViewItem *TabViewItem::initWithIdentifier(TabViewItem *self, const char *identifier)
 * @memberof TabViewItem
 */
static TabViewItem *initWithIdentifier(TabViewItem *self, const char *identifier) {

	self = (TabViewItem *) super(Object, self, init);
	if (self) {
		self->identifier = strdup(identifier);
		assert(self->identifier);

		self->label = $(alloc(Label), initWithText, self->identifier, NULL);
		assert(self->label);

		self->label->text->view.alignment = ViewAlignmentMiddleCenter;
		
		self->label->view.padding.top = DEFAULT_TAB_VIEW_ITEM_LABEL_PADDING;
		self->label->view.padding.bottom = DEFAULT_TAB_VIEW_ITEM_LABEL_PADDING;

		self->view = $(alloc(View), initWithFrame, NULL);
		assert(self->view);

		self->view->autoresizingMask = ViewAutoresizingContain;

		$(self, setState, TabStateDefault);
	}

	return self;
}

/**
 * @fn TabViewItem *TabViewItem::initWithView(TabViewItem *self, View *view)
 * @memberof TabViewItem
 */
static TabViewItem *initWithView(TabViewItem *self, View *view) {

	self = $(self, initWithIdentifier, view->identifier ?: _TabViewItem()->name);
	if (self) {
		release(self->view);
		self->view = retain(view);
	}

	return self;
}

/**
 * @fn TabViewItem::setState(TabViewItem *self, int state)
 * @memberof TabViewItem
 */
static void setState(TabViewItem *self, int state) {

	self->state = state;

	if (self->state & TabStateSelected) {
		self->label->view.backgroundColor = Colors.AlternateColor;
	} else {
		self->label->view.backgroundColor = Colors.Clear;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((TabViewItemInterface *) clazz->def->interface)->initWithIdentifier = initWithIdentifier;
	((TabViewItemInterface *) clazz->def->interface)->initWithView = initWithView;
	((TabViewItemInterface *) clazz->def->interface)->setState = setState;
}

/**
 * @fn Class *TabViewItem::_TabViewItem(void)
 * @memberof TabViewItem
 */
Class *_TabViewItem(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "TabViewItem";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(TabViewItem);
		clazz.interfaceOffset = offsetof(TabViewItem, interface);
		clazz.interfaceSize = sizeof(TabViewItemInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
