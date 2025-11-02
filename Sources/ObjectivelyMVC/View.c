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
#include <string.h>

#include <Objectively.h>

#include "Log.h"
#include "View.h"
#include "ViewController.h"
#include "WindowController.h"
#include "Window.h"

Uint32 MVC_NOTIFICATION_EVENT;
Uint32 MVC_VIEW_EVENT;

const EnumName ViewAlignmentNames[] = MakeEnumNames(
  MakeEnumAlias(ViewAlignmentNone, none),
  MakeEnumAlias(ViewAlignmentTop, top),
  MakeEnumAlias(ViewAlignmentMiddle, middle),
  MakeEnumAlias(ViewAlignmentBottom, bottom),
  MakeEnumAlias(ViewAlignmentLeft, left),
  MakeEnumAlias(ViewAlignmentCenter, center),
  MakeEnumAlias(ViewAlignmentRight, right),
  MakeEnumAlias(ViewAlignmentTopLeft, top-left),
  MakeEnumAlias(ViewAlignmentTopCenter, top-center),
  MakeEnumAlias(ViewAlignmentTopRight, top-right),
  MakeEnumAlias(ViewAlignmentMiddleLeft, middle-left),
  MakeEnumAlias(ViewAlignmentMiddleCenter, middle-center),
  MakeEnumAlias(ViewAlignmentMiddleRight, middle-right),
  MakeEnumAlias(ViewAlignmentBottomLeft, bottom-left),
  MakeEnumAlias(ViewAlignmentBottomCenter, bottom-center),
  MakeEnumAlias(ViewAlignmentBottomRight, bottom-right),
  MakeEnumAlias(ViewAlignmentInternal, internal)
);

const EnumName ViewAutoresizingNames[] = MakeEnumNames(
  MakeEnumAlias(ViewAutoresizingNone, none),
  MakeEnumAlias(ViewAutoresizingWidth, width),
  MakeEnumAlias(ViewAutoresizingHeight, height),
  MakeEnumAlias(ViewAutoresizingFill, fill),
  MakeEnumAlias(ViewAutoresizingFit, fit),
  MakeEnumAlias(ViewAutoresizingContain, contain)
);

#define _Class _View

#pragma mark - ObjectInterface

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  View *this = (View *) self;

  release(this->subviews);

  $(this, resignFirstResponder);

  free(this->identifier);

  release(this->classNames);
  release(this->computedStyle);
  release(this->style);
  release(this->stylesheet);
  release(this->warnings);

  this->superview = NULL;

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  View *this = (View *) self;
  const SDL_Rect bounds = $(this, bounds);

  String *classNames = $((Object *) this->classNames, description);
  String *description = str("%s@%p %s [%d, %d, %d, %d]",
                this->identifier ?: classnameof(self),
                self,
                classNames->chars,
                bounds.x, bounds.y, bounds.w, bounds.h);

  release(classNames);
  return description;
}

#pragma mark - View

/**
 * @fn bool View::acceptsFirstResponder(const View *self)
 * @memberof View
 */
static bool acceptsFirstResponder(const View *self) {
  return false;
}

/**
 * @fn void View::addClassName(View *self, const char *className)
 * @memberof View
 */
static void addClassName(View *self, const char *className) {

  if (className) {

    String *string = $$(String, stringWithCharacters, className);
    assert(string);

    $(self->classNames, addObject, string);
    release(string);

    $(self, invalidateStyle);
  }
}

/**
 * @fn void View::addSubview(View *self, View *subview)
 * @memberof View
 */
static void addSubview(View *self, View *subview) {
  $(self, addSubviewRelativeTo, subview, NULL, ViewPositionAfter);
}

/**
 * @fn void View::addSubviewRelativeTo(View *self, View *subview, View *other, ViewPosition position)
 * @memberof View
 */
static void addSubviewRelativeTo(View *self, View *subview, View *other, ViewPosition position) {

  assert(subview);
  assert(subview != other);

  retain(subview);

  $(subview, removeFromSuperview);

  if (other && other->superview == self) {

    const Array *subviews = (Array *) self->subviews;
    const ssize_t index = $(subviews, indexOfObject, other);

    if (position == ViewPositionAfter) {
      if (index == (ssize_t) (subviews->count - 1)) {
        $(self->subviews, addObject, subview);
      } else {
        $(self->subviews, insertObjectAtIndex, subview, index + 1);
      }
    } else {
      $(self->subviews, insertObjectAtIndex, subview, index);
    }
  } else {
    $(self->subviews, addObject, subview);
  }

  release(subview);

  subview->superview = self;

  $(subview, moveToWindow, self->window);

  $(subview, invalidateStyle);

  self->needsLayout = true;
}

/**
 * @fn View::ancestorWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *ancestorWithIdentifier(const View *self, const char *identifier) {

  assert(identifier);

  View *view = (View *) self;
  while (view) {
    if (view->identifier) {
      if (strcmp(identifier, view->identifier) == 0) {
        return view;
      }
    }
    view = view->superview;
  }

  return NULL;
}

/**
 * @fn void View::applyStyle(View *self, const Style *style)
 * @memberof View
 */
static void applyStyle(View *self, const Style *style) {

  assert(style);

  const Inlet inlets[] = MakeInlets(
    MakeInlet("alignment", InletTypeEnum, &self->alignment, (ident) ViewAlignmentNames),
    MakeInlet("autoresizing-mask", InletTypeEnum, &self->autoresizingMask, (ident) ViewAutoresizingNames),
    MakeInlet("background-color", InletTypeColor, &self->backgroundColor, NULL),
    MakeInlet("border-color", InletTypeColor, &self->borderColor, NULL),
    MakeInlet("border-width", InletTypeInteger, &self->borderWidth, NULL),
    MakeInlet("clips-subviews", InletTypeBool, &self->clipsSubviews, NULL),
    MakeInlet("frame", InletTypeRectangle, &self->frame, NULL),
    MakeInlet("hidden", InletTypeBool, &self->hidden, NULL),
    MakeInlet("height", InletTypeInteger, &self->frame.h, NULL),
    MakeInlet("left", InletTypeInteger, &self->frame.x, NULL),
    MakeInlet("max-height", InletTypeInteger, &self->maxSize.h, NULL),
    MakeInlet("max-size", InletTypeSize, &self->maxSize, NULL),
    MakeInlet("max-width", InletTypeInteger, &self->maxSize.w, NULL),
    MakeInlet("min-height", InletTypeInteger, &self->minSize.h, NULL),
    MakeInlet("min-size", InletTypeSize, &self->minSize, NULL),
    MakeInlet("min-width", InletTypeInteger, &self->minSize.w, NULL),
    MakeInlet("padding", InletTypeRectangle, &self->padding, NULL),
    MakeInlet("padding-top", InletTypeInteger, &self->padding.top, NULL),
    MakeInlet("padding-right", InletTypeInteger, &self->padding.right, NULL),
    MakeInlet("padding-bottom", InletTypeInteger, &self->padding.bottom, NULL),
    MakeInlet("padding-left", InletTypeInteger, &self->padding.left, NULL),
    MakeInlet("top", InletTypeInteger, &self->frame.y, NULL),
    MakeInlet("width", InletTypeInteger, &self->frame.w, NULL)
  );

  $(self, bind, inlets, style->attributes);
}

