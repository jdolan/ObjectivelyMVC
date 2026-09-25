/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

#include "ObjectivelyMVC.h"

/**
 * @brief Allocates a leaf View with a fixed, locked size (minSize == maxSize == size).
 */
static View *fixedView(int w, int h) {

  View *view = $(alloc(View), initWithFrame, NULL);

  const SDL_Size size = MakeSize(w, h);

  view->minSize = size;
  view->maxSize = size;

  view->frame.w = w;
  view->frame.h = h;

  return view;
}

START_TEST(fixedSizeLeavesInStackView) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *a = fixedView(40, 20);
  View *b = fixedView(60, 30);

  $((View *) stackView, addSubview, a);
  $((View *) stackView, addSubview, b);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(60, stackView->view.frame.w);
  ck_assert_int_eq(50, stackView->view.frame.h);

  ck_assert_int_eq(0, a->frame.y);
  ck_assert_int_eq(20, b->frame.y);

  release(stackView);

} END_TEST

START_TEST(nestedContainStackViews) {

  StackView *outer = $(alloc(StackView), initWithFrame, NULL);
  outer->axis = StackViewAxisVertical;

  StackView *inner = $(alloc(StackView), initWithFrame, NULL);
  inner->axis = StackViewAxisHorizontal;

  View *innerA = fixedView(10, 15);
  View *innerB = fixedView(20, 25);

  $((View *) inner, addSubview, innerA);
  $((View *) inner, addSubview, innerB);

  View *sibling = fixedView(50, 5);

  $((View *) outer, addSubview, (View *) inner);
  $((View *) outer, addSubview, sibling);

  $((View *) outer, layoutIfNeeded);

  ck_assert_int_eq(30, inner->view.frame.w);
  ck_assert_int_eq(25, inner->view.frame.h);

  ck_assert_int_eq(50, outer->view.frame.w);
  ck_assert_int_eq(30, outer->view.frame.h);

  ck_assert_int_eq(0, inner->view.frame.y);
  ck_assert_int_eq(25, sibling->frame.y);

  release(outer);

} END_TEST

START_TEST(containStackViewWithFillChild) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  StackView *fillChild = $(alloc(StackView), initWithFrame, NULL);
  fillChild->view.autoresizingMask |= ViewAutoresizingWidth;

  View *grandchild = fixedView(35, 10);
  $((View *) fillChild, addSubview, grandchild);

  $((View *) stackView, addSubview, (View *) fillChild);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(35, stackView->view.frame.w);
  ck_assert_int_eq(35, fillChild->view.frame.w);

  const SDL_Rect frame = fillChild->view.frame;

  $((View *) stackView, setNeedsLayout);
  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(frame.w, fillChild->view.frame.w);
  ck_assert_int_eq(frame.h, fillChild->view.frame.h);
  ck_assert_int_eq(35, stackView->view.frame.w);

  release(stackView);

} END_TEST

START_TEST(containStackViewWithFillAndFixedSiblings) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *fixedSibling = fixedView(80, 20);

  View *fillSibling = fixedView(30, 10);
  fillSibling->autoresizingMask |= ViewAutoresizingWidth;
  fillSibling->maxSize.w = INT32_MAX;

  $((View *) stackView, addSubview, fixedSibling);
  $((View *) stackView, addSubview, fillSibling);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(80, stackView->view.frame.w);
  ck_assert_int_eq(80, fillSibling->frame.w);

  release(stackView);

} END_TEST

START_TEST(stackViewMeasuresFreshNotStale) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *child = fixedView(10, 10);
  $((View *) stackView, addSubview, child);

  const SDL_Size grown = MakeSize(90, 40);
  child->minSize = grown;
  child->maxSize = grown;
  child->frame.w = grown.w;
  child->frame.h = grown.h;

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(90, stackView->view.frame.w);
  ck_assert_int_eq(40, stackView->view.frame.h);

  release(stackView);

} END_TEST

