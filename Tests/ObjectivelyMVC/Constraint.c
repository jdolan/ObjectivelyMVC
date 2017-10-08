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

#include <ObjectivelyMVC.h>

START_TEST(constraint)
{
	Constraint *constraint;

	constraint = $(alloc(Constraint), initWithDescriptor, "w >= 500");
	ck_assert(constraint != NULL);
	ck_assert_ptr_eq(_Constraint(), classof(constraint));

	ck_assert_str_eq("w >= 500", constraint->descriptor);
	ck_assert(constraint->identifier == NULL);
	ck_assert_int_eq(ConstraintAttributeWidth, constraint->target);
	ck_assert_int_eq(ConstraintRelationGreaterThanOrEqual, constraint->relation);
	ck_assert_int_eq(ConstraintAttributeNone, constraint->source);
	ck_assert_int_eq(1, constraint->multiplier);
	ck_assert_int_eq(500, constraint->constant);
	ck_assert_int_eq(DEFAULT_CONSTRAINT_PRIORITY, constraint->priority);

	release(constraint);

	constraint = $(alloc(Constraint), initWithDescriptor, "h = w * 2 + 40");
	ck_assert(constraint != NULL);
	ck_assert_ptr_eq(_Constraint(), classof(constraint));

	ck_assert_str_eq("h = w * 2 + 40", constraint->descriptor);
	ck_assert(constraint->identifier == NULL);
	ck_assert_int_eq(ConstraintAttributeHeight, constraint->target);
	ck_assert_int_eq(ConstraintRelationEqual, constraint->relation);
	ck_assert_int_eq(ConstraintAttributeWidth, constraint->source);
	ck_assert_int_eq(2, constraint->multiplier);
	ck_assert_int_eq(40, constraint->constant);
	ck_assert_int_eq(DEFAULT_CONSTRAINT_PRIORITY, constraint->priority);

	release(constraint);

	constraint = $(alloc(Constraint), initWithDescriptor, "h = foo.h - 20");
	ck_assert(constraint != NULL);

	ck_assert_str_eq("h = foo.h - 20", constraint->descriptor);
	ck_assert_str_eq("foo", constraint->identifier);
	ck_assert_int_eq(ConstraintAttributeHeight, constraint->target);
	ck_assert_int_eq(ConstraintRelationEqual, constraint->relation);
	ck_assert_int_eq(ConstraintAttributeHeight, constraint->source);
	ck_assert_int_eq(1, constraint->multiplier);
	ck_assert_int_eq(-20, constraint->constant);
	ck_assert_int_eq(DEFAULT_CONSTRAINT_PRIORITY, constraint->priority);

	release(constraint);

	constraint = $(alloc(Constraint), initWithDescriptor, "w <= bar.w * 2.0 - 10");
	ck_assert(constraint != NULL);

	ck_assert_str_eq("w <= bar.w * 2.0 - 10", constraint->descriptor);
	ck_assert_str_eq("bar", constraint->identifier);
	ck_assert_int_eq(ConstraintAttributeWidth, constraint->target);
	ck_assert_int_eq(ConstraintRelationLessThanOrEqual, constraint->relation);
	ck_assert_int_eq(ConstraintAttributeWidth, constraint->source);
	ck_assert_int_eq(2, constraint->multiplier);
	ck_assert_int_eq(-10, constraint->constant);
	ck_assert_int_eq(DEFAULT_CONSTRAINT_PRIORITY, constraint->priority);

	constraint = $(alloc(Constraint), initWithDescriptor, "w > baz.h * 3 - 20 [90]");
	ck_assert(constraint != NULL);

	ck_assert_str_eq("w > baz.h * 3 - 20 [90]", constraint->descriptor);
	ck_assert_str_eq("baz", constraint->identifier);
	ck_assert_int_eq(ConstraintAttributeWidth, constraint->target);
	ck_assert_int_eq(ConstraintRelationGreaterThan, constraint->relation);
	ck_assert_int_eq(ConstraintAttributeHeight, constraint->source);
	ck_assert_int_eq(3, constraint->multiplier);
	ck_assert_int_eq(-20, constraint->constant);
	ck_assert_int_eq(90, constraint->priority);

	release(constraint);

}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("constraint");
	tcase_add_test(tcase, constraint);

	Suite *suite = suite_create("constraint");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