/**
 * @fn void View::applyTheme(View *self, const Theme *theme)
 * @memberof View
 */
static void applyTheme(View *self, const Theme *theme) {

  assert(theme);

  Style *computedStyle = $(theme, computeStyle, self);
  assert(computedStyle);

  if (!$(self->computedStyle, isComputedEqual, computedStyle)) {

    release(self->computedStyle);
    self->computedStyle = retain(computedStyle);

    $(self->computedStyle, addAttributes, self->style->attributes);
    $(self, applyStyle, self->computedStyle);
  }

  release(computedStyle);
}

/**
 * @fn void View::applyThemeIfNeeded(View *self, const Theme *theme)
 * @memberof View
 */
static void applyThemeIfNeeded(View *self, const Theme *theme) {

  assert(theme);

  $(self, enumerateSubviews, (ViewEnumerator) applyThemeIfNeeded, (ident) theme);

  if (self->needsApplyTheme) {

    $(self, clearWarnings, WarningTypeStyle);

    $(self, applyTheme, theme);

    self->needsApplyTheme = false;
  }
}

/**
 * @fn void View::attachStylesheet(View *self, SDL_Window *window)
 * @memberof View
 */
static void attachStylesheet(View *self, SDL_Window *window) {

  assert(window);

  if (self->stylesheet) {
    Theme *theme = $$(Theme, theme, window);
    if (theme) {
      $(theme, addStylesheet, self->stylesheet);
    }
  }
}

/**
 * @fn void View::awakeWithCharacters(View *self, const char *chars)
 * @memberof View
 */
static void awakeWithCharacters(View *self, const char *chars) {

  Data *data = $$(Data, dataWithConstMemory, (uint8_t *) chars, strlen(chars));

  $(self, awakeWithData, data);

  release(data);
}

/**
 * @fn void View::awakeWithData(View *self, const Data *data)
 * @memberof View
 */
static void awakeWithData(View *self, const Data *data) {

  Dictionary *dictionary = $$(JSONSerialization, objectFromData, data, 0);

  $(self, awakeWithDictionary, dictionary);

  release(dictionary);
}

/**
 * @fn void Viem::awakeWithDictionary(View *self, const Dictionary *dictionary)
 * @memberof View
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  assert(dictionary);

  const Inlet inlets[] = MakeInlets(
    MakeInlet("identifier", InletTypeCharacters, &self->identifier, NULL),
    MakeInlet("classNames", InletTypeClassNames, &self, NULL),
    MakeInlet("style", InletTypeStyle, &self, NULL),
    MakeInlet("subviews", InletTypeSubviews, &self, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @fn void View::awakeWithResource(View *self, const Resource *resource)
 * @memberof View
 */
static void awakeWithResource(View *self, const Resource *resource) {

  assert(resource);

  $(self, awakeWithData, resource->data);
}

/**
 * @fn void View::awakeWithResourceName(View *self, const char *name)
 * @memberof View
 */
static void awakeWithResourceName(View *self, const char *name) {

  Resource *resource = $$(Resource, resourceWithName, name);

  $(self, awakeWithResource, resource);

  release(resource);
}

/**
 * @fn void View::becomeFirstResponder(View *self)
 * @memberof View
 */
static void becomeFirstResponder(View *self) {

  assert(self->window);

  if (!$(self, isFirstResponder)) {

    View *firstResponder = SDL_GetWindowData(self->window, "firstResponder");
    if (firstResponder) {
      $(firstResponder, resignFirstResponder);
    }

    String *path = $(self, path);
    MVC_LogDebug("%s\n", path->chars);
    release(path);

    SDL_SetWindowData(self->window, "firstResponder", self);
  }
}

/**
 * @fn bool View::bind(View *self, const Inlet *inlets, const Dictionary *dictionary)
 * @memberof View
 */
static bool _bind(View *self, const Inlet *inlets, const Dictionary *dictionary) {

  if (inlets) {
    if (bindInlets(inlets, dictionary)) {
      self->needsApplyTheme = true;
      self->needsLayout = true;
      return true;
    }
  }

  return false;
}

/**
 * @fn SDL_Rect View::bounds(const View *self)
 * @memberof View
 */
static SDL_Rect bounds(const View *self) {

  const SDL_Size size = $(self, size);

  const SDL_Rect bounds = {
    .x = self->padding.left,
    .y = self->padding.top,
    .w = max(0, size.w - (self->padding.left + self->padding.right)),
    .h = max(0, size.h - (self->padding.top + self->padding.bottom)),
  };

  return bounds;
}

/**
 * @fn void View::bringSubviewToFront(View *self, View *subview)
 * @memberof View
 */
static void bringSubviewToFront(View *self, View *subview) {

  assert(subview);

  if (subview->superview == self) {

    View *last = $((Array *) self->subviews, lastObject);
    if (last != subview) {
      $(self, addSubviewRelativeTo, subview, last, ViewPositionAfter);
    }
  }
}

/**
 * @brief Filter Predicate for clearWarnings.
 */
static bool clearWarnings_predicate(const ident obj, ident data) {

  const Warning *warning = obj;
  const WarningType type = *(WarningType *) data;

  return (warning->type & type) == 0;
}

/**
 * @fn void View::clearWarnings(const View *self, WarningType type)
 * @memberof View
 */
static void clearWarnings(const View *self, WarningType type) {
  $(self->warnings, filter, clearWarnings_predicate, &type);
}

/**
 * @fn SDL_Rect View::clippingFrame(const View *self)
 * @memberof View
 */
