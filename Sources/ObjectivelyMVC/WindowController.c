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

#include <assert.h>

#include <Objectively/String.h>

#include "Log.h"
#include "WindowController.h"

#define _Class _WindowController

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  WindowController *this = (WindowController *) self;

  release(this->debugViewController);
  release(this->renderer);
  release(this->viewController);

  super(Object, self, dealloc);
}

#pragma mark - WindowController

/**
 * @fn void WindowController::debug(WindowController *self)
 * @memberof WindowController
 */
static void debug(WindowController *self) {

  if (self->viewController && self->debugViewController) {

    ViewController *debugViewController = (ViewController *) self->debugViewController;

    SDL_Point point = MakePoint(0, 0);
    SDL_GetMouseState(&point.x, &point.y);

    View *view = $(self->viewController->view, hitTest, &point);
    if (view) {
      $(self->debugViewController, debug, view, self->renderer);
    }

    $(debugViewController->view, applyThemeIfNeeded, self->theme);
    $(debugViewController->view, layoutIfNeeded);
    $(debugViewController->view, draw, self->renderer);
  }
}

/**
 * @fn View *WindowController::eventTarget(const WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static View *eventTarget(const WindowController *self, const SDL_Event *event) {

  SDL_Point point;

  switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      point = MakePoint(event->button.x, event->button.y);
      break;
    case SDL_MOUSEMOTION:
      point = MakePoint(event->motion.x, event->motion.y);
      break;
    case SDL_MOUSEWHEEL:
      SDL_GetMouseState(&point.x, &point.y);
      break;
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
      point = MakePoint(event->tfinger.x, event->tfinger.y);
      break;
    default:
      return NULL;
  }

  View *view = $(self->viewController->view, hitTest, &point);
  return view;
}

/**
 * @fn View *WindowController::firstResponder(const WindowController *self)
 * @memberof WindowController
 */
static View *firstResponder(const WindowController *self) {
  return SDL_GetWindowData(self->window, "firstResponder");
}

/**
 * @brief ViewEnumerator for finding the next first responder.
 */
static void enumerateFirstResponders(View *view, ident data) {

  if ($(view, acceptsFirstResponder) && $(view, isVisible)) {
    $((MutableArray *) data, addObject, view);
  }
}

/**
 * @fn Array *WindowController::firstResponders(const WindowController *self)
 * @memberof WindowController
 */
static Array *firstResponders(const WindowController *self) {

  Array *array = (Array *) $(alloc(MutableArray), init);
  $(self->viewController->view, enumerateDescendants, enumerateFirstResponders, array);

  return array;
}

/**
 * @fn WindowController *WindowController::initWithWindow(WindowController *self, SDL_Window *window)
 * @memberof WindowController
 */
static WindowController *initWithWindow(WindowController *self, SDL_Window *window) {

  self = (WindowController *) super(Object, self, init);
  if (self) {
    $(self, setWindow, window);
    $(self, setViewController, NULL);
    $(self, setRenderer, NULL);
    $(self, setTheme, NULL);
  }

  return self;
}

/**
 * @fn void WindowController::render(WindowController *self)
 * @memberof WindowController
 */
static void render(WindowController *self) {

  assert(self->renderer);

  $(self->renderer, beginFrame);

  $(self->viewController->view, applyThemeIfNeeded, self->theme);
  $(self->viewController->view, layoutIfNeeded);

  $(self->viewController->view, draw, self->renderer);

  $(self, debug);

  $(self->renderer, endFrame);
}

/**
 * @brief ViewEnumerator for `SDL_MOUSEMOTION_EVENT` which dispatches `ViewEventMouseEnter`
 * and `ViewEventMouseLeave`.
 */
static void mouseMotion_enumerate(View *view, ident data) {
  
  const SDL_MouseMotionEvent *event = data;
  const SDL_Point a = MakePoint(event->x - event->xrel, event->y - event->yrel);
  const SDL_Point b = MakePoint(event->x, event->y);

  if ($(view, containsPoint, &a) && !$(view, containsPoint, &b)) {
    $(view, emitViewEvent, ViewEventMouseLeave, NULL);
  } else if ($(view, containsPoint, &b) && !$(view, containsPoint, &a)) {
  } else if ($(view, containsPoint, &b) && !$(view, containsPoint, &a)) {
    $(view, emitViewEvent, ViewEventMouseEnter, NULL);
  }
}

