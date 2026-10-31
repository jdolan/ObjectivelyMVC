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
} Probe;

static void Probe_respondToEvent(ViewController *self, const SDL_Event *event) {
  ((Probe *) self)->events++;
}

static void Probe_viewWillAppear(ViewController *self) {
  ((Probe *) self)->willAppear++;
}

static void Probe_viewDidAppear(ViewController *self) {
  ((Probe *) self)->didAppear++;
}

static void Probe_viewWillDisappear(ViewController *self) {
  ((Probe *) self)->willDisappear++;
}

static void Probe_viewDidDisappear(ViewController *self) {
  ((Probe *) self)->didDisappear++;
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

static Probe *push(NavigationViewController *navigationViewController) {

  Probe *probe = (Probe *) $((ViewController *) alloc(Probe), init);

  $(navigationViewController, pushViewController, (ViewController *) probe);

  return probe;
}

static void appear(NavigationViewController *navigationViewController) {
  $((ViewController *) navigationViewController, viewWillAppear);
  $((ViewController *) navigationViewController, viewDidAppear);
}

static void disappear(NavigationViewController *navigationViewController) {
  $((ViewController *) navigationViewController, viewWillDisappear);
  $((ViewController *) navigationViewController, viewDidDisappear);
}

static void sendEvent(NavigationViewController *navigationViewController) {
  const SDL_Event event = { .type = SDL_EVENT_USER };
  $((ViewController *) navigationViewController, respondToEvent, &event);
}

START_TEST(onlyTopAppearsAndReceivesEvents) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);

  appear(navigationViewController);
  sendEvent(navigationViewController);

  ck_assert_int_eq(0, a->willAppear);
  ck_assert_int_eq(0, a->events);

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);
  ck_assert_int_eq(1, b->events);

  disappear(navigationViewController);

  ck_assert_int_eq(0, a->willDisappear);
  ck_assert_int_eq(1, b->willDisappear);
  ck_assert_int_eq(1, b->didDisappear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(pushWhileVisibleCoversTheTop) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  appear(navigationViewController);

  Probe *b = push(navigationViewController);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);
  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(pushAndPopWhileHiddenSendNoAppearance) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  appear(navigationViewController);
  disappear(navigationViewController);

  Probe *b = push(navigationViewController);

  $(navigationViewController, popViewController);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->willDisappear);

  appear(navigationViewController);

  ck_assert_int_eq(2, a->willAppear);
  ck_assert_int_eq(2, a->didAppear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(popToSkipsCoveredViewControllers) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);
  Probe *c = push(navigationViewController);

  appear(navigationViewController);

  $(navigationViewController, popToViewController, (ViewController *) a);

  ck_assert_ptr_eq(a, $(navigationViewController, topViewController));

  ck_assert_int_eq(1, c->willDisappear);
  ck_assert_int_eq(1, c->didDisappear);

  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->willDisappear);
  ck_assert_ptr_null(b->viewController.parentViewController);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->didAppear);

  release(a);
  release(b);
  release(c);
  release(navigationViewController);

} END_TEST

START_TEST(pushWhileAppearingAppearsItOnce) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  $((ViewController *) navigationViewController, viewWillAppear);

  Probe *b = push(navigationViewController);

  $((ViewController *) navigationViewController, viewDidAppear);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(0, a->didAppear);
  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(pushWhileDisappearingCompletesThePrevious) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  appear(navigationViewController);

  $((ViewController *) navigationViewController, viewWillDisappear);

  Probe *b = push(navigationViewController);

  $((ViewController *) navigationViewController, viewDidDisappear);

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, b->didDisappear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(onlyTopViewIsShown) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);

  View *view = ((ViewController *) navigationViewController)->view;

  ck_assert_ptr_null(a->viewController.view->superview);
  ck_assert_ptr_eq(view, b->viewController.view->superview);

  $(navigationViewController, popViewController);

  ck_assert_ptr_eq(view, a->viewController.view->superview);
  ck_assert_ptr_null(b->viewController.view->superview);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(popToWhileHiddenSendsNoAppearance) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);
  Probe *c = push(navigationViewController);

  $(navigationViewController, popToViewController, (ViewController *) a);

  ck_assert_ptr_eq(a, $(navigationViewController, topViewController));
  ck_assert_ptr_eq(((ViewController *) navigationViewController)->view, a->viewController.view->superview);

  ck_assert_int_eq(0, a->willAppear);
  ck_assert_int_eq(0, b->willAppear);
  ck_assert_int_eq(0, c->willDisappear);

  release(a);
  release(b);
  release(c);
  release(navigationViewController);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("NavigationViewController");
  tcase_add_test(tcase, onlyTopAppearsAndReceivesEvents);
  tcase_add_test(tcase, pushWhileVisibleCoversTheTop);
  tcase_add_test(tcase, pushAndPopWhileHiddenSendNoAppearance);
  tcase_add_test(tcase, popToSkipsCoveredViewControllers);
  tcase_add_test(tcase, pushWhileAppearingAppearsItOnce);
  tcase_add_test(tcase, pushWhileDisappearingCompletesThePrevious);
  tcase_add_test(tcase, onlyTopViewIsShown);
  tcase_add_test(tcase, popToWhileHiddenSendsNoAppearance);

  Suite *suite = suite_create("NavigationViewController");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