static SDL_Rect clippingFrame(const View *self) {

  SDL_Rect frame = $(self, renderFrame);

  if (self->borderWidth && self->borderColor.a) {
    for (int i = 0; i < self->borderWidth; i++) {
      frame.x -= 1;
      frame.y -= 1;
      frame.w += 2;
      frame.h += 2;
    }
  }

  const View *superview = self->superview;
  while (superview) {
    if (superview->clipsSubviews) {
      const SDL_Rect clippingFrame = $(superview, clippingFrame);
      if (SDL_IntersectRect(&clippingFrame, &frame, &frame) == false) {

        if (MVC_LogEnabled(SDL_LOG_PRIORITY_VERBOSE)) {
          String *desc = $((Object *) self, description);
          String *superdesc = $((Object *) superview, description);

          MVC_LogVerbose("%s is clipped by %s\n", desc->chars, superdesc->chars);

          release(desc);
          release(superdesc);
        }

        frame.w = frame.h = 0;
        break;
      }
    }
    superview = superview->superview;
  }

  return frame;
}

/**
 * @fn bool View::containsPoint(const View *self, const SDL_Point *point)
 * @memberof View
 */
static bool containsPoint(const View *self, const SDL_Point *point) {

  const SDL_Rect frame = $(self, clippingFrame);

  return (bool) SDL_PointInRect(point, &frame);
}

/**
 * @fn int View::depth(const View *self)
 * @memberof View
 */
static int depth(const View *self) {
  return (self->superview ? $(self->superview, depth) + 1 : 0);
}

/**
 * @fn View::descendantWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *descendantWithIdentifier(const View *self, const char *identifier) {

  assert(identifier);

  if (self->identifier) {
    if (strcmp(identifier, self->identifier) == 0) {
      return (View *) self;
    }
  }

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {
    const View *subview = $(subviews, objectAtIndex, i);
    View *descendant = $(subview, descendantWithIdentifier, identifier);
    if (descendant) {
      return descendant;
    }
  }

  return NULL;
}

/**
 * @fn void View::detachStylesheet(View *self, SDL_Window *window)
 * @memberof View
 */
static void detachStylesheet(View *self, SDL_Window *window) {

  assert(window);

  if (self->stylesheet) {
    Theme *theme = $$(Theme, theme, window);
    if (theme) {
      $(theme, removeStylesheet, self->stylesheet);
    }
  }
}

/**
 * @fn void View::didMoveToWindow(View *self, SDL_Window *window)
 * @memberof View
 */
static void didMoveToWindow(View *self, SDL_Window *window) {

  if (window) {
    $(self, attachStylesheet, window);

    if (self->superview == NULL) {
      $(self, sizeToFill);
    }

    self->needsLayout = true;
  }
}

/**
 * @fn bool View::didReceiveEvent(const View *self, const SDL_Event *event)
 * @memberof View
 */
static bool didReceiveEvent(const View *self, const SDL_Event *event) {

  if ($(self, isVisible)) {

    SDL_Point point;

    if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
      point = MakePoint(event->button.x, event->button.y);
    } else if (event->type == SDL_MOUSEMOTION) {
      point = MakePoint(event->motion.x, event->motion.y);
    } else if (event->type == SDL_MOUSEWHEEL) {
      SDL_GetMouseState(&point.x, &point.y);
    } else {
      return false;
    }

    return $(self, containsPoint, &point);
  }

  return false;
}

/**
 * @fn void View::draw(View *self, Renderer *renderer)
 * @memberof View
 */
static void draw(View *self, Renderer *renderer) {

  assert(self->window);

  if (self->hidden == false) {

    $(renderer, drawView, self);

    $(self, enumerateSubviews, (ViewEnumerator) draw, renderer);
  }
}

/**
 * @fn void View::emitViewEvent(View *self, ViewEvent code, ident data)
 * @memberof View
 */
static void emitViewEvent(View *self, ViewEvent code, ident data) {

  // FIXME: There's an edge case where we are released before these events are processed
  // resulting in a segfault. Maybe we should emit our path or identifier so that event
  // listeners can attempt to look the View up and bail out if not found?

  SDL_PushEvent((SDL_Event *) &(const SDL_UserEvent) {
    .type = MVC_VIEW_EVENT,
    .code = code,
    .data1 = self,
    .data2 = data
  });
}

/**
 * @fn void View::enumerate(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerate(View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  enumerator(self, data);

  $(self, enumerateDescendants, enumerator, data);
}

/**
 * @fn void View::enumerateAdjacent(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateAdjacent(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  if (self->superview) {
    const Array *siblings = (Array *) self->superview->subviews;
    const ssize_t index = $(siblings, indexOfObject, (const ident) self);
    if (index > 0) {
      enumerator($(siblings, objectAtIndex, index - 1), data);
    }
    if (index < (ssize_t) (siblings->count - 1)) {
      enumerator($(siblings, objectAtIndex, index + 1), data);
    }
  }
}

/**
 * @fn void View::enumerateAncestors(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateAncestors(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  for (View *view = self->superview; view; view = view->superview) {
    enumerator(view, data);
  }
}

/**
 * @fn void View::enumerateSelection(View *self, const char *rule, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateSelection(View *self, const char *rule, ViewEnumerator enumerator, ident data) {

  Selector *selector = $(alloc(Selector), initWithRule, rule);
  assert(selector);

  $(selector, enumerateSelection, self, enumerator, data);
  release(selector);
}

/**
 * @fn void View::enumerateDescendants(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateDescendants(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {

    View *subview = $(subviews, objectAtIndex, i);
    enumerator(subview, data);

    $(subview, enumerateDescendants, enumerator, data);
  }
}

/**
 * @fn void View::enumerateSubviews(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateSiblings(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  if (self->superview) {

    const Array *siblings = (Array *) self->superview->subviews;
    for (size_t i = 0; i < siblings->count; i++) {
      View *sibling = $(siblings, objectAtIndex, i);
      if (sibling != self) {
        enumerator(sibling, data);
      }
    }
  }
}

/**
 * @fn void View::enumerateSubviews(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateSubviews(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {
    enumerator((View *) subviews->elements[i], data);
  }
}

/**
 * @fn void View::enumerateSuperview(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateSuperview(const View *self, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  if (self->superview) {
    enumerator(self->superview, data);
  }
}

/**
 * @fn void View::enumerateVisible(const View *self, ViewEnumerator enumerator, ident data)
 * @memberof View
 */
