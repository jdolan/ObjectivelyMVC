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

#include "ScrollView.h"

#define _Class _ScrollView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  ScrollView *this = (ScrollView *) self;

  release(this->contentView);
  release(this->scrollBar);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  ScrollView *this = (ScrollView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("scrollbarVisibility", InletTypeEnum, &this->scrollBarVisibility, (ident) ScrollBarVisibilityNames),
    MakeInlet("step", InletTypeFloat, &this->step, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  ScrollView *this = (ScrollView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("scrollbar", InletTypeEnum, &this->scrollBarVisibility, (ident) ScrollBarVisibilityNames)
  );

  $(self, bind, inlets, (Dictionary *) style->attributes);

  self->needsLayout = true;
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  ScrollView *this = (ScrollView *) self;

  bool showScrollBar;
  switch (this->scrollBarVisibility) {
    case ScrollBarShow:
      showScrollBar = true;
      break;
    case ScrollBarHide:
      showScrollBar = false;
      break;
    default: {
      if (this->contentView == NULL) {
        showScrollBar = false;
      } else {
        const SDL_Size contentSize = $(this->contentView, size);
        const SDL_Rect bounds = $(self, bounds);
        showScrollBar = contentSize.h > bounds.h;
      }
      break;
    }
  }
  
  ((View *) this->scrollBar)->hidden = !showScrollBar;

  super(View, self, layoutSubviews);

  if (this->contentView) {
    this->contentView->frame.x = this->contentOffset.x;
    this->contentView->frame.y = this->contentOffset.y;
  }

  if (showScrollBar) {
    ((View *) this->scrollBar)->needsLayout = true;
  }
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  ScrollView *this = (ScrollView *) self;

  if (event->type == SDL_EVENT_MOUSE_MOTION && (event->motion.state & SDL_BUTTON_LMASK)) {
    self->state |= ControlStateHighlighted;

    SDL_Point offset = this->contentOffset;

    offset.x += event->motion.xrel;
    offset.y += event->motion.yrel;

    $(this, scrollToOffset, &offset);
    return true;
  } else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
    SDL_Point offset = this->contentOffset;

    offset.x -= event->wheel.x * this->step;
    offset.y += event->wheel.y * this->step;

    $(this, scrollToOffset, &offset);
    return true;
  } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP && (event->button.button & SDL_BUTTON_LMASK)) {

    if ($(self, isHighlighted)) {
      self->state &= ~ControlStateHighlighted;
      return true;
    }
  }

  return super(Control, self, captureEvent, event);
}

#pragma mark - ScrollView

/**
 * @fn ScrollView *ScrollView::initWithFrame(ScrollView *self, const SDL_Rect *frame)
 * @memberof ScrollView
 */
static ScrollView *initWithFrame(ScrollView *self, const SDL_Rect *frame) {

  self = (ScrollView *) super(Control, self, initWithFrame, frame);
  if (self) {
    self->step = 12.f;
    self->scrollBarVisibility = ScrollBarAuto;

    self->scrollBar = $(alloc(ScrollBar), initWithScrollView, self);
    assert(self->scrollBar);

    View *scrollBar = (View *) self->scrollBar;
    scrollBar->alignment = ViewAlignmentRight;
    scrollBar->autoresizingMask = ViewAutoresizingHeight;
    scrollBar->hidden = true;

    $((View *) self, addSubview, scrollBar);
  }

  return self;
}

/**
 * @brief Scrolls the content View to the specified offset.
 * @memberof ScrollView
 */
static void scrollToOffset(ScrollView *self, const SDL_Point *offset) {

  if (self->contentView) {
    const SDL_Size contentSize = $(self->contentView, size);
    const SDL_Rect bounds = $((View *) self, bounds);

    if (contentSize.w > bounds.w) {
      self->contentOffset.x = clamp(offset->x, -(contentSize.w - bounds.w), 0);
    } else {
      self->contentOffset.x = 0;
    }

    if (contentSize.h > bounds.h) {
      self->contentOffset.y = clamp(offset->y, -(contentSize.h - bounds.h), 0);
    } else {
      self->contentOffset.y = 0;
    }

  } else {
    self->contentOffset.x = self->contentOffset.y = 0;
  }

  self->control.view.needsLayout = true;
  ((View *) self->scrollBar)->needsLayout = true;
}

/**
 * @fn void ScrollView::setContentView(ScrollView *self, View *contentView)
 * @memberof ScrollView
 */
static void setContentView(ScrollView *self, View *contentView) {

  if (contentView != self->contentView) {

    if (self->contentView) {
      $(self->contentView, removeClassName, "contentView");
      $((View *) self, removeSubview, self->contentView);
      self->contentView = release(self->contentView);
    }

    if (contentView) {
      $(contentView, addClassName, "contentView");
      $((View *) self, addSubview, contentView);
      self->contentView = retain(contentView);
    }
    
    $((View *) self, addSubview, (View *) self->scrollBar);

    $(self, scrollToOffset, &MakePoint(0, 0));
  }
}

/**
 * @fn void ScrollView::setScrollBarVisibility(ScrollView *self, ScrollBarVisibility visibility)
 * @memberof ScrollView
 */
static void setScrollBarVisibility(ScrollView *self, ScrollBarVisibility visibility) {

  self->scrollBarVisibility = visibility;
  self->control.view.needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((ScrollViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((ScrollViewInterface *) clazz->interface)->scrollToOffset = scrollToOffset;
  ((ScrollViewInterface *) clazz->interface)->setContentView = setContentView;
  ((ScrollViewInterface *) clazz->interface)->setScrollBarVisibility = setScrollBarVisibility;
}

/**
 * @fn Class *ScrollView::_ScrollView(void)
 * @memberof ScrollView
 */
Class *_ScrollView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "ScrollView",
      .superclass = _Control(),
      .instanceSize = sizeof(ScrollView),
      .interfaceOffset = offsetof(ScrollView, interface),
      .interfaceSize = sizeof(ScrollViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
