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

	release(this->contentView);
	release(this->tabSelectionView);
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

	TabView *this = (TabView *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("contentView", InletTypeView, &this->contentView, NULL),
		MakeInlet("tabSelectionView", InletTypeView, &this->tabSelectionView, NULL)
	);

	$(self, bind, dictionary, inlets);

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
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	super(View, self, respondToEvent, event);

	switch (event->type) {
		case SDL_MOUSEBUTTONUP:
		case SDL_FINGERUP:
			break;
		default:
			return;
	}

	TabView *this = (TabView *) self;

	const Array *tabs = (Array *) this->tabs;
	for (size_t i = 0; i < tabs->count; i++) {

		TabViewItem *tab = $(tabs, objectAtIndex, i);

		if ($((View *) tab->label, didReceiveEvent, event)) {
			$(this, selectTab, tab);
		}
	}
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	TabView *this = (TabView *) self;

	const SDL_Size contentSize = $(this, contentSize);

	$(this->contentView, resize, &contentSize);

	return super(View, self, sizeThatFits);
}

#pragma mark - TabView

/**
 * @fn void TabView::addTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void addTab(TabView *self, TabViewItem *tab) {

	assert(tab);

	$(self->tabs, addObject, tab);

	$((View *) self->tabSelectionView, addSubview, (View *) tab->label);

	if (self->delegate.didAddTab) {
		self->delegate.didAddTab(self, tab);
	}

	if (self->selectedTab == NULL) {
		$(self, selectTab, tab);
	}
}

/**
 * @fn SDL_Size TabView::contentSize(const TabView *self)
 * @memberof TabView
 */
static SDL_Size contentSize(const TabView *self) {

	SDL_Size size = MakeSize(0, 0);

	const Array *tabs = (Array *) self->tabs;
	for (size_t i = 0; i < tabs->count; i++) {

		const TabViewItem *tab = $(tabs, objectAtIndex, i);
		const SDL_Size tabSize = $(tab->view, sizeThatFits);

		SDL_Point tabOrigin = MakePoint(0, 0);
		switch (tab->view->alignment) {
			case ViewAlignmentNone:
				tabOrigin = MakePoint(tab->view->frame.x, tab->view->frame.y);
				break;
			default:
				break;
		}

		size.w = max(size.w, tabOrigin.x + tabSize.w);
		size.h = max(size.h, tabOrigin.y + tabSize.h);
	}

	size.w += self->contentView->padding.left + self->contentView->padding.right;
	size.h += self->contentView->padding.top + self->contentView->padding.bottom;

	return size;
}

/**
 * @fn TabView *TabView::initWithFrame(TabView *self, const SDL_Rect *frame)
 * @memberof TabView
 */
static TabView *initWithFrame(TabView *self, const SDL_Rect *frame) {

	self = (TabView *) super(StackView, self, initWithFrame, frame);
	if (self) {

		self->contentView = $(alloc(View), initWithFrame, NULL);
		assert(self->contentView);

		self->contentView->autoresizingMask = ViewAutoresizingContain;

		self->contentView->borderColor = Colors.DarkGray;
		self->contentView->borderWidth = 1;

		self->contentView->padding.top = self->contentView->padding.bottom = DEFAULT_TAB_VIEW_PADDING;
		self->contentView->padding.left = self->contentView->padding.right = DEFAULT_TAB_VIEW_PADDING;

		self->tabs = $$(MutableArray, array);
		assert(self->tabs);

		self->tabSelectionView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->tabSelectionView);

		self->tabSelectionView->axis = StackViewAxisHorizontal;
		self->tabSelectionView->distribution = StackViewDistributionFillEqually;

		self->tabSelectionView->view.autoresizingMask |= ViewAutoresizingWidth;

		self->stackView.spacing = DEFAULT_TAB_VIEW_SPACING;

		$((View *) self, addSubview, (View *) self->tabSelectionView);
		$((View *) self, addSubview, self->contentView);
	}

	return self;
}

/**
 * @fn void TabView::removeTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void removeTab(TabView *self, TabViewItem *tab) {

	assert(tab);

	if (self->delegate.willRemoveTab) {
		self->delegate.willRemoveTab(self, tab);
	}

	retain(tab);

	$(self->tabs, removeObject, tab);
	
	$((View *) self, removeSubview, tab->view);
	$((View *) self->tabSelectionView, removeSubview, (View *) tab->label);

	if (self->selectedTab == tab) {
		$(self, selectTab, NULL);
	}

	release(tab);
}

/**
 * @brief ArrayEnumerator for selectTab.
 */
static void selectTab_enumerate(const Array *array, ident obj, ident data) {

	TabViewItem *tab = obj;

	if (tab == ((TabView *) data)->selectedTab) {
		tab->state |= TabStateSelected;
		tab->label->view.backgroundColor = Colors.AlternateColor;
	} else {
		tab->state &= ~TabStateSelected;
		tab->label->view.backgroundColor = Colors.Clear;
	}
}

/**
 * @fn void TabView::selectTab(TabView *self, TabViewItem *tab)
 * @memberof TabView
 */
static void selectTab(TabView *self, TabViewItem *tab) {

	if (self->selectedTab != tab) {

		if (self->selectedTab) {
			$(self->contentView, removeSubview, self->selectedTab->view);
		}

		const Array *tabs = (Array *) self->tabs;

		if (tab) {
			self->selectedTab = tab;
		} else {
			self->selectedTab = $(tabs, firstObject);
		}

		$(tabs, enumerateObjects, selectTab_enumerate, self);

		if (self->selectedTab) {
			$(self->contentView, addSubview, self->selectedTab->view);

			if (self->delegate.didSelectTab) {
				self->delegate.didSelectTab(self, self->selectedTab);
			}
		}

		self->stackView.view.needsLayout = true;
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
	((ViewInterface *) clazz->def->interface)->respondToEvent = respondToEvent;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;

	((TabViewInterface *) clazz->def->interface)->addTab = addTab;
	((TabViewInterface *) clazz->def->interface)->contentSize = contentSize;
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
		clazz.superclass = _StackView();
		clazz.instanceSize = sizeof(TabView);
		clazz.interfaceOffset = offsetof(TabView, interface);
		clazz.interfaceSize = sizeof(TabViewInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
