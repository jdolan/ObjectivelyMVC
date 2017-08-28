/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>

#include <Objectively.h>

#include <ObjectivelyMVC.h>

/**
 * @brief InletBinding for InletTypeBool.
 */
static void bindBool(const Inlet *inlet, ident obj) {
	*((_Bool *) inlet->dest) = cast(Boole, obj)->value;
}

/**
 * @brief InletBinding for InletTypeCharacters.
 */
static void bindCharacters(const Inlet *inlet, ident obj) {
	*((char **) inlet->dest) = strdup(cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeColor.
 */
static void bindColor(const Inlet *inlet, ident obj) {

	const Array *array = cast(Array, obj);

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

	*((SDL_Color *) inlet->dest) = color;
}

/**
 * @brief InletBinding for InletTypeConstraint.
 */
static void bindConstraint(const Inlet *inlet, ident obj) {

	String *descriptor = cast(String, obj);

	*(Constraint **) inlet->dest = $(alloc(Constraint), initWithDescriptor, descriptor->chars);
}

/**
 * @brief ArrayEnumerator for `bindConstraints`.
 */
static void bindConstraints_enumerate(const Array *array, ident obj, ident data) {

	Constraint *constraint = NULL;

	const Inlet inlet = MakeInlet(NULL, InletTypeConstraint, &constraint, NULL);

	bindConstraint(&inlet, obj);

	$(cast(View, data), addConstraint, constraint);

	release(constraint);
}

/**
 * @brief InletBinding for InletTypeConstraints.
 */
static void bindConstraints(const Inlet *inlet, ident obj) {
	$(cast(Array, obj), enumerateObjects, bindConstraints_enumerate, *(View **) inlet->dest);
}

/**
 * @brief InletBinding for InletTypeDouble.
 */
static void bindDouble(const Inlet *inlet, ident obj) {
	*((double *) inlet->dest) = cast(Number, obj)->value;
}

/**
 * @brief InletBinding for InletTypeEnum.
 */
static void bindEnum(const Inlet *inlet, ident obj) {
	*((int *) inlet->dest) = valueof(inlet->data, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeFloat.
 */
static void bindFloat(const Inlet *inlet, ident obj) {
	*((float *) inlet->dest) = cast(Number, obj)->value;
}

/**
 * @brief InletBinding for InletTypeFont.
 */
static void bindFont(const Inlet *inlet, ident obj) {
	*((Font **) inlet->dest) = $(alloc(Font), initWithName, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeImage.
 */
static void bindImage(const Inlet *inlet, ident obj) {
	*((Image **) inlet->dest) = $(alloc(Image), initWithName, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeInteger.
 */
static void bindInteger(const Inlet *inlet, ident obj) {
	*((int *) inlet->dest) = cast(Number, obj)->value;
}

/**
 * @brief InletBinding for InletTypeRectangle.
 */
static void bindRectangle(const Inlet *inlet, ident obj) {

	const Array *array = cast(Array, obj);

	assert(array->count == 4);

	const Number *x = $(array, objectAtIndex, 0);
	const Number *y = $(array, objectAtIndex, 1);
	const Number *w = $(array, objectAtIndex, 2);
	const Number *h = $(array, objectAtIndex, 3);

	*((SDL_Rect *) inlet->dest) = MakeRect(x->value, y->value, w->value, h->value);
}

/**
 * @brief InletBinding for InletTypeSize.
 */
static void bindSize(const Inlet *inlet, ident obj) {

	const Array *array = cast(Array, obj);

	assert(array->count == 2);

	const Number *w = $(array, objectAtIndex, 0);
	const Number *h = $(array, objectAtIndex, 1);

	*((SDL_Size *) inlet->dest) = MakeSize(w->value, h->value);
}

/**
 * @brief Binds the given View with the specified Dictionary.
 */
static void bindView(const Inlet *inlet, ident obj) {

	View *view = NULL, *source = *(View **) inlet->dest;

	const Dictionary *dictionary = cast(Dictionary, obj);

	const String *clazzName = $(dictionary, objectForKeyPath, "class");
	const String *includePath = $(dictionary, objectForKeyPath, "include");

	if (clazzName == NULL && includePath == NULL) {
		$(source, awakeWithDictionary, dictionary);
	} else {
		if (clazzName) {
			Class *clazz = classForName(clazzName->chars);
			if (clazz) {
				const Class *c = clazz;
				while (c) {
					if (c == _View()) {
						break;
					}
					c = c->superclass;
				}
				assert(c);

				view = $((View *) _alloc(clazz), init);
				$(view, awakeWithDictionary, dictionary);
			}
		} else if (includePath) {
			view = $$(View, viewWithContentsOfFile, includePath->chars, NULL);
		}

		assert(view);

		if (source) {
			if (source->superview) {
				$(source->superview, replaceSubview, source, view);
			}
			release(source);
		}

		*(View **) inlet->dest = view;
	}
}

/**
 * @brief ArrayEnumerator for bind subview recursion.
 */
static void bindSubviews_enumerate(const Array *array, ident obj, ident data) {

	View *subview = NULL;

	const Inlet inlet = MakeInlet(NULL, InletTypeView, &subview, NULL);

	bindView(&inlet, obj);

	$(cast(View, data), addSubview, subview);

	release(subview);
}

/**
 * @brief InletBinding for InletTypeSubviews.
 */
static void bindSubviews(const Inlet *inlet, ident obj) {
	$(cast(Array, obj), enumerateObjects, bindSubviews_enumerate, *(View **) inlet->dest);
}

/**
 * @brief InletBinding for InletTypeApplicationDefined.
 */
static void bindApplicationDefined(const Inlet *inlet, ident obj) {

	assert(inlet->data);

	((InletBinding) inlet->data)(inlet, obj);
}

/**
 * @brief The array of InletBinding functions, indexed by InletType.
 */
const InletBinding inletBindings[] = {
	bindBool,
	bindCharacters,
	bindColor,
	bindConstraint,
	bindConstraints,
	bindDouble,
	bindEnum,
	bindFloat,
	bindFont,
	bindImage,
	bindInteger,
	bindRectangle,
	bindSize,
	bindSubviews,
	bindView,
	bindApplicationDefined,
};

void bindInlets(const Inlet *inlets, const Dictionary *dictionary) {

	assert(inlets);
	assert(dictionary);

	for (const Inlet *inlet = inlets; inlet->name; inlet++) {
		const ident obj = $(dictionary, objectForKeyPath, inlet->name);
		if (obj) {
			BindInlet(inlet, obj);
		}
	}
}
