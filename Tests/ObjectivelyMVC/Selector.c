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

#include <check.h>

#include <ObjectivelyMVC.h>

START_TEST(selector)
{
	Selector *selector = $(alloc(Selector), initWithRule, "Panel#main .foo > Control:highlighted");
	ck_assert_ptr_ne(NULL, selector);
	ck_assert_ptr_eq(_Selector(), classof(selector));
	ck_assert_int_eq(3, selector->sequences->count);
	ck_assert_int_eq(125, selector->specificity);

	SelectorSequence *selectorSequence;
	SimpleSelector *simpleSelector;

	// first sequence

	selectorSequence = $(selector->sequences, objectAtIndex, 0);
	ck_assert_int_eq(2, selectorSequence->simpleSelectors->count);
	ck_assert_int_eq(SequenceCombinatorNone, selectorSequence->left);
	ck_assert_int_eq(SequenceCombinatorDescendent, selectorSequence->right);

	simpleSelector = $(selectorSequence->simpleSelectors, objectAtIndex, 0);
	ck_assert_int_eq(SimpleSelectorTypeType, simpleSelector->type);
	ck_assert_str_eq("Panel", simpleSelector->pattern);

	simpleSelector = $(selectorSequence->simpleSelectors, objectAtIndex, 1);
	ck_assert_int_eq(SimpleSelectorTypeId, simpleSelector->type);
	ck_assert_str_eq("main", simpleSelector->pattern);

	// second sequence

	selectorSequence = $(selector->sequences, objectAtIndex, 1);
	ck_assert_int_eq(1, selectorSequence->simpleSelectors->count);
	ck_assert_int_eq(SequenceCombinatorDescendent, selectorSequence->left);
	ck_assert_int_eq(SequenceCombinatorChild, selectorSequence->right);

	simpleSelector = $(selectorSequence->simpleSelectors, objectAtIndex, 0);
	ck_assert_int_eq(SimpleSelectorTypeClass, simpleSelector->type);
	ck_assert_str_eq("foo", simpleSelector->pattern);

	// third sequence

	selectorSequence = $(selector->sequences, objectAtIndex, 2);
	ck_assert_int_eq(2, selectorSequence->simpleSelectors->count);
	ck_assert_int_eq(SequenceCombinatorChild, selectorSequence->left);
	ck_assert_int_eq(SequenceCombinatorTerminal, selectorSequence->right);

	simpleSelector = $(selectorSequence->simpleSelectors, objectAtIndex, 0);
	ck_assert_int_eq(SimpleSelectorTypeType, simpleSelector->type);
	ck_assert_str_eq("Control", simpleSelector->pattern);

	simpleSelector = $(selectorSequence->simpleSelectors, objectAtIndex, 1);
	ck_assert_int_eq(SimpleSelectorTypePseudo, simpleSelector->type);
	ck_assert_str_eq("highlighted", simpleSelector->pattern);

	release(selector);

} END_TEST

START_TEST(compareTo)
{
	Selector *a = $(alloc(Selector), initWithRule, "#id Type .class");
	ck_assert_ptr_ne(NULL, a);
	ck_assert_int_eq(111, a->specificity);

	Selector *b = $(alloc(Selector), initWithRule, "Type .class:pseudo");
	ck_assert_ptr_ne(NULL, b);
	ck_assert_int_eq(21, b->specificity);

	ck_assert_int_eq(OrderDescending, $(a, compareTo, b));

} END_TEST

START_TEST(matchesView)
{
	View *root = $(alloc(View), initWithFrame, NULL);
	root->identifier = strdup("root");

	View *container = $(alloc(View), initWithFrame, NULL);
	$(container, addClassName, "container");

	$(root, addSubview, container);

	Panel *panel = $(alloc(Panel), initWithFrame, NULL);
	$(container, addSubview, (View *) panel);

	View *view = $(alloc(View), initWithFrame, NULL);
	$((View *) panel->contentView, addSubview, view);

	View *subview = $(alloc(View), initWithFrame, NULL);
	$(view, addSubview, subview);

	{
		Selector *selector = $(alloc(Selector), initWithRule, "*");
		ck_assert_ptr_ne(NULL, selector);

		ck_assert_int_eq(1, $(selector, matchesView, (View *) panel));
		ck_assert_int_eq(1, $(selector, matchesView, container));
		ck_assert_int_eq(1, $(selector, matchesView, root));

		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, "Panel");
		ck_assert_ptr_ne(NULL, selector);

		ck_assert_int_eq(1, $(selector, matchesView, (View *) panel));
		ck_assert_int_eq(0, $(selector, matchesView, container));
		ck_assert_int_eq(0, $(selector, matchesView, root));

		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".container Panel");
		ck_assert_ptr_ne(NULL, selector);

		ck_assert_int_eq(1, $(selector, matchesView, (View *) panel));
		ck_assert_int_eq(0, $(selector, matchesView, container));
		ck_assert_int_eq(0, $(selector, matchesView, root));

		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, "#root .container Panel");
		ck_assert_ptr_ne(NULL, selector);

		ck_assert_int_eq(1, $(selector, matchesView, (View *) panel));
		ck_assert_int_eq(0, $(selector, matchesView, container));
		ck_assert_int_eq(0, $(selector, matchesView, root));

		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".contentView > View");
		ck_assert_ptr_ne(NULL, selector);

		ck_assert_int_eq(1, $(selector, matchesView, view));
		ck_assert_int_eq(0, $(selector, matchesView, subview));

		release(selector);
	}

	release(subview);
	release(view);
	release(panel);
	release(container);
	release(root);

} END_TEST

START_TEST(_select)
{
	View *root = $(alloc(View), initWithFrame, NULL);
	root->identifier = strdup("root");

	View *container = $(alloc(View), initWithFrame, NULL);
	$(container, addClassName, "container");

	$(root, addSubview, container);

	Panel *panel = $(alloc(Panel), initWithFrame, NULL);
	$(container, addSubview, (View *) panel);

	{
		Selector *selector = $(alloc(Selector), initWithRule, "#root .container Panel");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_eq(1, selection->count);
		ck_assert($(selection, containsObject, panel));

		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".container Panel");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_eq(1, selection->count);
		ck_assert($(selection, containsObject, panel));
		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, "Panel");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_eq(1, selection->count);
		ck_assert($(selection, containsObject, panel));

		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".container");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_le(1, selection->count);
		ck_assert($(selection, containsObject, container));

		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".contentView");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_eq(1, selection->count);
		ck_assert($(selection, containsObject, panel->contentView));

		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, ".accessoryView");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert_int_eq(1, selection->count);
		ck_assert($(selection, containsObject, panel->accessoryView));

		release(selection);
		release(selector);
	}

	{
		Selector *selector = $(alloc(Selector), initWithRule, "*");
		ck_assert_ptr_ne(NULL, selector);

		Set *selection = $(selector, select, root);
		ck_assert_ptr_ne(NULL, selection);

		ck_assert($(selection, containsObject, root));
		ck_assert($(selection, containsObject, container));
		ck_assert($(selection, containsObject, panel));

		release(selection);
		release(selector);
	}

	release(panel);
	release(container);
	release(root);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("selector");
	tcase_add_test(tcase, selector);
	tcase_add_test(tcase, compareTo);
	tcase_add_test(tcase, matchesView);
	tcase_add_test(tcase, _select);

	Suite *suite = suite_create("selector");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}

