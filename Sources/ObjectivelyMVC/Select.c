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

#include <Objectively/Array.h>

#include "Select.h"

#define _Class _Select

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Select *this = (Select *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  release(this->options);
  release(this->stackView);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((Select *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  Select *this = (Select *) self;

  Control *control = (Control *) self;

  if (control->selection == ControlSelectionSingle) {

    const Array *options = (Array *) this->options;
    for (size_t i = 0; i < options->count; i++) {

      Option *option = $(options, objectAtIndex, i);
      if (option->isSelected) {
        option->view.hidden = false;
      } else if ($(control, isHighlighted)) {
        option->view.hidden = false;
      } else {
        option->view.hidden = true;
      }
    }

    $((View *) this->stackView, sizeToFit);
    $((View *) this->stackView, layoutIfNeeded);
  }

  super(View, self, layoutSubviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

  const Select *this = (Select *) self;

  SDL_Size size = super(View, self, sizeThatFits);

  const Array *options = (Array *) this->options;
  for (size_t i = 0; i < options->count; i++) {

    const View *option = $(options, objectAtIndex, i);
    const SDL_Size optionSize = $(option, sizeThatFits);

    size.w = max(size.w, optionSize.w + self->padding.left + self->padding.right);
    size.h = max(size.h, optionSize.h + self->padding.top + self->padding.bottom);
  }

  return size;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  Select *this = (Select *) self;

  const Array *options = (Array *) this->options;

  if (event->type == SDL_MOUSEBUTTONUP) {
    if (self->state & ControlStateHighlighted) {
      self->state &= ~ControlStateHighlighted;

      for (size_t i = 0; i < options->count; i++) {

        Option *option = $(options, objectAtIndex, i);
        if ($((View *) option, didReceiveEvent, event)) {
          $(this, selectOption, option);
          if (this->delegate.didSelectOption) {
            this->delegate.didSelectOption(this, option);
          }
          return true;
        }
      }
    } else {
      self->state |= ControlStateHighlighted;
      return true;
    }
  }

  return super(Control, self, captureEvent, event);
}

/**
 * @see Control::stateDidChange(Control *)
 */
static void stateDidChange(Control *self) {

  Select *this = (Select *) self;

  if (self->selection == ControlSelectionSingle) {

    View *stackView = (View *) this->stackView;

    if ($(self, isHighlighted)) {

      const SDL_Rect renderFrame = $((View *) self, renderFrame);

      stackView->frame.x = renderFrame.x + self->view.padding.left;
      stackView->frame.y = renderFrame.y + self->view.padding.top;

      View *view = (View *) self;

      while (view->superview) {
        view = view->superview;
      }

      $(stackView, addClassName, "options");
      $(view, addSubview, stackView);

      stackView->nextResponder = (View *) self;

    } else {

      $(stackView, removeClassName, "options");
      $((View *) self, addSubview, stackView);

      stackView->nextResponder = NULL;
    }

    $(stackView, sizeToFit);
  }

  super(Control, self, stateDidChange);
}

#pragma mark - Select

/**
 * @brief ArrayEnumerator to remove Options from the stackView.
 */
static void addOption_removeSubview(const Array *array, ident obj, ident data) {
  $((View *) data, removeSubview, obj);
}

/**
 * @brief ArrayEnumerator to add Options to the stackView.
 */
static void addOption_addSubview(const Array *array, ident obj, ident data) {
  $((View *) data, addSubview, obj);
}

/**
 * @fn void Select::addOption(Select *self, const char *title, ident value)
 * @memberof Select
 */
static void addOption(Select *self, const char *title, ident value) {

  Option *option = $(alloc(Option), initWithTitle, title, value);
  assert(option);

  $(self->options, addObject, option);

  $((Array *) self->options, enumerateObjects, addOption_removeSubview, self->stackView);

  if (self->comparator) {
    $(self->options, sort, self->comparator);
  }

  $((Array *) self->options, enumerateObjects, addOption_addSubview, self->stackView);

  if (self->control.selection == ControlSelectionSingle) {
    if ($(self, selectedOption) == NULL) {
      $(option, setSelected, true);
    }
  }

  option->view.nextResponder = (View *) self;

  release(option);

  self->control.view.needsLayout = true;
}

/**
 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame)
 * @memberof Select
 */
static Select *initWithFrame(Select *self, const SDL_Rect *frame) {

  self = (Select *) super(Control, self, initWithFrame, frame);
  if (self) {

    self->control.selection = ControlSelectionSingle;

    self->options = $$(MutableArray, arrayWithCapacity, 8);
    assert(self->options);

    self->stackView = $(alloc(StackView), initWithFrame, NULL);
    assert(self->stackView);

    $((View *) self, addSubview, (View *) self->stackView);
  }

  return self;
}

/**
 * @brief Predicate function for optionWithValue.
 */
static bool optionWithValue_predicate(ident obj, ident data) {
  return ((Option *) obj)->value == data;
}

/**
 * @fn Option *Select::optionWithValue(const Select *self, ident value)
 * @memberof Select
 */
static Option *optionWithValue(const Select *self, const ident value) {
  return $((Array *) self->options, findObject, optionWithValue_predicate, value);
}

/**
 * @brief ArrayEnumerator for removeAllOptions.
 */
static void removeAllOptions_enumerate(const Array *array, ident obj, ident data) {
  $((Select *) data, removeOption, obj);
}

/**
 * @fn void Select::removeAllOptions(Select *self)
 * @memberof Select
 */
static void removeAllOptions(Select *self) {
  $((Array *) self->options, enumerateObjects, removeAllOptions_enumerate, self);
}

/**
 * @fn void Select::removeOption(Select *self, Option *option)
 * @memberof Select
 */
static void removeOption(Select *self, Option *option) {

  if ($((Array *) self->options, containsObject, option)) {

    $(self->options, removeObject, option);
    $((View *) self->stackView, removeSubview, (View *) option);

    if (self->control.selection == ControlSelectionSingle) {
      if ($(self, selectedOption) == NULL) {
        Option *first_option = $((Array *) self->options, firstObject);
        if (first_option) {
          $(self, selectOption, first_option);
        }
      }
    }

    self->control.view.needsLayout = true;
  }
}

/**
 * @fn void Select::removeOptionWithValue(Select *self, ident value)
 * @memberof Select
 */
static void removeOptionWithValue(Select *self, ident value) {

  Option *option = (Option *) $(self, optionWithValue, value);
  if (option) {
    $(self, removeOption, option);
  }
}

/**
 * @brief ArrayEnumerator for enforcing ControlSelectionSingle.
 */
static void selectOption_enumerate(const Array *array, ident obj, ident data) {
  $((Option *) obj, setSelected, false);
}

/**
 * @fn void Select::selectOption(Select *self, Option *option)
 * @memberof Select
 */
static void selectOption(Select *self, Option *option) {

  if (self->control.selection == ControlSelectionSingle) {
    $((Array *) self->options, enumerateObjects, selectOption_enumerate, NULL);
  }

  if (option) {

    if (option && option->isSelected == false) {
      $(option, setSelected, true);
    }
  }

  self->control.view.needsLayout = true;
}

/**
 * @fn void Select::selectOptionWithValue(Select *self, ident value)
 * @memberof Select
 */
static void selectOptionWithValue(Select *self, ident value) {

  Option *option = $(self, optionWithValue, value);

  $(self, selectOption, option);
}

/**
 * @brief Predicate for selectedOption and selectedOptions.
 */
static bool selectedOptions_predicate(ident obj, ident data) {
  return ((Option *) obj)->isSelected;
}

/**
 * @fn Option *Select::selectedOption(const Select *self)
 * @memberof Select
 */
static Option *selectedOption(const Select *self) {
  return $((Array *) self->options, findObject, selectedOptions_predicate, NULL);
}

/**
 * @fn Array *Select::selectedOptions(const Select *self)
 * @memberof Select
 */
static Array *selectedOptions(const Select *self) {
  return $((Array *) self->options, filteredArray, selectedOptions_predicate, NULL);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
  ((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;

  ((SelectInterface *) clazz->interface)->addOption = addOption;
  ((SelectInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((SelectInterface *) clazz->interface)->optionWithValue = optionWithValue;
  ((SelectInterface *) clazz->interface)->removeAllOptions = removeAllOptions;
  ((SelectInterface *) clazz->interface)->removeOption = removeOption;
  ((SelectInterface *) clazz->interface)->removeOptionWithValue = removeOptionWithValue;
  ((SelectInterface *) clazz->interface)->selectOption = selectOption;
  ((SelectInterface *) clazz->interface)->selectOptionWithValue = selectOptionWithValue;
  ((SelectInterface *) clazz->interface)->selectedOption = selectedOption;
  ((SelectInterface *) clazz->interface)->selectedOptions = selectedOptions;
}

/**
 * @fn Class *Select::_Select(void)
 * @memberof Select
 */
Class *_Select(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Select",
      .superclass = _Control(),
      .instanceSize = sizeof(Select),
      .interfaceOffset = offsetof(Select, interface),
      .interfaceSize = sizeof(SelectInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