static void enumerateVisible(View *self, ViewEnumerator enumerator, ident data) {

  if ($(self, isVisible) == false) {
    return;
  }

  enumerator(self, data);

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {

    View *subview = $(subviews, objectAtIndex, i);
    $(subview, enumerateVisible, enumerator, data);
  }
}

/**
 * @brief Predicate for hasClassName.
 */
static bool hasClassName_predicate(const ident obj, ident data) {
  return strcmp(((String *) obj)->chars, (const char *) data) == 0;
}

/**
 * @fn bool View::hasClassName(const View *self, cosnt char *className)
 * @memberof View
 */
static bool hasClassName(const View *self, const char *className) {

  if (className) {
    return $((Set *) self->classNames, containsObjectMatching, hasClassName_predicate, (ident) className);
  }

  return false;
}

typedef struct {
  SDL_Rect bounds;
  bool hasOverflow;
} Overflow;

/**
 * @brief ViewEnumerator for hasOverflow.
 */
static void hasOverflow_enumerate(View *view, ident data) {

  Overflow *overflow = data;

  if (!view->hidden) {

    const SDL_Rect bounds = $(view, bounds);

    if (bounds.x + bounds.w > overflow->bounds.w || bounds.y + bounds.h > overflow->bounds.h) {

      overflow->hasOverflow = true;

      $(view, warn, WarningTypeLayout, "Exceeds superview bounds [%d %d %d %d]",
        overflow->bounds.x, overflow->bounds.y, overflow->bounds.w, overflow->bounds.h);
    }
  }
}

/**
 * @fn bool View::hasOverflow(const View *self)
 * @memberof View
 */
static bool hasOverflow(const View *self) {

  Overflow overflow = {
    .bounds = $(self, bounds)
  };

  $(self, enumerateSubviews, hasOverflow_enumerate, (ident) &overflow);

  return overflow.hasOverflow;
}

/**
 * @fn View *View::hitTest(const View *self, const SDL_Point *point)
 * @memberof View
 */
static View *hitTest(const View *self, const SDL_Point *point) {

  if (self->hidden == false) {

    if ($(self, containsPoint, point)) {

      const Array *subviews = (Array *) self->subviews;
      for (size_t i = subviews->count; i; i--) {

        const View *subview = $(subviews, objectAtIndex, i - 1);
        const View *view = $(subview, hitTest, point);
        if (view) {
          return (View *) view;
        }
      }

      return (View *) self;
    }
  }

  return NULL;
}

/**
 * @fn View *View::init(View *self)
 * @memberof View
 */
static View *init(View *self) {
  return $(self, initWithFrame, NULL);
}

/**
 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
 * @memberof View
 */
static View *initWithFrame(View *self, const SDL_Rect *frame) {

  self = (View *) super(Object, self, init);
  if (self) {

    if (frame) {
      self->frame = *frame;
    }

    self->classNames = $$(MutableSet, setWithCapacity, 0);
    assert(self->classNames);

    self->computedStyle = $(alloc(Style), initWithAttributes, NULL);
    assert(self->computedStyle);

    self->subviews = $$(MutableArray, arrayWithCapacity, 0);
    assert(self->subviews);

    self->style = $(alloc(Style), initWithAttributes, NULL);
    assert(self->style);

    self->warnings = $$(MutableArray, arrayWithCapacity, 0);
    assert(self->warnings);

    self->maxSize = MakeSize(INT32_MAX, INT32_MAX);

    self->needsApplyTheme = true;
    self->needsLayout = true;
  }

  return self;
}

/**
 * @brief ViewEnumerator for invalidateStyle.
 */
static void invalidateStyle_enumerate(View *view, ident data) {
  view->needsApplyTheme = true;
}

/**
 * @fn void View::invalidateStyle(View *self)
 * @memberof View
 */
static void invalidateStyle(View *self) {
  $(self, enumerate, invalidateStyle_enumerate, NULL);
}

/**
 * @fn bool View::isContainer(const View *self)
 * @memberof View
 */
static bool isContainer(const View *self) {
  return self->autoresizingMask & (ViewAutoresizingFit | ViewAutoresizingContain);
}

/**
 * @fn bool View::isDescendantOfView(const View *self, const View *view)
 * @memberof View
 */
static bool isDescendantOfView(const View *self, const View *view) {

  assert(view);

  while (self) {
    if (self == view) {
      return true;
    }
    self = self->superview;
  }

  return false;
}

/**
 * @fn bool View::isFirstResponder(const View *self)
 * @memberof View
 */
static bool isFirstResponder(const View *self) {

  if (self->window) {
    return SDL_GetWindowData(self->window, "firstResponder") == self;
  } else {
    return false;
  }
}

/**
 * @fn bool View::isVisible(const View *self)
 * @memberof View
 */
static bool isVisible(const View *self) {

  for (const View *view = self; view; view = view->superview) {
    if (view->hidden) {
      return false;
    }
  }

  return true;
}

/**
 * @brief ViewEnumerator for layoutIfNeeded recursion.
 */
static void layoutIfNeeded_recurse(View *subview, ident data) {
  $(subview, layoutIfNeeded);
}

/**
 * @fn void View::layoutIfNeeded(View *self)
 * @memberof View
 */
static void layoutIfNeeded(View *self) {

  $(self, enumerateSubviews, layoutIfNeeded_recurse, NULL);

  if (self->needsLayout) {

    $(self, clearWarnings, WarningTypeLayout);

    $(self, layoutSubviews);

    self->needsLayout = false;
  }
}

/**
 * @fn View::layoutSubviews(View *self)
 * @memberof View
 */
static void layoutSubviews(View *self) {

  if (self->autoresizingMask & ViewAutoresizingContain) {
    $(self, sizeToContain);
  } else if (self->autoresizingMask & ViewAutoresizingFit) {
    $(self, sizeToFit);
  }

  const SDL_Rect bounds = $(self, bounds);

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {

    View *subview = $(subviews, objectAtIndex, i);

    SDL_Size subviewSize = $(subview, size);

    if (subview->autoresizingMask & ViewAutoresizingWidth) {
      subviewSize.w = bounds.w;
    }

    if (subview->autoresizingMask & ViewAutoresizingHeight) {
      subviewSize.h = bounds.h;
    }

    $(subview, resize, &subviewSize);
    $(subview, layoutIfNeeded);

    switch (subview->alignment & ViewAlignmentMaskHorizontal) {
      case ViewAlignmentLeft:
        subview->frame.x = 0;
        break;
      case ViewAlignmentCenter:
        subview->frame.x = (bounds.w - subview->frame.w) * 0.5f;
        break;
      case ViewAlignmentRight:
        subview->frame.x = bounds.w - subview->frame.w;
        break;
    }

    switch (subview->alignment & ViewAlignmentMaskVertical) {
      case ViewAlignmentMaskTop:
        subview->frame.y = 0;
        break;
      case ViewAlignmentMaskMiddle:
        subview->frame.y = (bounds.h - subview->frame.h) * 0.5f;
        break;
      case ViewAlignmentMaskBottom:
        subview->frame.y = bounds.h - subview->frame.h;
        break;
    }
  }
}

