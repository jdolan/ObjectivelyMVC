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
#include <string.h>

#include <Objectively.h>

#include <ObjectivelyMVC.h>

#define _Class _View

/**
 * @brief InletBinding for InletTypeBool.
 */
static void bindBool(const Inlet *inlet, ident obj) {
  *((bool *) inlet->dest) = cast(Boole, obj)->value;
}

/**
 * @brief InletBinding for InletTypeCharacters.
 */
static void bindCharacters(const Inlet *inlet, ident obj) {

  char **dest = inlet->dest;

  if (*dest) {
    free(*dest);
  }

  *dest = strdup(cast(String, obj)->chars);
}

/**
 * @brief ArrayEnumerator for bindClassNames.
 */
static void bindClassNames_enumerate(const Array *array, ident obj, ident data) {
  $((View *) data, addClassName, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeClassNames.
 */
static void bindClassNames(const Inlet *inlet, ident obj) {
  $(cast(Array, obj), enumerateObjects, bindClassNames_enumerate, *(View **) inlet->dest);
}

/**
 * @brief InletBinding for InletTypeColor.
 */
static void bindColor(const Inlet *inlet, ident obj) {

  SDL_Color color = Colors.Black;

  if ($((Object *) obj, isKindOfClass, _String())) {

    String *string = cast(String, obj);
    if (string->length) {

      if (strcmp("none", string->chars) == 0) {
        return;
      }

      if (string->chars[0] == '#') {
        color = MVC_HexToRGBA(string->chars + 1);
      } else {
        color = MVC_ColorForName(string->chars);
      }
    }

  } else {
    const Array *array = cast(Array, obj);

    assert(array->count == 4);

    const Number *r = $(array, objectAtIndex, 0);
    const Number *g = $(array, objectAtIndex, 1);
    const Number *b = $(array, objectAtIndex, 2);
    const Number *a = $(array, objectAtIndex, 3);

    #define ScaleColor(c) (c > 0.0 && c < 1.0 ? c * 255 : c)

    color = MakeColor(
      ScaleColor(r->value),
      ScaleColor(g->value),
      ScaleColor(b->value),
      ScaleColor(a->value)
    );
  }

  *((SDL_Color *) inlet->dest) = color;
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

  release(*(Font **) inlet->dest);

  // TODO

  //*((Font **) inlet->dest) = $(alloc(Font), initWithName, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeImage.
 */
static void bindImage(const Inlet *inlet, ident obj) {

  release(*(Image **) inlet->dest);

  *((Image **) inlet->dest) = $(alloc(Image), initWithResourceName, cast(String, obj)->chars);
}

/**
 * @brief InletBinding for InletTypeInteger.
 */
static void bindInteger(const Inlet *inlet, ident obj) {
  *((int *) inlet->dest) = cast(Number, obj)->value;
}

/**
 * @brief InletBinding for InletTypePoint.
 */
static void bindPoint(const Inlet *inlet, ident obj) {

  const Array *array = cast(Array, obj);

  assert(array->count == 2);

  const Number *x = $(array, objectAtIndex, 0);
  const Number *y = $(array, objectAtIndex, 1);

  *((SDL_Point *) inlet->dest) = MakePoint(x->value, y->value);
}

/**
 * @brief InletBinding for InletTypeRectangle.
 */
static void bindRectangle(const Inlet *inlet, ident obj) {

  if ($((Object *) obj, isKindOfClass, _Number())) {
    const Number *n = obj;

    *((SDL_Rect *) inlet->dest) = MakeRect(n->value, n->value, n->value, n->value);
  } else {
    const Array *array = cast(Array, obj);

    assert(array->count == 4);

    const Number *x = $(array, objectAtIndex, 0);
    const Number *y = $(array, objectAtIndex, 1);
    const Number *w = $(array, objectAtIndex, 2);
    const Number *h = $(array, objectAtIndex, 3);

    *((SDL_Rect *) inlet->dest) = MakeRect(x->value, y->value, w->value, h->value);
  }
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

  View *dest = *(View **) inlet->dest;

  const Dictionary *dictionary = cast(Dictionary, obj);

  const String *className = $(dictionary, objectForKeyPath, "class");
  if (className || dest == NULL) {

    Class *clazz = className ? classForName(className->chars) : _View();
    assert(clazz);

    const Class *c = clazz;
    while (c) {
      if (c == _View()) {
        break;
      }
      c = c->def.superclass;
    }
    assert(c);

    if (clazz != _View()) {
      if (interfaceof(View, clazz)->init == interfaceof(View, clazz->def.superclass)->init) {
        MVC_LogWarn("%s does not implement View::init\n", clazz->def.name);
      }
    }

    View *view = $((View *) _alloc(clazz), init);
    assert(view);

    $(view, awakeWithDictionary, dictionary);

    if (dest) {
      if (dest->superview) {
        $(dest->superview, replaceSubview, dest, view);
      }
      release(dest);
    }

    *(View **) inlet->dest = view;

  } else if (dest) {
    $(dest, awakeWithDictionary, dictionary);
  } else {
    MVC_LogWarn("Inlet %s has NULL destination and no className specified\n", inlet->name);
  }
}

/**
 * @brief InletBinding for InletTypeStyle.
 */
static void bindStyle(const Inlet *inlet, ident obj) {
  $(cast(View, *((View **) inlet->dest))->style, addAttributes, cast(Dictionary, obj));
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
  bindClassNames,
  bindColor,
  bindDouble,
  bindEnum,
  bindFloat,
  bindFont,
  bindImage,
  bindInteger,
  bindPoint,
  bindRectangle,
  bindSize,
  bindStyle,
  bindSubviews,
  bindView,
  bindApplicationDefined,
};

bool bindInlets(const Inlet *inlets, const Dictionary *dictionary) {

  assert(inlets);
  assert(dictionary);

  bool didBindInlets = false;

  for (const Inlet *inlet = inlets; inlet->name; inlet++) {
    const ident obj = $(dictionary, objectForKeyPath, inlet->name);
    if (obj) {
      BindInlet(inlet, obj);
      didBindInlets = true;
    }
  }

  return didBindInlets;
}

#undef _Class
