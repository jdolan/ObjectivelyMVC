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
  NavigationViewController *navigationViewController;
  ViewController *pushOnWillAppear, *pushOnDidDisappear;
  bool popOnWillAppear;
} Probe;

static int didDisappearTotal;

static void Probe_respondToEvent(ViewController *self, const SDL_Event *event) {
  ((Probe *) self)->events++;
}

static void Probe_viewWillAppear(ViewController *self) {

  Probe *this = (Probe *) self;

  this->willAppear++;

  if (this->pushOnWillAppear) {
    ViewController *viewController = this->pushOnWillAppear;
    this->pushOnWillAppear = NULL;
    $(this->navigationViewController, pushViewController, viewController);
  }

  if (this->popOnWillAppear) {
    this->popOnWillAppear = false;
    $(this->navigationViewController, popViewController);
  }
}

static void Probe_viewDidAppear(ViewController *self) {
  ((Probe *) self)->didAppear++;
}

static void Probe_viewWillDisappear(ViewController *self) {
  ((Probe *) self)->willDisappear++;
}

static void Probe_viewDidDisappear(ViewController *self) {

  Probe *this = (Probe *) self;

  this->didDisappear++;
  didDisappearTotal++;

  if (this->pushOnDidDisappear) {
    ViewController *viewController = this->pushOnDidDisappear;
    this->pushOnDidDisappear = NULL;
    $(this->navigationViewController, pushViewController, viewController);
  }
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

static Probe *probe(NavigationViewController *navigationViewController) {

  Probe *probe = (Probe *) $((ViewController *) alloc(Probe), init);

  probe->navigationViewController = navigationViewController;

  return probe;
}

static Probe *push(NavigationViewController *navigationViewController) {

  Probe *p = probe(navigationViewController);

  $(navigationViewController, pushViewController, (ViewController *) p);

  return p;
}

static bool isAttached(NavigationViewController *navigationViewController, Probe *probe) {
  return probe->viewController.view->superview == ((ViewController *) navigationViewController)->view;
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

START_TEST(onlyTopViewIsVisible) {

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

START_TEST(pushFromWillAppearShowsOnlyTheFinalTop) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  appear(navigationViewController);

  Probe *b = probe(navigationViewController);
  Probe *c = probe(navigationViewController);

  b->pushOnWillAppear = (ViewController *) c;

  $(navigationViewController, pushViewController, (ViewController *) b);

  ck_assert_ptr_eq(c, $(navigationViewController, topViewController));

  ck_assert(isAttached(navigationViewController, c));
  ck_assert(!isAttached(navigationViewController, b));
  ck_assert(!isAttached(navigationViewController, a));

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(0, b->didAppear);
  ck_assert_int_eq(1, b->willDisappear);
  ck_assert_int_eq(1, b->didDisappear);

  ck_assert_int_eq(1, c->willAppear);
  ck_assert_int_eq(1, c->didAppear);

  release(a);
  release(b);
  release(c);
  release(navigationViewController);

} END_TEST

START_TEST(popFromWillAppearShowsThePrevious) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);

  appear(navigationViewController);

  Probe *b = probe(navigationViewController);
  b->popOnWillAppear = true;

  $(navigationViewController, pushViewController, (ViewController *) b);

  ck_assert_ptr_eq(a, $(navigationViewController, topViewController));

  ck_assert(isAttached(navigationViewController, a));
  ck_assert(!isAttached(navigationViewController, b));

  ck_assert_int_eq(0, b->didAppear);
  ck_assert_int_eq(1, b->willDisappear);
  ck_assert_int_eq(1, b->didDisappear);

  ck_assert_int_eq(2, a->willAppear);
  ck_assert_int_eq(2, a->didAppear);
  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(popWhileDisappearingRetainsTheOutgoingTop) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);

  appear(navigationViewController);

  release(b);

  didDisappearTotal = 0;

  $((ViewController *) navigationViewController, viewWillDisappear);
  $(navigationViewController, popViewController);

  ck_assert_int_eq(0, didDisappearTotal);

  $((ViewController *) navigationViewController, viewDidDisappear);

  ck_assert_int_eq(1, didDisappearTotal);

  ck_assert_int_eq(0, a->willAppear);
  ck_assert_int_eq(0, a->didDisappear);

  release(a);
  release(navigationViewController);

} END_TEST

START_TEST(popToDisappearsAControllerPushedOnTheWay) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);
  Probe *c = probe(navigationViewController);

  appear(navigationViewController);

  b->pushOnDidDisappear = (ViewController *) c;

  $(navigationViewController, popToViewController, (ViewController *) a);

  ck_assert_ptr_eq(a, $(navigationViewController, topViewController));

  ck_assert(isAttached(navigationViewController, a));
  ck_assert(!isAttached(navigationViewController, c));

  ck_assert_int_eq(1, c->willAppear);
  ck_assert_int_eq(1, c->didAppear);
  ck_assert_int_eq(1, c->willDisappear);
  ck_assert_int_eq(1, c->didDisappear);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->didAppear);
  ck_assert_int_eq(0, a->willDisappear);

  release(a);
  release(b);
  release(c);
  release(navigationViewController);

} END_TEST

