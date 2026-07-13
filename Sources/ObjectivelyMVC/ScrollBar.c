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

#include <assert.h>

#include "ScrollBar.h"
#include "ScrollView.h"

#define _Class _ScrollBar

const EnumName ScrollBarVisibilityNames[] = MakeEnumNames(
  MakeEnumAlias(ScrollBarShow, show),
  MakeEnumAlias(ScrollBarHide, hide),
  MakeEnumAlias(ScrollBarAuto, auto)
);

#pragma mark - Delegate callbacks

/**
 * @brief ScrollHandleDelegate; maps handle travel onto the ScrollView's content offset.
 */
static void didDragHandle(ScrollHandle *handle, float delta) {

  ScrollBar *self = handle->delegate.self;

  if (self->scrollView) {

    const SDL_Rect track = $((View *) self, bounds);
    const SDL_Size content = $((View *) self->scrollView->contentView, size);
    const SDL_Rect visible = $((View *) self->scrollView, bounds);

    const int scrollRange = content.h - visible.h;
    const int travel = track.h - ((View *) self->handle)->frame.h;

    if (scrollRange > 0 && travel > 0) {

      SDL_Point offset = self->scrollView->contentOffset;
      offset.y -= (int) (delta * ((float) scrollRange / travel));

      $(self->scrollView, scrollToOffset, &offset);
      ((View *) self)->needsLayout = true;
    }
  }
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  ScrollBar *this = (ScrollBar *) self;

  release(this->handle);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((ScrollBar *) self, initWithScrollView, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  ScrollBar *this = (ScrollBar *) self;

  super(View, self, layoutSubviews);

  if (this->scrollView == NULL || this->scrollView->contentView == NULL) {
    return;
  }

  const SDL_Rect track = $(self, bounds);
  const SDL_Size content = $(this->scrollView->contentView, size);
  const SDL_Rect visible = $((View *) this->scrollView, bounds);

  View *handle = (View *) this->handle;

  if (content.h <= visible.h || track.h <= 0) {
    handle->frame = (SDL_Rect) { 0, 0, track.w, track.h };
  } else {

    const int scrollRange = content.h - visible.h;

    int handleH = (int) ((float) track.h * visible.h / content.h);
    handleH = clamp(handleH, handle->minSize.h, track.h);

    const int travel = track.h - handleH;
    const float fraction = clamp((float) (-this->scrollView->contentOffset.y) / scrollRange, 0.f, 1.f);

    handle->frame = (SDL_Rect) { 0, (int) (fraction * travel), track.w, handleH };
  }
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

  ScrollBar *this = (ScrollBar *) self;

  switch (event->type) {

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (this->scrollView) {

        const SDL_Point point = MakePoint(event->button.x, event->button.y);
        const SDL_Rect handleFrame = $((View *) this->handle, renderFrame);
        const SDL_Rect visible = $((View *) this->scrollView, bounds);

        SDL_Point offset = this->scrollView->contentOffset;

        if (point.y < handleFrame.y) {
          offset.y += visible.h;
        } else if (point.y > handleFrame.y + handleFrame.h) {
          offset.y -= visible.h;
        }

        $(this->scrollView, scrollToOffset, &offset);
        self->needsLayout = true;
      }
      return;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      $(self, resignTouchResponder);
      return;

    case SDL_EVENT_MOUSE_MOTION:
      return;

    default:
      break;
  }

  super(View, self, respondToEvent, event);
}

#pragma mark - ScrollBar

/**
 * @fn ScrollBar *ScrollBar::initWithScrollView(ScrollBar *self, ScrollView *scrollView)
 * @memberof ScrollBar
 */
static ScrollBar *initWithScrollView(ScrollBar *self, ScrollView *scrollView) {

  self = (ScrollBar *) super(View, self, initWithFrame, NULL);
  if (self) {

    $((View *) self, addClassName, "scrollBar");
    ((View *) self)->clipsSubviews = true;

    self->handle = $(alloc(ScrollHandle), initWithFrame, NULL);
    assert(self->handle);
    self->handle->delegate.self = self;
    self->handle->delegate.didDrag = didDragHandle;
    $((View *) self, addSubview, (View *) self->handle);

    $(self, setScrollView, scrollView);
  }

  return self;
}

/**
 * @fn void ScrollBar::setScrollView(ScrollBar *self, ScrollView *scrollView)
 * @memberof ScrollBar
 */
static void setScrollView(ScrollBar *self, ScrollView *scrollView) {

  self->scrollView = scrollView;

  ((View *) self)->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

  ((ScrollBarInterface *) clazz->interface)->initWithScrollView = initWithScrollView;
  ((ScrollBarInterface *) clazz->interface)->setScrollView = setScrollView;
}

/**
 * @fn Class *ScrollBar::_ScrollBar(void)
 * @memberof ScrollBar
 */
Class *_ScrollBar(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "ScrollBar",
      .superclass = _View(),
      .instanceSize = sizeof(ScrollBar),
      .interfaceOffset = offsetof(ScrollBar, interface),
      .interfaceSize = sizeof(ScrollBarInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
