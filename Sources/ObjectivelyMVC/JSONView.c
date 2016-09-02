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

#include <Objectively.h>

#include <ObjectivelyMVC.h>

#define _Class _JSONView

#pragma mark - JSONView


/**
 * @fn SDL_Color JSONView::colorWithPath(const ident root, const char *path)
 *
 * memberof JSONView
 */
static SDL_Color colorWithPath(const ident root, const char *path) {

	SDL_Color color = Colors.Clear;

	const Array *array = $$(JSONPath, objectWithPath, root, path);
	if (array) {
		assert(array->count == 4);

		const Number *r = $(array, objectAtIndex, 0);
		const Number *g = $(array, objectAtIndex, 1);
		const Number *b = $(array, objectAtIndex, 2);
		const Number *a = $(array, objectAtIndex, 3);

		#define ScaleColor(c) (c > 0.0 && c < 1.0 ? c * 255 : c)

		color.r = ScaleColor(r->value);
		color.g = ScaleColor(g->value);
		color.b = ScaleColor(b->value);
		color.a = ScaleColor(a->value);
	}

	return color;
}

/**
 * @fn Padding JSONView::paddingWithPath(const ident root, const char *path)
 *
 * memberof JSONView
 */
static Padding paddingWithPath(const ident root, const char *path) {

	Padding padding = { .top = 0, .right = 0, .bottom = 0, .left = 0 };

	const Array *array = $$(JSONPath, objectWithPath, root, path);
	if (array) {
		assert(array->count == 4);

		const Number *top = $(array, objectAtIndex, 0);
		const Number *right = $(array, objectAtIndex, 1);
		const Number *bottom = $(array, objectAtIndex, 2);
		const Number *left = $(array, objectAtIndex, 3);

		padding.top = top->value;
		padding.right = right->value;
		padding.bottom = bottom->value;
		padding.left = left->value;
	}

	return padding;
}

/**
 * @fn SDL_Rect JSONView::rectWithPath(const ident root, const char *path)
 *
 * memberof JSONView
 */
static SDL_Rect rectWithPath(const ident root, const char *path) {

	SDL_Rect rect = { .x = 0, .y = 0, .w = 0, .h = 0 };

	const Array *array = $$(JSONPath, objectWithPath, root, path);
	if (array) {
		assert(array->count == 4);

		const Number *x = $(array, objectAtIndex, 0);
		const Number *y = $(array, objectAtIndex, 1);
		const Number *w = $(array, objectAtIndex, 2);
		const Number *h = $(array, objectAtIndex, 3);

		rect.x = x->value, rect.y = y->value, rect.w = w->value, rect.h = h->value;
	}

	return rect;
}

/**
 * @fn View *JSONView::viewWithContentsOfFile(const char *path, Outlet *outlets)
 *
 * @memberof JSONView
 */
static View *viewWithContentsOfFile(const char *path, Outlet *outlets) {

	Data *data = $$(Data, dataWithContentsOfFile, path);

	View *view = $$(JSONView, viewWithData, data, outlets);

	release(data);

	return view;
}

/**
 * @fn View *JSONView::viewWithData(const Data *data, Outlet *outlets)
 *
 * @memberof JSONView
 */
static View *viewWithData(const Data *data, Outlet *outlets) {

	Dictionary *dictionary = $$(JSONSerialization, objectFromData, data, 0);

	View *view = $$(JSONView, viewWithDictionary, dictionary, outlets);

	release(dictionary);
	
	return view;
}

/**
 * @fn View *JSONView::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
 *
 * @memberof JSONView
 */
static View *viewWithDictionary(const Dictionary *dictionary, Outlet *outlets) {

	String *string = $$(JSONPath, objectWithPath, (ident) dictionary, "$.class");
	if (string) {
		Class *clazz = classFromString(string->chars);
		if (clazz) {
			const Class *c = clazz;
			while (c) {
				if (c == &_View) {
					return $((View *) _alloc(clazz), initWithDictionary, dictionary, outlets);
				}
				c = c->superclass;
			}
			MVC_LogError("Class %s does not extend View\n", string->chars);
		} else {
			MVC_LogError("Class %s not found. Did you _initialize it?\n", string->chars);
		}
	} else {
		MVC_LogError("View Class not specified\n");
	}
	
	return NULL;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	_initialize(&_Box);
	_initialize(&_Button);
	_initialize(&_Checkbox);
	_initialize(&_CollectionView);
	_initialize(&_ImageView);
	_initialize(&_Input);
	_initialize(&_Label);
	_initialize(&_Panel);
	_initialize(&_ScrollView);
	_initialize(&_Select);
	_initialize(&_Slider);
	_initialize(&_StackView);
	_initialize(&_TableView);
	_initialize(&_TextView);
	_initialize(&_View);

	((JSONViewInterface *) clazz->interface)->colorWithPath = colorWithPath;
	((JSONViewInterface *) clazz->interface)->paddingWithPath = paddingWithPath;
	((JSONViewInterface *) clazz->interface)->rectWithPath = rectWithPath;
	((JSONViewInterface *) clazz->interface)->viewWithContentsOfFile = viewWithContentsOfFile;
	((JSONViewInterface *) clazz->interface)->viewWithData = viewWithData;
	((JSONViewInterface *) clazz->interface)->viewWithDictionary = viewWithDictionary;
}

Class _JSONView = {
	.name = "JSONView",
	.superclass = &_Object,
	.instanceSize = sizeof(JSONView),
	.interfaceOffset = offsetof(JSONView, interface),
	.interfaceSize = sizeof(JSONViewInterface),
	.initialize = initialize,
};

#undef _Class

