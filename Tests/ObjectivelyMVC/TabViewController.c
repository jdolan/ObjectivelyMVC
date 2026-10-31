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
 * @brief A ViewController that counts the appearance callbacks and the events it receives.
 */
typedef struct {
  ViewController viewController;
  int willAppear, didAppear, willDisappear, didDisappear, events;
  ViewController *removeOnAppear;
} Probe;

static int didDisappearTotal;

static void Probe_respondToEvent(ViewController *self, const SDL_Event *event) {
  ((Probe *) self)->events++;
}

static void Probe_viewWillAppear(ViewController *self) {

  Probe *this = (Probe *) self;

  this->willAppear++;

  if (this->removeOnAppear) {
    $(this->removeOnAppear, removeFromParentViewController);
    this->removeOnAppear = NULL;
  }
}

static void Probe_viewDidAppear(ViewController *self) {
  ((Probe *) self)->didAppear++;
}

static void Probe_viewWillDisappear(ViewController *self) {
  ((Probe *) self)->willDisappear++;
}

static void Probe_viewDidDisappear(ViewController *self) {
  ((Probe *) self)->didDisappear++;
  didDisappearTotal++;
}

static void Probe_initialize(Class *clazz) {
  ((ViewControllerInterface *) clazz->interface)->respondToEvent = Probe_respondToEvent;
  ((ViewControllerInterface *) clazz->interface)->viewWillAppear = Probe_viewWillAppear;
  ((ViewControllerInterface *) clazz->interface)->viewDidAppear = Probe_viewDidAppear;
  ((ViewControllerInterface *) clazz->interface)->viewWillDisappear = Probe_viewWillDisappear;
  ((ViewControllerInterface *) clazz->interface)->viewDidDisappear = Probe_viewDidDisappear;
}

static Class *_Probe(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Probe",
      .superclass = _ViewController(),
      .instanceSize = sizeof(Probe),
      .interfaceSize = sizeof(ViewControllerInterface),
      .initialize = Probe_initialize,
    });
  });

  return clazz;
}

static Probe *probe(TabViewController *tabViewController) {

  Probe *probe = (Probe *) $((ViewController *) alloc(Probe), init);

  $((ViewController *) tabViewController, addChildViewController, (ViewController *) probe);

  return probe;
}

static void appear(TabViewController *tabViewController) {
  $((ViewController *) tabViewController, viewWillAppear);
  $((ViewController *) tabViewController, viewDidAppear);
}

static void disappear(TabViewController *tabViewController) {
  $((ViewController *) tabViewController, viewWillDisappear);
  $((ViewController *) tabViewController, viewDidDisappear);
}

static void select(TabViewController *tabViewController, Probe *probe) {
  TabViewItem *tab = $(tabViewController, tabForViewController, (ViewController *) probe);
  $(tabViewController->tabView, selectTab, tab);
}

static void sendEvent(TabViewController *tabViewController) {
  const SDL_Event event = { .type = SDL_EVENT_USER };
  $((ViewController *) tabViewController, respondToEvent, &event);
}

static int selections;

static void didSelectTab(TabViewController *tabViewController, TabViewItem *tab) {
  selections++;
}

