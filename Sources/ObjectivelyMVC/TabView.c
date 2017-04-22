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

#include <ObjectivelyMVC/TabView.h>

#define _Class _TabView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TabView *this = (TabView *) self;

	release(this->tabViewItems);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	// TODO: JSON binding
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((TabView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

	super(View, self, layoutSubviews);

	// TODO: Layout subviews
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	super(View, self, respondToEvent, event);

	// TODO: Tab selection and visibility toggling
}

#pragma mark - TabView

/**
 * @fn void TabView::addTabViewItem(TabView *self, TabViewItem *tabViewItem)
 * @memberof TabView
 */
static void addTabViewItem(TabView *self, TabViewItem *tabViewItem) {

	assert(tabViewItem);
	assert(tabViewItem->view);

	$(self->tabViewItems, addObject, tabViewItem);
	$((View *) self, addSubview, tabViewItem->view);

	if (self->delegate.didAddTabViewItem) {
		self->delegate.didAddTabViewItem(self, tabViewItem);
	}

	if (self->selectedTabViewItem == NULL) {
		$(self, selectTabViewItem, tabViewItem);
	}
}

/**
 * @fn TabView *TabView::initWithFrame(TabView *self, const SDL_Rect *frame)
 * @memberof TabView
 */
static TabView *initWithFrame(TabView *self, const SDL_Rect *frame) {

	self = (TabView *) super(View, self, initWithFrame, frame);
	if (self) {

		self->tabViewItems = $$(MutableArray, array);
		assert(self->tabViewItems);

		self->view.autoresizingMask = ViewAutoresizingContain;

		self->view.padding.top = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.right = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.bottom = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.left = DEFAULT_TAB_VIEW_PADDING;
	}

	return self;
}

/**
 * @fn void TabView::selectTabViewItem(TabView *self, TabViewItem *tabViewItem)
 * @memberof TabView
 */
static void selectTabViewItem(TabView *self, TabViewItem *tabViewItem) {

	self->selectedTabViewItem = tabViewItem ?: $((Array *) self->tabViewItems, firstObject);

	if (self->selectedTabViewItem) {
		self->selectedTabViewItem->state |= TabStateSelected;

		if (self->delegate.didSelectTabViewItem) {
			self->delegate.didSelectTabViewItem(self, self->selectedTabViewItem);
		}
	}
}

/**
 * @fn void TabView::removeTabViewItem(TabView *self, TabViewItem *tabViewItem)
 * @memberof TabView
 */
static void removeTabViewItem(TabView *self, TabViewItem *tabViewItem) {

	assert(tabViewItem);
	assert(tabViewItem->view);

	if (self->delegate.willRemoveTabViewItem) {
		self->delegate.willRemoveTabViewItem(self, tabViewItem);
	}

	if (self->selectedTabViewItem == tabViewItem) {
		$(self, selectTabViewItem, NULL);
	}
	
	$(self->tabViewItems, removeObject, tabViewItem);
	$((View *) self, removeSubview, tabViewItem->view);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->def->interface)->respondToEvent = respondToEvent;

	((TabViewInterface *) clazz->def->interface)->addTabViewItem = addTabViewItem;
	((TabViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((TabViewInterface *) clazz->def->interface)->removeTabViewItem = removeTabViewItem;
	((TabViewInterface *) clazz->def->interface)->selectTabViewItem = selectTabViewItem;
}

/**
 * @fn Class *TabView::_TabView(void)
 * @memberof TabView
 */
Class *_TabView(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "TabView";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(TabView);
		clazz.interfaceOffset = offsetof(TabView, interface);
		clazz.interfaceSize = sizeof(TabViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