START_TEST(pushFromDidDisappearDuringPushIsCovered) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = probe(navigationViewController);
  Probe *y = probe(navigationViewController);

  appear(navigationViewController);

  a->pushOnDidDisappear = (ViewController *) y;

  $(navigationViewController, pushViewController, (ViewController *) b);

  ck_assert_ptr_eq(b, $(navigationViewController, topViewController));

  ck_assert(isAttached(navigationViewController, b));
  ck_assert(!isAttached(navigationViewController, y));
  ck_assert(!isAttached(navigationViewController, a));

  ck_assert_int_eq(1, y->willAppear);
  ck_assert_int_eq(1, y->didAppear);
  ck_assert_int_eq(1, y->willDisappear);
  ck_assert_int_eq(1, y->didDisappear);

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(y);
  release(navigationViewController);

} END_TEST

START_TEST(pushFromDidDisappearDuringPopAppearsOnce) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);
  Probe *y = probe(navigationViewController);

  appear(navigationViewController);

  b->pushOnDidDisappear = (ViewController *) y;

  $(navigationViewController, popViewController);

  ck_assert_ptr_eq(y, $(navigationViewController, topViewController));

  ck_assert(isAttached(navigationViewController, y));
  ck_assert(!isAttached(navigationViewController, a));

  ck_assert_int_eq(1, y->willAppear);
  ck_assert_int_eq(1, y->didAppear);

  ck_assert_int_eq(0, a->willAppear);
  ck_assert_int_eq(0, a->willDisappear);

  release(a);
  release(b);
  release(y);
  release(navigationViewController);

} END_TEST

START_TEST(popToDisappearsACoveredControllerPushedAgain) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = push(navigationViewController);
  Probe *c = push(navigationViewController);

  appear(navigationViewController);

  c->pushOnDidDisappear = (ViewController *) b;

  $(navigationViewController, popToViewController, (ViewController *) a);

  ck_assert_ptr_eq(a, $(navigationViewController, topViewController));
  ck_assert_ptr_eq(a, navigationViewController->visibleViewController);

  ck_assert(isAttached(navigationViewController, a));
  ck_assert(!isAttached(navigationViewController, b));

  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);
  ck_assert_int_eq(1, b->willDisappear);
  ck_assert_int_eq(1, b->didDisappear);

  ck_assert_int_eq(1, a->willAppear);
  ck_assert_int_eq(1, a->didAppear);

  release(a);
  release(b);
  release(c);
  release(navigationViewController);

} END_TEST

START_TEST(addChildViewControllerCoversTheTop) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = probe(navigationViewController);

  appear(navigationViewController);

  $((ViewController *) navigationViewController, addChildViewController, (ViewController *) b);

  ck_assert(!isAttached(navigationViewController, a));
  ck_assert(isAttached(navigationViewController, b));

  ck_assert_int_eq(1, a->willDisappear);
  ck_assert_int_eq(1, a->didDisappear);
  ck_assert_int_eq(1, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);

  release(a);
  release(b);
  release(navigationViewController);

} END_TEST

START_TEST(pushAndPopFromWillAppearAppearsOnce) {

  NavigationViewController *navigationViewController = $(alloc(NavigationViewController), init);

  Probe *a = push(navigationViewController);
  Probe *b = probe(navigationViewController);
  Probe *c = probe(navigationViewController);

  appear(navigationViewController);

  b->pushOnWillAppear = (ViewController *) c;
  b->popOnWillAppear = true;

  $(navigationViewController, pushViewController, (ViewController *) b);

  ck_assert_ptr_eq(b, $(navigationViewController, topViewController));
  ck_assert(isAttached(navigationViewController, b));

  ck_assert_int_eq(2, b->willAppear);
  ck_assert_int_eq(1, b->didAppear);
  ck_assert_int_eq(1, b->willDisappear);
  ck_assert_int_eq(1, b->didDisappear);

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
  tcase_add_test(tcase, onlyTopViewIsVisible);
  tcase_add_test(tcase, popToWhileHiddenSendsNoAppearance);
  tcase_add_test(tcase, pushFromWillAppearShowsOnlyTheFinalTop);
  tcase_add_test(tcase, popFromWillAppearShowsThePrevious);
  tcase_add_test(tcase, popWhileDisappearingRetainsTheOutgoingTop);
  tcase_add_test(tcase, popToDisappearsAControllerPushedOnTheWay);
  tcase_add_test(tcase, pushFromDidDisappearDuringPushIsCovered);
  tcase_add_test(tcase, pushFromDidDisappearDuringPopAppearsOnce);
  tcase_add_test(tcase, popToDisappearsACoveredControllerPushedAgain);
  tcase_add_test(tcase, addChildViewControllerCoversTheTop);
  tcase_add_test(tcase, pushAndPopFromWillAppearAppearsOnce);

  Suite *suite = suite_create("NavigationViewController");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
