/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include <ObjectivelyMVC.h>

START_TEST(stylesheet) {

	Stylesheet *stylesheet = $$(Stylesheet, stylesheetWithCharacters, "\
		selector one, selector two .class { \
			bool-attribute: true; \
			characters-attribute: hello-world; \
			color-attribute: #aabbccdd; \
			double-attribute: 1.0; \
			enum-attribute: Foo | Bar; \
			float-attribute: 1.0; \
			integer-attribute: 1; \
			point-attribute: 320 240; \
			rectangle-attribute: 50 50 400 300; \
			size-attribute: 640 480; \
		} \
	");

	ck_assert_ptr_ne(NULL, stylesheet);

	const Array *selectors = stylesheet->selectors;
	const Array *styles = stylesheet->styles;

	ck_assert_int_eq(2, selectors->count);
	ck_assert_int_eq(1, styles->count);

	Selector *selectorOne = $(selectors, firstObject);
	ck_assert_str_eq("selector one", selectorOne->rule);

	Selector *selectorTwo = $(selectors, lastObject);
	ck_assert_str_eq("selector two .class", selectorTwo->rule);

	Style *style = selectorOne->style;
	ck_assert_ptr_ne(NULL, style);

	Boole *booleAttribute = cast(Boole, $(style, attributeValue, "bool-attribute"));
	ck_assert_ptr_eq($$(Boole, True), booleAttribute);

	String *charactersAttribute = cast(String, $(style, attributeValue, "characters-attribute"));
	ck_assert_str_eq("hello-world", charactersAttribute->chars);

	String *colorAttribute = cast(String, $(style, attributeValue, "color-attribute"));
	ck_assert_str_eq("#aabbccdd", colorAttribute->chars);

	Number *doubleNumber = cast(Number, $(style, attributeValue, "double-attribute"));
	ck_assert_int_eq(1, doubleNumber->value);

	String *enumAttribute = cast(String, $(style, attributeValue, "enum-attribute"));
	ck_assert_str_eq("Foo | Bar", enumAttribute->chars);

	Number *floatAttribute = cast(Number, $(style, attributeValue, "float-attribute"));
	ck_assert_int_eq(1, floatAttribute->value);

	Number *integerAttribute = cast(Number, $(style, attributeValue, "integer-attribute"));
	ck_assert_int_eq(1, integerAttribute->value);

	Array *pointAttribute = cast(Array, $(style, attributeValue, "point-attribute"));
	ck_assert_int_eq(2, pointAttribute->count);

	Number *pointX = $(pointAttribute, objectAtIndex, 0);
	Number *pointY = $(pointAttribute, objectAtIndex, 1);
	ck_assert_int_eq(320, pointX->value);
	ck_assert_int_eq(240, pointY->value);

	Array *rectangleAttribute = cast(Array, $(style, attributeValue, "rectangle-attribute"));
	ck_assert_int_eq(4, rectangleAttribute->count);

	Number *rectX = $(rectangleAttribute, objectAtIndex, 0);
	Number *rectY = $(rectangleAttribute, objectAtIndex, 1);
	Number *rectW = $(rectangleAttribute, objectAtIndex, 2);
	Number *rectH = $(rectangleAttribute, objectAtIndex, 3);
	ck_assert_int_eq(50, rectX->value);
	ck_assert_int_eq(50, rectY->value);
	ck_assert_int_eq(400, rectW->value);
	ck_assert_int_eq(300, rectH->value);

	Array *sizeAttribute = cast(Array, $(style, attributeValue, "size-attribute"));
	ck_assert_int_eq(2, sizeAttribute->count);

	Number *sizeW = $(sizeAttribute, objectAtIndex, 0);
	Number *sizeH = $(sizeAttribute, objectAtIndex, 1);
	ck_assert_int_eq(640, sizeW->value);
	ck_assert_int_eq(480, sizeH->value);

	release(stylesheet);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Stylesheet");
	tcase_add_test(tcase, stylesheet);

	Suite *suite = suite_create("Stylesheet");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}