/**
 * @fn bool View::matchesSelector(const View *self, const SimpleSelector *simpleSelector)
 * @memberof View
 */
static bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

  assert(simpleSelector);

  const char *pattern = simpleSelector->pattern;

  switch (simpleSelector->type) {
    case SimpleSelectorTypeNone:
      return false;

    case SimpleSelectorTypeUniversal:
      return true;

    case SimpleSelectorTypeType: {
      const Class *clazz = classForName(pattern);
      if (clazz) {
        return $((Object *) self, isKindOfClass, clazz);
      }
    }
      break;

    case SimpleSelectorTypeClass:
      return $(self, hasClassName, pattern);

    case SimpleSelectorTypeId:
      if (self->identifier) {
        return strcmp(self->identifier, pattern) == 0;
      } else {
        return false;
      }

    case SimpleSelectorTypePseudo:
      if (strcmp("first-child", pattern) == 0) {
        if (self->superview) {
          return $((Array *) self->superview->subviews, firstObject) == self;
        }
      } else if (strcmp("last-child", pattern) == 0) {
        if (self->superview) {
          return $((Array *) self->superview->subviews, lastObject) == self;
        }
      } else if (strcmp("nth-child(even)", pattern) == 0) {
        if (self->superview) {
          return ($((Array *) self->superview->subviews, indexOfObject, (ident) self) & 1) == 0;
        }
      }  else if (strcmp("nth-child(odd)", pattern) == 0) {
        if (self->superview) {
          return $((Array *) self->superview->subviews, indexOfObject, (ident) self) & 1;
        }
      } else if (strcmp("hover", pattern) == 0) {
        SDL_Point point;
        SDL_GetMouseState(&point.x, &point.y);
        return $(self, containsPoint, &point);
      }
      break;
  }

  return false;
}

/**
 * @brief ViewEnumerator for moveToWindow recursion.
 */
static void moveToWindow_recurse(View *subview, ident data) {
  $(subview, moveToWindow, data);
}

/**
 * @fn void View::moveToWindow(View *self, SDL_Window *window)
 * @memberof View
 */
static void moveToWindow(View *self, SDL_Window *window) {

  if (self->window != window) {

    $(self, willMoveToWindow, window);

    self->window = window;

    $(self, didMoveToWindow, window);

    $(self, enumerateSubviews, moveToWindow_recurse, window);
  }
}

/**
 * @fn String *View::path(const View *self)
 * @memberof View
 */
static String *path(const View *self) {

  MutableArray *parts = $$(MutableArray, array);

  const View *view = self;
  while (view) {

    Array *classNames = $((Set *) view->classNames, allObjects);

    String *part;
    if (view->identifier) {
      part = str("#%s", view->identifier);
    } else if (classNames->count) {
      part = str(".%s", ((String *) $(classNames, firstObject))->chars);
    } else {
      part = str("%s", classnameof(view));
    }

    release(classNames);

    $(parts, insertObjectAtIndex, part, 0);
    release(part);

    if (view->viewController) {
      break;
    }

    view = view->superview;
  }

  String *path = $((Array *) parts, componentsJoinedByCharacters, " > ");

  release(parts);

  return path;
}

/**
 * @fn void View::removeAllClassNames(View *self)
 * @memberof View
 */
static void removeAllClassNames(View *self) {

  $(self->classNames, removeAllObjects);

  $(self, invalidateStyle);
}

/**
 * @brief ArrayEnumerator for removeAllSubviews.
 */
static void removeAllSubviews_enumerate(const Array *array, ident obj, ident data) {
  $((View *) data, removeSubview, obj);
}

/**
 * @fn void View::removeAllSubviews(View *self)
 * @memberof View
 */
static void removeAllSubviews(View *self) {
  $(self->subviews, removeAllObjectsWithEnumerator, removeAllSubviews_enumerate, self);
}

/**
 * @fn void View::removeClassName(View *self, const char *className)
 * @memberof View
 */
static void removeClassName(View *self, const char *className) {

  if (className) {
    String *string = $$(String, stringWithCharacters, className);
    assert(string);

    $(self->classNames, removeObject, string);
    release(string);

    $(self, invalidateStyle);
  }
}

/**
 * @fn void View::removeFromSuperview(View *self)
 * @memberof View
 */
static void removeFromSuperview(View *self) {

  if (self->superview) {
    $(self->superview, removeSubview, self);
  }
}

/**
 * @fn void View::removeSubview(View *self, View *subview)
 * @memberof View
 */
static void removeSubview(View *self, View *subview) {

  assert(subview);

  if (subview->superview == self) {
    
    subview->superview = NULL;

    $(subview, moveToWindow, NULL);

    $(self->subviews, removeObject, subview);

    self->needsLayout = true;
  }
}

/**
 * @fn void View::render(View *self, Renderer *renderer)
 * @memberof View
 */
static void render(View *self, Renderer *renderer) {

  assert(self->window);

  if ($(self, hasClassName, "breakpoint")) {
    SDL_TriggerBreakpoint();
  }
  
  if (self->backgroundColor.a) {

    $(renderer, setDrawColor, &self->backgroundColor);

    const SDL_Rect frame = $(self, renderFrame);
    $(renderer, drawRectFilled, &frame);
  }

  if (self->borderWidth && self->borderColor.a) {

    $(renderer, setDrawColor, &self->borderColor);

    SDL_Rect frame = $(self, renderFrame);
    for (int i = 0; i < self->borderWidth; i++) {

      $(renderer, drawRect, &frame);

      frame.x -= 1;
      frame.y -= 1;
      frame.w += 2;
      frame.h += 2;
    }
  }

  $(renderer, setDrawColor, &Colors.White);
}

/**
 * @brief ViewEnumerator for renderDeviceDidReset recursion.
 */
static void renderDeviceDidReset_recurse(View *subview, ident data) {
  $(subview, renderDeviceDidReset);
}