START_TEST(standaloneRelayoutDoesNotShrinkFillChild) {

  // Mirrors a selected TableRowView: a Contain|Width StackView (row) whose own intrinsic
  // content is much narrower than the width its Contain parent (table) already forced it to.

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *wideSibling = fixedView(200, 10);
  $((View *) stackView, addSubview, wideSibling);

  StackView *row = $(alloc(StackView), initWithFrame, NULL);
  row->view.autoresizingMask |= ViewAutoresizingWidth;

  View *cell = fixedView(50, 10);
  $((View *) row, addSubview, cell);

  $((View *) stackView, addSubview, (View *) row);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(200, stackView->view.frame.w);
  ck_assert_int_eq(200, row->view.frame.w);

  // Simulate a style rebind on the row alone (e.g. a `:selected` pseudo-class match), which
  // marks only the row -- not its superview -- needsLayout, per View::_bind's contract.
  $((View *) row, setNeedsLayout);
  $((View *) row, layoutIfNeeded);

  ck_assert_int_eq(200, row->view.frame.w);

  release(stackView);

} END_TEST

START_TEST(fillOnlyChildFillsContainBounds) {

  // Mirrors Slider's `bar`: a plain `fill` (Width|Height) leaf, not itself Contain/Fit, whose
  // parent is a Contain container with no other content to size from (a bare `fill` leaf has no
  // independent intrinsic size of its own, so the parent's floor comes entirely from minSize --
  // exactly how Slider's own width is floored via CSS `min-width`).

  View *outer = $(alloc(View), initWithFrame, NULL);
  outer->autoresizingMask = ViewAutoresizingContain;
  outer->minSize = MakeSize(100, 50);

  View *child = $(alloc(View), initWithFrame, NULL);
  child->autoresizingMask = ViewAutoresizingFill;

  $(outer, addSubview, child);

  $(outer, layoutIfNeeded);

  ck_assert_int_eq(100, outer->frame.w);
  ck_assert_int_eq(50, outer->frame.h);
  ck_assert_int_eq(100, child->frame.w);
  ck_assert_int_eq(50, child->frame.h);

  release(outer);

} END_TEST

START_TEST(minSizeFloorsContainBounds) {

  // Mirrors Slider itself: a styled `min-width` is the only thing giving a Contain view with no
  // sized content of its own (a bare `fill` leaf, contributing nothing) a real width.

  View *outer = $(alloc(View), initWithFrame, NULL);
  outer->autoresizingMask = ViewAutoresizingContain;

  Style *style = $(alloc(Style), initWithAttributes, NULL);
  $(style, addIntegerAttribute, "min-width", 140);
  $(style, addIntegerAttribute, "min-height", 24);

  $(outer, applyStyle, style);
  release(style);

  View *child = $(alloc(View), initWithFrame, NULL);
  child->autoresizingMask = ViewAutoresizingFill;

  $(outer, addSubview, child);

  $(outer, layoutIfNeeded);

  ck_assert_int_eq(140, outer->frame.w);
  ck_assert_int_eq(24, outer->frame.h);
  ck_assert_int_eq(140, child->frame.w);
  ck_assert_int_eq(24, child->frame.h);

  release(outer);

} END_TEST

START_TEST(setVisibilityMarksSuperviewNeedsLayout) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
  stackView->view.autoresizingMask = ViewAutoresizingContain;

  View *a = fixedView(40, 20);
  View *b = fixedView(60, 30);

  $((View *) stackView, addSubview, a);
  $((View *) stackView, addSubview, b);

  $((View *) stackView, layoutIfNeeded);

  $(a, setVisibility, ViewVisibilityHidden);
  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(60, stackView->view.frame.w);
  ck_assert_int_eq(30, stackView->view.frame.h);
  ck_assert_int_eq(0, b->frame.y);

  release(stackView);

} END_TEST

