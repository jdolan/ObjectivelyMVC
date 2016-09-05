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

/**
 * @return The SDL_Color for `array`.
 */
static SDL_Color colorForArray(const Array *array) {

	assert(array);
	assert(array->count == 4);

	const Number *r = $(array, objectAtIndex, 0);
	const Number *g = $(array, objectAtIndex, 1);
	const Number *b = $(array, objectAtIndex, 2);
	const Number *a = $(array, objectAtIndex, 3);

#define ScaleColor(c) (c > 0.0 && c < 1.0 ? c * 255 : c)

	SDL_Color color = {
		ScaleColor(r->value),
		ScaleColor(g->value),
		ScaleColor(b->value),
		ScaleColor(a->value)
	};

	return color;
}

/**
 * @return The SDL_Size for `array`.
 */
static SDL_Size sizeForArray(const Array *array) {

	assert(array);
	assert(array->count == 2);

	const Number *w = $(array, objectAtIndex, 2);
	const Number *h = $(array, objectAtIndex, 3);

	SDL_Size size = { w->value, h->value };
	return size;
}

/**
 * @return The SDL_Rect for `array`.
 */
static SDL_Rect rectForArray(const Array *array) {

	assert(array);
	assert(array->count == 4);

	const Number *x = $(array, objectAtIndex, 0);
	const Number *y = $(array, objectAtIndex, 1);
	const Number *w = $(array, objectAtIndex, 2);
	const Number *h = $(array, objectAtIndex, 3);

	SDL_Rect rect = { x->value, y->value, w->value, h->value };
	return rect;
}

static __thread Outlet *_outlets;

/**
 * @brief ArrayEnumerator for subview recursion.
 */
static _Bool viewWithDictionary_recurse(const Array *array, ident obj, ident data) {

	View *subview = $$(JSONView, viewWithDictionary, obj, _outlets);

	$((View *) data, addSubview, subview);

	release(subview);

	return false;
}

#pragma mark - JSONView

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
 * @fn void JSONView::applyInlets(View *view, const Dictionary *dictionary, const Inlet *inlets)
 *
 * @memberof JSONView
 */
static void applyInlets(View *view, const Dictionary *dictionary, const Inlet *inlets) {

	const Inlet *inlet = inlets;
	while (inlet->name) {

		const ident obj = $(dictionary, objectForKeyPath, inlet->name);
		if (obj) {

			ident dest = ((ident) view) + inlet->offset;
			switch (inlet->type) {
				case InletTypeBool:
					*((_Bool *) dest) = cast(Boole, obj)->value;
					break;
				case InletTypeCharacters:
					*((char **) dest) = strdup(cast(String, obj)->chars);
					break;
				case InletTypeColor:
					*((SDL_Color *) dest) = colorForArray(cast(Array, obj));
					break;
				case InletTypeEnum:
					*((int *) dest) = valueof(inlet->data, (cast(String, obj))->chars);
					break;
				case InletTypeInteger:
					*((int *) dest) = cast(Number, obj)->value;
					break;
				case InletTypeRectangle:
					*((SDL_Rect *) dest) = rectForArray(cast(Array, obj));
					break;
				case InletTypeSize:
					*((SDL_Size *) dest) = sizeForArray(cast(Array, obj));
					break;
				case InletTypeViewArray:
					$(cast(Array, obj), enumerateObjects, viewWithDictionary_recurse, inlet->data ?: view);
					break;
			}
		}

		inlet++;
	}
}

/**
 * @fn View *JSONView::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
 *
 * @memberof JSONView
 */
static View *viewWithDictionary(const Dictionary *dictionary, Outlet *outlets) {

	_outlets = outlets;

	String *string = $$(JSONPath, objectForKeyPath, (ident) dictionary, "$.class");
	if (string) {
		Class *clazz = classFromString(string->chars);
		if (clazz) {
			const Class *c = clazz;
			while (c) {
				if (c == &_View) {
					View *view = $((View *) _alloc(clazz), init);

					$(view, awakeWithDictionary, dictionary);

					const Array *subviews = $(dictionary, objectForKeyPath, "subviews");
					if (subviews) {
						$(subviews, enumerateObjects, viewWithDictionary_recurse, view);
					}

					if (view->identifier) {
						for (Outlet *outlet = _outlets; outlet->identifier; outlet++) {
							if (strcmp(outlet->identifier, view->identifier) == 0) {
								*outlet->view = view;
							}
						}
					}
					
					return view;
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

	((JSONViewInterface *) clazz->interface)->applyInlets = applyInlets;
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