/**
 * @fn void View::renderDeviceDidReset(View *self)
 * @memberof View
 */
static void renderDeviceDidReset(View *self) {
  $(self, enumerateSubviews, renderDeviceDidReset_recurse, NULL);
}

/**
 * @brief ViewEnumerator for renderDeviceWillReset recursion.
 */
static void renderDeviceWillReset_recurse(View *subview, ident data) {
  $(subview, renderDeviceWillReset);
}

/**
 * @fn void View::renderDeviceWillReset(View *self)
 * @memberof View
 */
static void renderDeviceWillReset(View *self) {
  $(self, enumerateSubviews, renderDeviceWillReset_recurse, NULL);
}

/**
 * @fn SDL_Rect View::renderFrame(const View *self)
 * @memberof View
 */
static SDL_Rect renderFrame(const View *self) {

  SDL_Rect frame = self->frame;

  const View *view = self;
  const View *superview = view->superview;
  while (superview) {

    frame.x += superview->frame.x;
    frame.y += superview->frame.y;

    if (view->alignment != ViewAlignmentInternal) {
      frame.x += superview->padding.left;
      frame.y += superview->padding.top;
    }

    view = superview;
    superview = view->superview;
  }

  return frame;
}

/**
 * @fn void View::replaceSubview(View *self, View *subview, View *replacement)
 * @memberof View
 */
static void replaceSubview(View *self, View *subview, View *replacement) {

  assert(subview);
  assert(replacement);

  $(self, addSubviewRelativeTo, replacement, subview, ViewPositionAfter);
  $(self, removeSubview, subview);
}

/**
 * @fn void View::resignFirstResponder(View *self)
 * @memberof View
 */
static void resignFirstResponder(View *self) {

  if (self->window && $(self, isFirstResponder)) {

    String *path = $(self, path);
    MVC_LogDebug("%s\n", path->chars);
    release(path);

    SDL_SetWindowData(self->window, "firstResponder", NULL);
  }
}

/**
 * @fn void View::resize(View *self, const SDL_Size *size)
 * @memberof View
 */
static void resize(View *self, const SDL_Size *size) {

  const int w = clamp(size->w, self->minSize.w, self->maxSize.w);
  const int h = clamp(size->h, self->minSize.h, self->maxSize.h);

  if (self->frame.w != w || self->frame.h != h) {

    self->frame.w = w;
    self->frame.h = h;

    self->needsLayout = true;

    if (self->superview && $(self->superview, isContainer)) {
      self->superview->needsLayout = true;
    }
  }
}

/**
 * @fn void View::resolve(View *self, Outlet *outlets)
 * @memberof View
 */
static void resolve(View *self, Outlet *outlets) {

  if (outlets) {
    for (Outlet *outlet = outlets; outlet->identifier; outlet++) {
      *outlet->view = $(self, descendantWithIdentifier, outlet->identifier);
      if (*outlet->view == NULL) {
        String *desc = $((Object *) self, description);
        MVC_LogError("Failed to resolve outlet '%s' for %s", outlet->identifier, desc->chars);
        release(desc);
      }

      assert(*outlet->view);
    }
  }
}

/**
 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
 * @memberof View
 */
static void respondToEvent(View *self, const SDL_Event *event) {

  assert(event);

  ViewEvent code = ViewEventNone;

  switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
      code = ViewEventMouseButtonDown;
      break;
    case SDL_MOUSEBUTTONUP:
      code = ViewEventMouseButtonUp;
      break;
    case SDL_KEYDOWN:
      code = ViewEventKeyDown;
      break;
    case SDL_KEYUP:
      code = ViewEventKeyUp;
      break;
    default:
      break;
  }

  if (code != ViewEventNone) {
    $(self, emitViewEvent, code, NULL);

    if (code == ViewEventMouseButtonUp) {
      if (event->button.clicks) {
        $(self, emitViewEvent, ViewEventClick, NULL);
      }
    }
  }

  if (event->type == SDL_MOUSEBUTTONDOWN) {
    if ($(self, acceptsFirstResponder)) {
      $(self, becomeFirstResponder);
      return;
    }
  }

  if (self->nextResponder) {
    $(self->nextResponder, respondToEvent, event);
  } else if (self->superview) {
    $(self->superview, respondToEvent, event);
  }
}

/**
 * @fn Set *View::select(View *self, const char *rule)
 * @memberof View
 */
static Set *_select(View *self, const char *rule) {

  Selector *selector = $(alloc(Selector), initWithRule, rule);
  assert(selector);

  Set *selection = $(selector, select, self);

  release(selector);
  return selection;
}

/**
 * @fn View *View::selectFirst(View *self, const char *rule)
 * @memberof View
 */
static View *selectFirst(View *self, const char *rule) {

  Selector *selector = $(alloc(Selector), initWithRule, rule);
  assert(selector);

  View *first = $(selector, selectFirst, self);

  release(selector);
  return first;
}

/**
 * @fn SDL_Size View::size(const View *self)
 * @memberof View
 */
static SDL_Size size(const View *self) {
  return MakeSize(self->frame.w, self->frame.h);
}

/**
 * @fn SDL_Size View::sizeThatContains(const View *self)
 * @memberof View
 */
static SDL_Size sizeThatContains(const View *self) {

  const SDL_Size size = $(self, size);
  const SDL_Size sizeThatFits = $(self, sizeThatFits);

  return MakeSize(max(size.w, sizeThatFits.w), max(size.h, sizeThatFits.h));
}

/**
 * @fn SDL_Size View::sizeThastFills(const View *self)
 * @memberof View
 */
static SDL_Size sizeThatFills(const View *self) {
  SDL_Size size;

  if (self->superview == NULL) {
    SDL_GetWindowSize(self->window, &size.w, &size.h);
  } else {
    size = $(self->superview, size);
  }

  return size;
}

/**
 * @fn void View::sizeThatFits(const View *self)
 * @memberof View
 */