START_TEST(alignedContainChildRecentersOnGrowth) {

  View *root = fixedView(400, 200);
  root->autoresizingMask = ViewAutoresizingFill;

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
  stackView->axis = StackViewAxisHorizontal;
  stackView->view.alignment = ViewAlignmentBottomCenter;
  stackView->view.autoresizingMask = ViewAutoresizingContain;

  $(root, addSubview, (View *) stackView);
  $((View *) stackView, addSubview, fixedView(40, 20));
  $(root, layoutIfNeeded);

  ck_assert_int_eq(180, stackView->view.frame.x);

  // Growing the aligned child after the parent has laid out must re-center it
  $((View *) stackView, addSubview, fixedView(40, 20));
  $(root, layoutIfNeeded);

  ck_assert_int_eq(80, stackView->view.frame.w);
  ck_assert_int_eq(160, stackView->view.frame.x);
  ck_assert_int_eq(180, stackView->view.frame.y);

  release(stackView);
  release(root);

} END_TEST

START_TEST(pointerEventsPassThroughToSubviewsAndSiblings) {

  View *root = fixedView(400, 200);
  root->autoresizingMask = ViewAutoresizingFill;

  View *beneath = fixedView(400, 200);
  $(root, addSubview, beneath);

  View *overlay = fixedView(400, 200);
  View *button = fixedView(40, 20);
  $(overlay, addSubview, button);
  $(root, addSubview, overlay);

  $(root, layoutIfNeeded);

  ck_assert_ptr_eq(button, $(root, hitTest, &MakePoint(10, 10)));
  ck_assert_ptr_eq(overlay, $(root, hitTest, &MakePoint(300, 100)));

  overlay->pointerEvents = ViewPointerEventsNone;

  ck_assert_ptr_eq(button, $(root, hitTest, &MakePoint(10, 10)));
  ck_assert_ptr_eq(beneath, $(root, hitTest, &MakePoint(300, 100)));

  Style *style = $(alloc(Style), initWithRules, "View");
  $(style, addEnumAttribute, "pointer-events", ViewPointerEventsNames, ViewPointerEventsAuto);
  $(overlay, applyStyle, style);
  release(style);

  ck_assert_ptr_eq(overlay, $(root, hitTest, &MakePoint(300, 100)));

  release(button);
  release(overlay);
  release(beneath);
  release(root);

} END_TEST

/**
 * @brief Creates a View of the given size that a StackView may resize.
 */
static View *flexibleView(int w, int h) {

  View *view = $(alloc(View), initWithFrame, &MakeRect(0, 0, w, h));

  view->autoresizingMask = ViewAutoresizingNone;

  return view;
}

START_TEST(stretchChildTakesRemainingHeight) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
  stackView->view.minSize = stackView->view.maxSize = MakeSize(100, 200);

  View *tabs = fixedView(100, 20);
  View *page = flexibleView(100, 30);
  page->stretch = true;

  $((View *) stackView, addSubview, tabs);
  $((View *) stackView, addSubview, page);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(200, stackView->view.frame.h);
  ck_assert_int_eq(20, tabs->frame.h);
  ck_assert_int_eq(20, page->frame.y);
  ck_assert_int_eq(180, page->frame.h);

  release(stackView);

} END_TEST

START_TEST(stretchChildrenShareRemainingWidth) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
  stackView->view.minSize = stackView->view.maxSize = MakeSize(111, 10);
  stackView->axis = StackViewAxisHorizontal;

  View *a = fixedView(10, 10);
  View *b = flexibleView(0, 10);
  View *c = flexibleView(0, 10);
  b->stretch = c->stretch = true;

  $((View *) stackView, addSubview, a);
  $((View *) stackView, addSubview, b);
  $((View *) stackView, addSubview, c);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(10, a->frame.w);
  ck_assert_int_eq(50, b->frame.w);
  ck_assert_int_eq(10, b->frame.x);
  ck_assert_int_eq(51, c->frame.w);
  ck_assert_int_eq(60, c->frame.x);

  release(stackView);

} END_TEST

