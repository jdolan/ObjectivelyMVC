/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <check.h>

#include <Objectively.h>

#include "ObjectivelyMVC.h"

START_TEST(style) {
  
  Style *style;

  style = $(alloc(Style), initWithRules, "Control:highlighted, Control:focused");
  ck_assert_ptr_ne(NULL, style);
  ck_assert_ptr_eq(_Style(), classof(style));

  Object *object = $(alloc(Object), init);
  $(style, addAttribute, "object", object);
  ck_assert_ptr_eq(object, $(style, attributeValue, "object"));
  release(object);

  $(style, addBoolAttribute, "bool", true);
  const Boole *b = cast(Boole, $(style, attributeValue, "bool"));
  ck_assert_int_eq(true, b->value);

  $(style, addCharactersAttribute, "characters", "characters");
  const String *s = cast(String, $(style, attributeValue, "characters"));
  ck_assert_str_eq("characters", s->chars);

  $(style, addColorAttribute, "color", &MakeColor(0, 64, 128, 255));
  const Array *color = cast(Array, $(style, attributeValue, "color"));
  ck_assert_int_eq(4, color->count);
  ck_assert_int_eq(0, ((Number *) $(color, objectAtIndex, 0))->value);
  ck_assert_int_eq(64, ((Number *) $(color, objectAtIndex, 1))->value);
  ck_assert_int_eq(128, ((Number *) $(color, objectAtIndex, 2))->value);
  ck_assert_int_eq(255, ((Number *) $(color, objectAtIndex, 3))->value);

  $(style, addDoubleAttribute, "double", 2.0);
  const Number *d = cast(Number, $(style, attributeValue, "double"));
  ck_assert_int_eq(2, d->value);

  $(style, addEnumAttribute, "enum", ViewAutoresizingNames, ViewAutoresizingWidth | ViewAutoresizingContain);
  const String *e = cast(String, $(style, attributeValue, "enum"));
  ck_assert_str_eq("ViewAutoresizingWidth | ViewAutoresizingContain", e->chars);
  ck_assert_int_eq(ViewAutoresizingWidth | ViewAutoresizingContain, valueof(ViewAutoresizingNames, e->chars));

  $(style, addFloatAttribute, "float", 3.0);
  const Number *f = cast(Number, $(style, attributeValue, "float"));
  ck_assert_int_eq(3, f->value);

  $(style, addIntegerAttribute, "integer", 4.0);
  const Number *i = cast(Number, $(style, attributeValue, "integer"));
  ck_assert_int_eq(4, i->value);

  $(style, addPointAttribute, "point", &MakePoint(22, 44));
  const Array *point = cast(Array, $(style, attributeValue, "point"));
  ck_assert_int_eq(2, point->count);
  ck_assert_int_eq(22, ((Number *) $(point, objectAtIndex, 0))->value);
  ck_assert_int_eq(44, ((Number *) $(point, objectAtIndex, 1))->value);

  $(style, addRectangleAttribute, "rectangle", &MakeRect(50, 50, 320, 240));
  const Array *rectangle = cast(Array, $(style, attributeValue, "rectangle"));
  ck_assert_int_eq(4, rectangle->count);
  ck_assert_int_eq(50, ((Number *) $(rectangle, objectAtIndex, 0))->value);
  ck_assert_int_eq(50, ((Number *) $(rectangle, objectAtIndex, 1))->value);
  ck_assert_int_eq(320, ((Number *) $(rectangle, objectAtIndex, 2))->value);
  ck_assert_int_eq(240, ((Number *) $(rectangle, objectAtIndex, 3))->value);

  $(style, addSizeAttribute, "size", &MakeSize(640, 480));
  const Array *size = cast(Array, $(style, attributeValue, "size"));
  ck_assert_int_eq(2, size->count);
  ck_assert_int_eq(640, ((Number *) $(size, objectAtIndex, 0))->value);
  ck_assert_int_eq(480, ((Number *) $(size, objectAtIndex, 1))->value);

  $(style, removeAttribute, "size");
  ck_assert_ptr_eq(NULL, $(style, attributeValue, "size"));

  release(style);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Style");
  tcase_add_test(tcase, style);

  Suite *suite = suite_create("Style");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