/**
 * @fn View *WindowController::nextFirstResponder(const WindowController *self, View *firstResponder)
 * @memberof WindowController
 */
static View *nextFirstResponder(const WindowController *self, View *firstResponder) {

  View *next = NULL;

  Array *array = $(self, firstResponders);

  if (array->count) {
    if (firstResponder && $(array, containsObject, firstResponder)) {
      const ssize_t index = $(array, indexOfObject, firstResponder);
      next = $(array, objectAtIndex, (index + 1) % array->count);
    } else {
      next = $(array, firstObject);
    }
  }

  release(array);
  return next;
}

/**
 * @fn View *WindowController::previousFirstResponder(const WindowController *self, View *firstResponder)
 * @memberof WindowController
 */
static View *previousFirstResponder(const WindowController *self, View *firstResponder) {

  View *prev = NULL;

  Array *array = $(self, firstResponders);

  if (array->count) {
    if (firstResponder && $(array, containsObject, firstResponder)) {
      const ssize_t index = $(array, indexOfObject, firstResponder);
      prev = $(array, objectAtIndex, (index + array->count - 1) % array->count);
    } else {
      prev = $(array, firstObject);
    }
  }

  release(array);
  return prev;
}

/**
 * @fn void WindowController::respondToEvent(WindowController *self, const SDL_Event *event)
 * @memberof WindowController
 */
static void respondToEvent(WindowController *self, const SDL_Event *event) {

  SDL_SetWindowData(self->window, "event", (ident) event);

  if (event->type == SDL_WINDOWEVENT) {
    switch (event->window.event) {
      case SDL_WINDOWEVENT_EXPOSED:
        $(self, setWindow, SDL_GL_GetCurrentWindow());
        $(self->renderer, renderDeviceDidReset);
        $(self->viewController->view, renderDeviceDidReset);
        $(self->viewController->view, updateBindings);
        break;
      case SDL_WINDOWEVENT_CLOSE:
        $(self->renderer, renderDeviceWillReset);
        $(self->viewController->view, renderDeviceWillReset);
        break;
      default:
        break;
    }
  }

  if (event->type == SDL_MOUSEMOTION) {
    $(self->viewController->view, enumerateVisible, mouseMotion_enumerate, (ident) event);
  }

  View *view = NULL;

  switch (event->type) {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
    case SDL_TEXTINPUT:
      view = $(self, firstResponder);
      if (view) {
        $(view, respondToEvent, event);
      } else {
        $(self->viewController, respondToEvent, event);
      }
      break;
    default:
      view = $(self, eventTarget, event);
      if (view) {
        $(view, respondToEvent, event);
      }
      $(self->viewController, respondToEvent, event);
      break;
  }

  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.sym == SDLK_TAB) {

      if (event->key.keysym.mod & KMOD_SHIFT) {
        view = $(self, previousFirstResponder, view);
      } else {
        view = $(self, nextFirstResponder, view);
      }

      if (view) {
        $(view, becomeFirstResponder);
      }
    }

    if (event->key.keysym.sym == SDLK_d) {
      if (event->key.keysym.mod & KMOD_CTRL) {
        $(self, toggleDebugger);
      }
    }
  }

  SDL_SetWindowData(self->window, "event", NULL);
}

/**
 * @fn void WindowController::setRenderer(WindowController *self, Renderer *renderer)
 * @memberof WindowController
 */
static void setRenderer(WindowController *self, Renderer *renderer) {

  if (self->renderer != renderer || self->renderer == NULL) {

    release(self->renderer);

    if (renderer) {
      self->renderer = retain(renderer);
    } else {
      self->renderer = $(alloc(Renderer), init);
    }

    assert(self->renderer);

    $(self->viewController->view, renderDeviceDidReset);
  }
}

/**
 * @fn void WindowController::setTheme(WindowController *self, Theme *theme)
 * @memberof WindowController
 */
static void setTheme(WindowController *self, Theme *theme) {

  if (self->theme != theme || self->theme == NULL) {

    release(self->theme);

    if (theme) {
      self->theme = retain(theme);
    } else {
      self->theme = $(alloc(Theme), init);
    }
  }

  $(self->viewController->view, invalidateStyle);
}