START_TEST(stretchIsIgnoredWithoutRemainingSpace) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *a = fixedView(40, 20);
  View *b = flexibleView(40, 30);
  b->stretch = true;

  $((View *) stackView, addSubview, a);
  $((View *) stackView, addSubview, b);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(50, stackView->view.frame.h);
  ck_assert_int_eq(30, b->frame.h);

  release(stackView);

} END_TEST

START_TEST(stretchContainChildKeepsStretchedSize) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
  stackView->view.minSize = stackView->view.maxSize = MakeSize(100, 200);

  View *tabs = fixedView(100, 20);

  StackView *page = $(alloc(StackView), initWithFrame, NULL);
  page->view.stretch = true;
  $((View *) page, addSubview, fixedView(80, 40));

  $((View *) stackView, addSubview, tabs);
  $((View *) stackView, addSubview, (View *) page);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(180, page->view.frame.h);

  $((View *) page, setNeedsLayout);
  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(180, page->view.frame.h);

  release(stackView);

} END_TEST

START_TEST(styledVisibilityRelaysOutSuperview) {

  StackView *stackView = $(alloc(StackView), initWithFrame, NULL);

  View *a = fixedView(40, 20);
  View *b = fixedView(40, 30);
  b->visibility = ViewVisibilityHidden;

  $((View *) stackView, addSubview, a);
  $((View *) stackView, addSubview, b);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(20, stackView->view.frame.h);

  Style *style = $(alloc(Style), initWithAttributes, NULL);
  $(style, addEnumAttribute, "visibility", ViewVisibilityNames, ViewVisibilityVisible);
  $(b, applyStyle, style);
  release(style);

  $((View *) stackView, layoutIfNeeded);

  ck_assert_int_eq(50, stackView->view.frame.h);
  ck_assert_int_eq(20, b->frame.y);

  release(stackView);

} END_TEST

START_TEST(scrollViewContentViewFromJSON) {

  View *view = $$(View, viewWithCharacters,
                  "{ \"class\": \"ScrollView\", \"contentView\": { \"identifier\": \"content\" } }", NULL);
  ck_assert_ptr_nonnull(view);

  ScrollView *scrollView = (ScrollView *) view;
  ck_assert_ptr_nonnull(scrollView->contentView);
  ck_assert_str_eq("content", scrollView->contentView->identifier);
  ck_assert_ptr_eq(view, scrollView->contentView->superview);
  ck_assert($(scrollView->contentView, hasClassName, "contentView"));

  release(view);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("View");
  tcase_add_test(tcase, fixedSizeLeavesInStackView);
  tcase_add_test(tcase, nestedContainStackViews);
  tcase_add_test(tcase, containStackViewWithFillChild);
  tcase_add_test(tcase, containStackViewWithFillAndFixedSiblings);
  tcase_add_test(tcase, stackViewMeasuresFreshNotStale);
  tcase_add_test(tcase, standaloneRelayoutDoesNotShrinkFillChild);
  tcase_add_test(tcase, fillOnlyChildFillsContainBounds);
  tcase_add_test(tcase, minSizeFloorsContainBounds);
  tcase_add_test(tcase, setVisibilityMarksSuperviewNeedsLayout);
  tcase_add_test(tcase, alignedContainChildRecentersOnGrowth);
  tcase_add_test(tcase, pointerEventsPassThroughToSubviewsAndSiblings);
  tcase_add_test(tcase, stretchChildTakesRemainingHeight);
  tcase_add_test(tcase, stretchChildrenShareRemainingWidth);
  tcase_add_test(tcase, stretchIsIgnoredWithoutRemainingSpace);
  tcase_add_test(tcase, stretchContainChildKeepsStretchedSize);
  tcase_add_test(tcase, styledVisibilityRelaysOutSuperview);
  tcase_add_test(tcase, scrollViewContentViewFromJSON);

  Suite *suite = suite_create("View");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