static SDL_Size sizeThatFits(const View *self) {

  SDL_Size size = $(self, size);

  if (self->autoresizingMask & ViewAutoresizingWidth) {
    size.w = 0;
  }

  if (self->autoresizingMask & ViewAutoresizingHeight) {
    size.h = 0;
  }

  if ($(self, isContainer)) {
    size = MakeSize(0, 0);

    Array *subviews = $(self, visibleSubviews);
    for (size_t i = 0; i < subviews->count; i++) {

      const View *subview = $(subviews, objectAtIndex, i);

      SDL_Size subviewSize;
      if (subview->autoresizingMask & ViewAutoresizingContain) {
        subviewSize = $(subview, sizeThatContains);
      } else if (subview->autoresizingMask & ViewAutoresizingFit) {
        subviewSize = $(subview, sizeThatFits);
      } else {
        subviewSize = $(subview, size);
      }

      SDL_Point subviewOrigin = MakePoint(0, 0);
      switch (subview->alignment) {
        case ViewAlignmentNone:
          subviewOrigin = MakePoint(subview->frame.x, subview->frame.y);
          break;
        default:
          break;
      }

      size.w = max(size.w, subviewOrigin.x + subviewSize.w);
      size.h = max(size.h, subviewOrigin.y + subviewSize.h);
    }

    size.w += self->padding.left + self->padding.right;
    size.h += self->padding.top + self->padding.bottom;

    release(subviews);
  }

  size.w = clamp(size.w, self->minSize.w, self->maxSize.w);
  size.h = clamp(size.h, self->minSize.h, self->maxSize.h);

  return size;
}

/**
 * @fn void View::sizeToContain(View *self)
 * @memberof View
 */
static void sizeToContain(View *self) {

  const SDL_Size size = $(self, sizeThatContains);

  $(self, resize, &size);
}

/**
 * @fn void View::sizeToFill(View *self)
 * @memberof View
 */
static void sizeToFill(View *self) {

  const SDL_Size size = $(self, sizeThatFills);

  $(self, resize, &size);
}

/**
 * @fn void View::sizeToFit(View *self)
 * @memberof View
 */
static void sizeToFit(View *self) {

  const SDL_Size size = $(self, sizeThatFits);

  $(self, resize, &size);
}

/**
 * @fn View::subviewWithIdentifier(const View *self, const char *identifier)
 * @memberof View
 */
static View *subviewWithIdentifier(const View *self, const char *identifier) {

  assert(identifier);

  const Array *subviews = (Array *) self->subviews;
  for (size_t i = 0; i < subviews->count; i++) {

    View *subview = $(subviews, objectAtIndex, i);
    if (subview->identifier) {

      if (strcmp(identifier, subview->identifier) == 0) {
        return subview;
      }
    }
  }

  return NULL;
}

/**
 * @brief ViewEnumerator for updateBindings recursion.
 */
static void updateBindings_recurse(View *subview, ident data) {
  $(subview, updateBindings);
}

/**
 * @fn void View::updateBindings(View *self)
 * @memberof View
 */
static void updateBindings(View *self) {
  $(self, enumerateSubviews, updateBindings_recurse, NULL);
}

/**
 * @fn SDL_Rect View::viewport(const View *self)
 * @memberof View
 */
static SDL_Rect viewport(const View *self) {

  assert(self->window);

  const SDL_Rect frame =  $(self, renderFrame);

  return MVC_TransformToWindow(self->window, &frame);
}

/**
 * @fn View *View::viewWithCharacters(const char *chars, Outlet *outlets)
 * @memberof View
 */
static View *viewWithCharacters(const char *chars, Outlet *outlets) {

  Data *data = $$(Data, dataWithConstMemory, (ident) chars, strlen(chars));

  View *view = $$(View, viewWithData, data, outlets);

  release(data);

  return view;
}

/**
 * @fn View *View::viewWithData(const Data *data, Outlet *outlets)
 * @memberof View
 */
static View *viewWithData(const Data *data, Outlet *outlets) {

  Dictionary *dictionary = $$(JSONSerialization, objectFromData, data, 0);

  View *view = $$(View, viewWithDictionary, dictionary, outlets);

  release(dictionary);

  return view;
}

/**
 * @fn View *View::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
 * @memberof View
 */
static View *viewWithDictionary(const Dictionary *dictionary, Outlet *outlets) {

  View *view = NULL;

  BindInlet(&MakeInlet(NULL, InletTypeView, &view, NULL), dictionary);

  $(view, resolve, outlets);

  return view;
}

/**
 * @fn View *View::viewWithResource(const Resource *resource, Outlet *outlets)
 * @memberof View
 */
static View *viewWithResource(const Resource *resource, Outlet *outlets) {

  assert(resource);

  return $$(View, viewWithData, resource->data, outlets);
}

/**
 * @fn View *View::viewWithResourceName(const char *name, Outlet *outlets)
 * @memberof View
 */
static View *viewWithResourceName(const char *name, Outlet *outlets) {

  Resource *resource = $$(Resource, resourceWithName, name);

  View *view = $$(View, viewWithResource, resource, outlets);

  release(resource);

  return view;
}

/**
 * @brief Predicate for visibleSubviews.
 */
static bool visibleSubviews_filter(ident obj, ident data) {

  const View *view = (View *) obj;

  return view->hidden == false && view->alignment != ViewAlignmentInternal;
}

/**
 * @fn Array *View::visibleSubviews(const View *self)
 * @memberof View
 */
static Array *visibleSubviews(const View *self) {
  return $((Array *) self->subviews, filteredArray, visibleSubviews_filter, NULL);
}

/**
 * @fn void View::warn(View *self, ViewWarningType type, const char *fmt, ...)
 * @memberof View
 */
static void warn(View *self, WarningType type, const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);

  Warning *warning = $(alloc(Warning), initWithVaList, type, fmt, args);

  va_end(args);

  String *description = $((Object *) self, description);
  MVC_LogWarn("%s:: %s\n", description->chars, warning->message->chars);
  release(description);

  $(self->warnings, addObject, warning);

  release(warning);
}

/**
 * @fn void View::willMoveToWindow(View *self, SDL_Window *window)
 * @memberof View
 */
static void willMoveToWindow(View *self, SDL_Window *window) {

  if (self->window) {
    $(self, resignFirstResponder);
    $(self, detachStylesheet, self->window);
  }
}