/**
 * @fn void WindowController::setViewController(WindowController *self, ViewController *viewController)
 * @memberof WindowController
 */
static void setViewController(WindowController *self, ViewController *viewController) {

  if (self->viewController != viewController || self->viewController == NULL) {

    if (self->viewController) {
      $(self->viewController, viewWillDisappear);
      $(self->viewController->view, moveToWindow, NULL);
      $(self->viewController, viewDidDisappear);
    }

    release(self->viewController);

    if (viewController) {
      self->viewController = retain(viewController);
    } else {
      self->viewController = $(alloc(ViewController), init);
    }

    $(self->viewController, loadViewIfNeeded);

    $(self->viewController, viewWillAppear);
    $(self->viewController->view, moveToWindow, self->window);
    $(self->viewController, viewDidAppear);
  }
}

/**
 * @fn void WindowController::setWindow(WindowController *self, SDL_Window *window)
 * @memberof WindowController
 */
static void setWindow(WindowController *self, SDL_Window *window) {

  self->window = window;
  assert(self->window);

  const Uint32 flags = SDL_GetWindowFlags(self->window);
  assert(flags & SDL_WINDOW_OPENGL);

  SDL_SetWindowData(self->window, "windowController", self);
  assert(SDL_GetWindowData(self->window, "windowController") == self);

  if (self->viewController) {
    $(self->viewController->view, moveToWindow, self->window);
  }

  if (self->debugViewController) {
    $(self->debugViewController->viewController.view, moveToWindow, self->window);
  }
}

/**
 * @fn void WindowController::toggleDebugger(WindowController *self)
 * @memberof WindowController
 */
static void toggleDebugger(WindowController *self) {

  if (self->debugViewController == NULL) {

    self->debugViewController = $(alloc(DebugViewController), init);
    assert(self->debugViewController);

    ViewController *debugViewController = (ViewController *) self->debugViewController;

    $(debugViewController, loadView);
    $(debugViewController, viewWillAppear);
    $(debugViewController->view, moveToWindow, self->window);
    $(debugViewController, viewDidAppear);

    if (self->viewController) {
      $(self->viewController->view, addClassName, "debug");
    }
  } else {
    self->debugViewController = release(self->debugViewController);

    if (self->viewController) {
      $(self->viewController->view, removeClassName, "debug");
    }
  }
}

/**
 * @fn WindowController *WindowController::windowController(SDL_Window *window)
 * @memberof WindowController
 */
static WindowController *windowController(SDL_Window *window) {

  assert(window);

  return SDL_GetWindowData(window, "windowController");
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((WindowControllerInterface *) clazz->interface)->debug = debug;
  ((WindowControllerInterface *) clazz->interface)->eventTarget = eventTarget;
  ((WindowControllerInterface *) clazz->interface)->firstResponder = firstResponder;
  ((WindowControllerInterface *) clazz->interface)->firstResponders = firstResponders;
  ((WindowControllerInterface *) clazz->interface)->initWithWindow = initWithWindow;
  ((WindowControllerInterface *) clazz->interface)->nextFirstResponder = nextFirstResponder;
  ((WindowControllerInterface *) clazz->interface)->previousFirstResponder = previousFirstResponder;
  ((WindowControllerInterface *) clazz->interface)->render = render;
  ((WindowControllerInterface *) clazz->interface)->respondToEvent = respondToEvent;
  ((WindowControllerInterface *) clazz->interface)->setRenderer = setRenderer;
  ((WindowControllerInterface *) clazz->interface)->setTheme = setTheme;
  ((WindowControllerInterface *) clazz->interface)->setViewController = setViewController;
  ((WindowControllerInterface *) clazz->interface)->setWindow = setWindow;
  ((WindowControllerInterface *) clazz->interface)->toggleDebugger = toggleDebugger;
  ((WindowControllerInterface *) clazz->interface)->windowController = windowController;
}

/**
 * @fn Class *WindowController::_WindowController(void)
 * @memberof WindowController
 */
Class *_WindowController(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "WindowController",
      .superclass = _Object(),
      .instanceSize = sizeof(WindowController),
      .interfaceOffset = offsetof(WindowController, interface),
      .interfaceSize = sizeof(WindowControllerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
