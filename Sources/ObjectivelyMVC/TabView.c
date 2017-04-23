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

#include <Objectively/String.h>

#include <ObjectivelyMVC/TabView.h>

#define _Class _TabView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	TabView *this = (TabView *) self;

	release(this->tabs);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @brief ArrayEnumerator for awaking TabViewItems.
 */
static void awakeWithDictionary_tabs(const Array *array, ident obj, ident data) {

	const String *identifier = $((Dictionary *) obj, objectForKeyPath, "identifier");
	assert(identifier);

	TabViewItem *tab = $(alloc(TabViewItem), initWithIdentifier, identifier->chars);
	assert(tab);

	const Inlet inlets[] = MakeInlets(
		MakeInlet("label", InletTypeView, &tab->label, NULL),
		MakeInlet("view", InletTypeView, &tab->view, NULL)
	);

	$((View *) data, bind, obj, inlets);
	$((TabView *) data, addTab, tab);

	release(tab);
}

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	const Array *tabs = $(dictionary, objectForKeyPath, "tabs");
	if (tabs) {
		$(tabs, enumerateObjects, awakeWithDictionary_tabs, self);
	}
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
 * @fn void TabView::addTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void addTab(TabView *self, TabViewItem *tab) {

	assert(tab);
	assert(tab->view);

	$(self->tabs, addObject, tab);
	$((View *) self, addSubview, tab->view);

	if (self->delegate.didAddTab) {
		self->delegate.didAddTab(self, tab);
	}

	if (self->selectedTab == NULL) {
		$(self, selectTab, tab);
	}
}

/**
 * @fn TabView *TabView::initWithFrame(TabView *self, const SDL_Rect *frame)
 * @memberof TabView
 */
static TabView *initWithFrame(TabView *self, const SDL_Rect *frame) {

	self = (TabView *) super(View, self, initWithFrame, frame);
	if (self) {

		self->tabs = $$(MutableArray, array);
		assert(self->tabs);

		self->view.autoresizingMask = ViewAutoresizingContain;

		self->view.padding.top = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.right = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.bottom = DEFAULT_TAB_VIEW_PADDING;
		self->view.padding.left = DEFAULT_TAB_VIEW_PADDING;
	}

	return self;
}

/**
 * @fn void TabView::removeTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void removeTab(TabView *self, TabViewItem *tab) {

	assert(tab);
	assert(tab->view);

	if (self->delegate.willRemoveTab) {
		self->delegate.willRemoveTab(self, tab);
	}

	if (self->selectedTab == tab) {
		$(self, selectTab, NULL);
	}

	$(self->tabs, removeObject, tab);
	$((View *) self, removeSubview, tab->view);
}

/**
 * @fn void TabView::selectTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void selectTab(TabView *self, TabViewItem *tab) {

	self->selectedTab = tab ?: $((Array *) self->tabs, firstObject);

	if (self->selectedTab) {
		self->selectedTab->state |= TabStateSelected;

		if (self->delegate.didSelectTab) {
			self->delegate.didSelectTab(self, self->selectedTab);
		}
	}
}

/**
 * @brief NULL-safe Predicate for tabWithIdentifier.
 */
static _Bool tabWithIdentifier_predicate(const ident obj, ident data) {

	const TabViewItem *tab = obj;

	if (tab->identifier) {
		if (data) {
			return !strcmp(tab->identifier, data);
		} else {
			return false;
		}
	} else {
		return data == NULL;
	}
}

/**
 * @fn TabViewItem *TabView::tabWithIdentifier(const TabView *self, const char *identifier)
 * @memberof TabView
 */
static TabViewItem *tabWithIdentifier(const TabView *self, const char *identifier) {
	return $((Array *) self->tabs, findObject, tabWithIdentifier_predicate, (ident) identifier);
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

	((TabViewInterface *) clazz->def->interface)->addTab = addTab;
	((TabViewInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((TabViewInterface *) clazz->def->interface)->removeTab = removeTab;
	((TabViewInterface *) clazz->def->interface)->selectTab = selectTab;
	((TabViewInterface *) clazz->def->interface)->tabWithIdentifier = tabWithIdentifier;
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
