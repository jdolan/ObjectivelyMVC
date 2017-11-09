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

#include <unistd.h>
#include <check.h>

#include <Objectively.h>

#include <ObjectivelyMVC.h>

START_TEST(stylesheet)
{
	Stylesheet *stylesheet = $$(Stylesheet, stylesheetWithCharacters, "\
		selector one, selector two .class { \
			boolAttribute: true; \
			charactersAttribute: hello-world; \
			colorAttribute: #aabbccdd; \
			doubleAttribute: 1.0; \
			enumAttribute: ViewAlignmentTopRight; \
			floatAttribute: 1.0; \
			integerAttribute: 1; \
			pointAttribute: 320 240; \
			rectangleAttribute: 50 50 400 300; \
			sizeAttribute: 640 480; \
		} \
	");

	ck_assert(stylesheet);
	ck_assert_int_eq(2, stylesheet->selectors->count);
	ck_assert_int_eq(2, stylesheet->styles->count);

	Selector *selectorOne = $(stylesheet->selectors, firstObject);
	ck_assert_str_eq("selector one", selectorOne->rule);

	Selector *selectorTwo = $(stylesheet->selectors, lastObject);
	ck_assert_str_eq("selector two .class", selectorTwo->rule);

	Style *style = $(stylesheet->styles, objectForKey, selectorOne);
	ck_assert(style);

	Boole *boole = cast(Boole, $(style, attributeValue, "boolAttribute"));
	ck_assert_ptr_eq($$(Boole, True), boole);

	String *characters = cast(String, $(style, attributeValue, "charactersAttribute"));
	ck_assert_str_eq("hello-world", characters->chars);

	String *color = cast(String, $(style, attributeValue, "colorAttribute"));
	ck_assert_str_eq("#aabbccdd", color->chars);

	Number *number = cast(Number, $(style, attributeValue, "doubleAttribute"));
	ck_assert(number->value == 1.0);

}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("stylesheet");
	tcase_add_test(tcase, stylesheet);

	Suite *suite = suite_create("stylesheet");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}