START_TEST(onlySelectedTabAppearsAndReceivesEvents) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  ck_assert_ptr_eq(a, tabViewController->selectedViewController);

  appear(tabViewController);
  sendEvent(tabViewController);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->didAppear);
  ck_assert_int_eq(1, a->events);

  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->didAppear);
  ck_assert_int_eq(0, b->events);

  disappear(tabViewController);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);
  ck_assert_int_eq(0, b->willDisappear);
  ck_assert_int_eq(0, b->didDisappear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(selectingTabWhileVisibleSwapsAppearance) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  appear(tabViewController);

  select(tabViewController, b);

  ck_assert_ptr_eq(b, tabViewController->selectedViewController);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);
  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  sendEvent(tabViewController);

  ck_assert_int_eq(0, a->events);
  ck_assert_int_eq(1, b->events);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(selectingTabWhileHiddenSendsNoAppearance) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  select(tabViewController, b);

  ck_assert_ptr_eq(b, tabViewController->selectedViewController);

  ck_assert_int_eq(0, a->willDisappear);
  ck_assert_int_eq(0, b->willAppear);

  appear(tabViewController);

  ck_assert_int_eq(0, a->willAppear);
  ck_assert_int_eq(1, b->willAppear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(removingSelectedTabWhileVisibleSelectsNext) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  appear(tabViewController);

  $((ViewController *) tabViewController, removeChildViewController, (ViewController *) a);

  ck_assert_ptr_eq(b, tabViewController->selectedViewController);
  ck_assert_ptr_null(a->viewController.parentViewController);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);
  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(removingUnselectedTabSendsNoAppearance) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  appear(tabViewController);

  $((ViewController *) tabViewController, removeChildViewController, (ViewController *) b);

  ck_assert_ptr_eq(a, tabViewController->selectedViewController);
  ck_assert_int_eq(0, b->willDisappear);
  ck_assert_int_eq(0, b->didDisappear);
  ck_assert_int_eq(0, a->willDisappear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(addingTabWhileVisibleAppearsOnlyTheFirst) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  appear(tabViewController);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  ck_assert_ptr_eq(a, tabViewController->selectedViewController);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->didAppear);
  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->didAppear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(removingSelectedTabWhileHiddenSendsNoAppearance) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  $((ViewController *) tabViewController, removeChildViewController, (ViewController *) a);

  ck_assert_ptr_eq(b, tabViewController->selectedViewController);
  ck_assert_int_eq(0, a->willDisappear);
  ck_assert_int_eq(0, b->willAppear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(removingLastTabClearsSelection) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);

  appear(tabViewController);

  $((ViewController *) tabViewController, removeChildViewController, (ViewController *) a);

  ck_assert_ptr_null(tabViewController->selectedViewController);
  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  sendEvent(tabViewController);

  ck_assert_int_eq(0, a->events);

  release(a);
  release(tabViewController);

} END_TEST

START_TEST(selectingTabWhileAppearingAppearsItOnce) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  $((ViewController *) tabViewController, viewWillAppear);

  select(tabViewController, b);

  $((ViewController *) tabViewController, viewDidAppear);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(0, a->didAppear);
  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(selectingTabWhileDisappearingCompletesThePrevious) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  appear(tabViewController);

  $((ViewController *) tabViewController, viewWillDisappear);

  select(tabViewController, b);

  $((ViewController *) tabViewController, viewDidDisappear);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->didAppear);
  ck_assert_int_eq(0, b->willDisappear);
  ck_assert_int_eq(0, b->didDisappear);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

START_TEST(appearanceCallbackMayRemoveThePreviousTab) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  Probe *a = probe(tabViewController);
  Probe *b = probe(tabViewController);

  appear(tabViewController);

  b->removeOnAppear = (ViewController *) a;
  release(a);

  didDisappearTotal = 0;

  select(tabViewController, b);

  ck_assert_ptr_eq(b, tabViewController->selectedViewController);
  ck_assert_int_eq(1, didDisappearTotal);
  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(b);
  release(tabViewController);

} END_TEST

START_TEST(delegateIsCalledOnSelection) {

  TabViewController *tabViewController = $(alloc(TabViewController), init);

  selections = 0;

  Probe *a = probe(tabViewController);

  tabViewController->delegate.didSelectTab = didSelectTab;

  Probe *b = probe(tabViewController);

  select(tabViewController, b);
  select(tabViewController, a);

  ck_assert_int_eq(2, selections);

  release(a);
  release(b);
  release(tabViewController);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("TabViewController");
  tcase_add_test(tcase, onlySelectedTabAppearsAndReceivesEvents);
  tcase_add_test(tcase, selectingTabWhileVisibleSwapsAppearance);
  tcase_add_test(tcase, selectingTabWhileHiddenSendsNoAppearance);
  tcase_add_test(tcase, removingSelectedTabWhileVisibleSelectsNext);
  tcase_add_test(tcase, removingUnselectedTabSendsNoAppearance);
  tcase_add_test(tcase, addingTabWhileVisibleAppearsOnlyTheFirst);
  tcase_add_test(tcase, removingSelectedTabWhileHiddenSendsNoAppearance);
  tcase_add_test(tcase, removingLastTabClearsSelection);
  tcase_add_test(tcase, selectingTabWhileAppearingAppearsItOnce);
  tcase_add_test(tcase, selectingTabWhileDisappearingCompletesThePrevious);
  tcase_add_test(tcase, appearanceCallbackMayRemoveThePreviousTab);
  tcase_add_test(tcase, delegateIsCalledOnSelection);

  Suite *suite = suite_create("TabViewController");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