#pragma mark - View class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;

  ((ViewInterface *) clazz->interface)->acceptsFirstResponder = acceptsFirstResponder;
  ((ViewInterface *) clazz->interface)->addClassName = addClassName;
  ((ViewInterface *) clazz->interface)->addSubview = addSubview;
  ((ViewInterface *) clazz->interface)->addSubviewRelativeTo = addSubviewRelativeTo;
  ((ViewInterface *) clazz->interface)->ancestorWithIdentifier = ancestorWithIdentifier;
  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->applyTheme = applyTheme;
  ((ViewInterface *) clazz->interface)->applyThemeIfNeeded = applyThemeIfNeeded;
  ((ViewInterface *) clazz->interface)->attachStylesheet = attachStylesheet;
  ((ViewInterface *) clazz->interface)->awakeWithCharacters = awakeWithCharacters;
  ((ViewInterface *) clazz->interface)->awakeWithData = awakeWithData;
  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->awakeWithResource = awakeWithResource;
  ((ViewInterface *) clazz->interface)->awakeWithResourceName = awakeWithResourceName;
  ((ViewInterface *) clazz->interface)->becomeFirstResponder = becomeFirstResponder;
  ((ViewInterface *) clazz->interface)->bind = _bind;
  ((ViewInterface *) clazz->interface)->bounds = bounds;
  ((ViewInterface *) clazz->interface)->bringSubviewToFront = bringSubviewToFront;
  ((ViewInterface *) clazz->interface)->clearWarnings = clearWarnings;
  ((ViewInterface *) clazz->interface)->clippingFrame = clippingFrame;
  ((ViewInterface *) clazz->interface)->containsPoint = containsPoint;
  ((ViewInterface *) clazz->interface)->depth = depth;
  ((ViewInterface *) clazz->interface)->descendantWithIdentifier = descendantWithIdentifier;
  ((ViewInterface *) clazz->interface)->detachStylesheet = detachStylesheet;
  ((ViewInterface *) clazz->interface)->didMoveToWindow = didMoveToWindow;
  ((ViewInterface *) clazz->interface)->didReceiveEvent = didReceiveEvent;
  ((ViewInterface *) clazz->interface)->draw = draw;
  ((ViewInterface *) clazz->interface)->emitViewEvent = emitViewEvent;
  ((ViewInterface *) clazz->interface)->enumerate = enumerate;
  ((ViewInterface *) clazz->interface)->enumerateAdjacent = enumerateAdjacent;
  ((ViewInterface *) clazz->interface)->enumerateAncestors = enumerateAncestors;
  ((ViewInterface *) clazz->interface)->enumerateDescendants = enumerateDescendants;
  ((ViewInterface *) clazz->interface)->enumerateSelection = enumerateSelection;
  ((ViewInterface *) clazz->interface)->enumerateSiblings = enumerateSiblings;
  ((ViewInterface *) clazz->interface)->enumerateSubviews = enumerateSubviews;
  ((ViewInterface *) clazz->interface)->enumerateSuperview = enumerateSuperview;
  ((ViewInterface *) clazz->interface)->enumerateVisible = enumerateVisible;
  ((ViewInterface *) clazz->interface)->hasClassName = hasClassName;
  ((ViewInterface *) clazz->interface)->hasOverflow = hasOverflow;
  ((ViewInterface *) clazz->interface)->hitTest = hitTest;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((ViewInterface *) clazz->interface)->invalidateStyle = invalidateStyle;
  ((ViewInterface *) clazz->interface)->isContainer = isContainer;
  ((ViewInterface *) clazz->interface)->isDescendantOfView = isDescendantOfView;
  ((ViewInterface *) clazz->interface)->isFirstResponder = isFirstResponder;
  ((ViewInterface *) clazz->interface)->isVisible = isVisible;
  ((ViewInterface *) clazz->interface)->layoutIfNeeded = layoutIfNeeded;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;
  ((ViewInterface *) clazz->interface)->moveToWindow = moveToWindow;
  ((ViewInterface *) clazz->interface)->path = path;
  ((ViewInterface *) clazz->interface)->removeAllClassNames = removeAllClassNames;
  ((ViewInterface *) clazz->interface)->removeAllSubviews = removeAllSubviews;
  ((ViewInterface *) clazz->interface)->removeClassName = removeClassName;
  ((ViewInterface *) clazz->interface)->removeFromSuperview = removeFromSuperview;
  ((ViewInterface *) clazz->interface)->removeSubview = removeSubview;
  ((ViewInterface *) clazz->interface)->render = render;
  ((ViewInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((ViewInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((ViewInterface *) clazz->interface)->renderFrame = renderFrame;
  ((ViewInterface *) clazz->interface)->replaceSubview = replaceSubview;
  ((ViewInterface *) clazz->interface)->resignFirstResponder = resignFirstResponder;
  ((ViewInterface *) clazz->interface)->resize = resize;
  ((ViewInterface *) clazz->interface)->resolve = resolve;
  ((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;
  ((ViewInterface *) clazz->interface)->select = _select;
  ((ViewInterface *) clazz->interface)->selectFirst = selectFirst;
  ((ViewInterface *) clazz->interface)->size = size;
  ((ViewInterface *) clazz->interface)->sizeThatContains = sizeThatContains;
  ((ViewInterface *) clazz->interface)->sizeThatFills = sizeThatFills;
  ((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
  ((ViewInterface *) clazz->interface)->sizeToContain = sizeToContain;
  ((ViewInterface *) clazz->interface)->sizeToFill = sizeToFill;
  ((ViewInterface *) clazz->interface)->sizeToFit = sizeToFit;
  ((ViewInterface *) clazz->interface)->subviewWithIdentifier = subviewWithIdentifier;
  ((ViewInterface *) clazz->interface)->updateBindings = updateBindings;
  ((ViewInterface *) clazz->interface)->viewport = viewport;
  ((ViewInterface *) clazz->interface)->viewWithCharacters = viewWithCharacters;
  ((ViewInterface *) clazz->interface)->viewWithData = viewWithData;
  ((ViewInterface *) clazz->interface)->viewWithDictionary = viewWithDictionary;
  ((ViewInterface *) clazz->interface)->viewWithResource = viewWithResource;
  ((ViewInterface *) clazz->interface)->viewWithResourceName = viewWithResourceName;
  ((ViewInterface *) clazz->interface)->visibleSubviews = visibleSubviews;
  ((ViewInterface *) clazz->interface)->warn = warn;
  ((ViewInterface *) clazz->interface)->willMoveToWindow = willMoveToWindow;

  MVC_NOTIFICATION_EVENT = SDL_RegisterEvents(1);
  assert(MVC_NOTIFICATION_EVENT != (Uint32) -1);
  
  MVC_VIEW_EVENT = SDL_RegisterEvents(1);
  assert(MVC_NOTIFICATION_EVENT != (Uint32) -1);
}

/**
 * @fn Class *View::_View(void)
 * @memberof View
 */
Class *_View(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "View",
      .superclass = _Object(),
      .instanceSize = sizeof(View),
      .interfaceOffset = offsetof(View, interface),
      .interfaceSize = sizeof(ViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
