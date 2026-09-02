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

#include "StackView.h"

const EnumName StackViewAxisNames[] = MakeEnumNames(
  MakeEnumAlias(StackViewAxisVertical, vertical),
  MakeEnumAlias(StackViewAxisHorizontal, horizontal)
);

const EnumName StackViewDistributionNames[] = MakeEnumNames(
  MakeEnumAlias(StackViewDistributionDefault, default),
  MakeEnumAlias(StackViewDistributionFill, fill),
  MakeEnumAlias(StackViewDistributionFillEqually, fill-equally)
);

#define _Class _StackView

#pragma mark - View

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  StackView *this = (StackView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("axis", InletTypeEnum, &this->axis, (ident) StackViewAxisNames),
    MakeInlet("distribution", InletTypeEnum, &this->distribution, (ident) StackViewDistributionNames),
    MakeInlet("spacing", InletTypeInteger, &this->spacing, NULL)
  );

  $(self, bind, inlets, (Dictionary *) style->attributes);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((StackView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  Array *subviews = $(self, visibleSubviews);
  if (subviews->count) {

    const SDL_Rect bounds = $(self, bounds);

    StackView *this = (StackView *) self;

    int availableSize, requestedSize = 0;
    switch (this->axis) {
      case StackViewAxisVertical:
        availableSize = bounds.h;
        break;
      case StackViewAxisHorizontal:
        availableSize = bounds.w;
        break;
    }

    availableSize -= this->spacing * (subviews->count - 1);

    const ViewConstraint unspecified = MakeConstraint(ViewConstraintUnspecified, 0);

    SDL_Size sizes[subviews->count];

    for (size_t i = 0; i < subviews->count; i++) {

      View *subview = $(subviews, objectAtIndex, i);

      sizes[i] = $(subview, sizeThatSatisfies, unspecified, unspecified);

      switch (this->axis) {
        case StackViewAxisVertical:
          requestedSize += sizes[i].h;
          break;
        case StackViewAxisHorizontal:
          requestedSize += sizes[i].w;
          break;
      }
    }

    int pos = 0;

    const float scale = requestedSize ? availableSize / (float) requestedSize : 1.f;

    for (size_t i = 0; i < subviews->count; i++) {

      View *subview = $(subviews, objectAtIndex, i);

      switch (this->axis) {
        case StackViewAxisVertical:
          subview->frame.y = pos;
          break;
        case StackViewAxisHorizontal:
          subview->frame.x = pos;
          break;
      }

      SDL_Size subviewSize = sizes[i];

      switch (this->axis) {
        case StackViewAxisVertical:
          if (subview->autoresizingMask & ViewAutoresizingWidth) {
            subviewSize.w = bounds.w;
          }
          break;
        case StackViewAxisHorizontal:
          if (subview->autoresizingMask & ViewAutoresizingHeight) {
            subviewSize.h = bounds.h;
          }
          break;
      }

      // Distribute along primary axis
      
      switch (this->distribution) {
        case StackViewDistributionDefault:
          break;

        case StackViewDistributionFill:
          switch (this->axis) {
            case StackViewAxisVertical:
              subviewSize.h *= scale;
              break;
            case StackViewAxisHorizontal:
              subviewSize.w *= scale;
              break;
          }
          break;

        case StackViewDistributionFillEqually:
          switch (this->axis) {
            case StackViewAxisVertical:
              subviewSize.h = max(availableSize / (float) subviews->count, sizes[i].h);
              break;
            case StackViewAxisHorizontal:
              subviewSize.w = max(availableSize / (float) subviews->count, sizes[i].w);
              break;
          }
          break;
      }

      $(subview, layoutWithSize, &subviewSize);

      // Align along secondary axis
      
      switch (this->axis) {
        case StackViewAxisVertical:
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
          pos += subviewSize.h;
          break;
        case StackViewAxisHorizontal:
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
          pos += subviewSize.w;
          break;
      }

      pos += this->spacing;
    }
  }

  release(subviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

  const StackView *this = (StackView *) self;

  SDL_Size size = MakeSize(0, 0);

  switch (this->axis) {
    case StackViewAxisVertical:
      size.h = self->padding.top + self->padding.bottom;
      break;
    case StackViewAxisHorizontal:
      size.w = self->padding.left + self->padding.right;
      break;
  }

  Array *subviews = $(self, visibleSubviews);

  const ViewConstraint unspecified = MakeConstraint(ViewConstraintUnspecified, 0);

  for (size_t i = 0; i < subviews->count; i++) {

    View *subview = $(subviews, objectAtIndex, i);

    const SDL_Size subviewSize = $(subview, sizeThatSatisfies, unspecified, unspecified);

    switch (this->axis) {
      case StackViewAxisVertical:
        size.w = max(size.w, subviewSize.w);
        size.h += subviewSize.h;
        break;
      case StackViewAxisHorizontal:
        size.w += subviewSize.w;
        size.h = max(size.h, subviewSize.h);
        break;
    }
  }

  switch (this->axis) {
    case StackViewAxisVertical:
      size.w += self->padding.left + self->padding.right;
      break;
    case StackViewAxisHorizontal:
      size.h += self->padding.top + self->padding.bottom;
      break;
  }

  if (subviews->count) {
    switch (this->axis) {
      case StackViewAxisVertical:
        size.h += this->spacing * (subviews->count - 1);
        break;
      case StackViewAxisHorizontal:
        size.w += this->spacing * (subviews->count - 1);
        break;
    }
  }

  release(subviews);

  size.w = clamp(size.w, self->minSize.w, self->maxSize.w);
  size.h = clamp(size.h, self->minSize.h, self->maxSize.h);

  return size;
}

#pragma mark - StackView

/**
 * @fn StackView *StackView::initWithFrame(StackView *self, const SDL_Rect *frame)
 * @memberof StackView
 */
static StackView *initWithFrame(StackView *self, const SDL_Rect *frame) {

  self = (StackView *) super(View, self, initWithFrame, frame);
  if (self) {
    self->view.autoresizingMask = ViewAutoresizingContain;
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->init = init;

  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

  ((StackViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

/**
 * @fn Class *StackView::_StackView(void)
 * @memberof StackView
 */
Class *_StackView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "StackView",
      .superclass = _View(),
      .instanceSize = sizeof(StackView),
      .interfaceSize = sizeof(StackViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
