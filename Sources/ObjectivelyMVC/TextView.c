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
#include <stdlib.h>
#include <string.h>

#include <Objectively/String.h>

#include "Colors.h"
#include "Text.h"
#include "TextView.h"
#include "Theme.h"

#define _Class _TextView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TextView *this = (TextView *) self;

  memset(&this->delegate, 0, sizeof(this->delegate));

  free(this->defaultText);

  release(this->text);

  release(this->attributedText);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @fn void View::applyStyle(View *self, const Style *style)
 * @memberof View
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  TextView *this = (TextView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("editable", InletTypeBool, &this->isEditable, NULL)
  );

  $(self, bind, inlets, style->attributes);
}

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  TextView *this = (TextView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("defaultText", InletTypeCharacters, &this->defaultText, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::becomeKeyResponder
 */
static void becomeKeyResponder(View *self) {

  super(View, self, becomeKeyResponder);

  SDL_StartTextInput(self->window);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((TextView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  TextView *this = (TextView *) self;

  const char *text = this->attributedText->chars;

  if (text == NULL || strlen(text) == 0) {
    if ($((Control *) this, isFocused) == false) {
      text = this->defaultText;
    }
  }

  if (text == NULL) {
    $(this->text, setText, NULL);
  } else {
    if (this->text->text) {
      if (strcmp(text, this->text->text)) {
        $(this->text, setText, text);
      }
    } else {
      $(this->text, setText, text);
    }
  }

  $((View *) this->text, layoutIfNeeded);

  super(View, self, layoutSubviews);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

  super(View, self, render, renderer);

  TextView *this = (TextView *) self;

  if ($((Control *) this, isFocused)) {
    const char *text = this->text->text ?: "";

    // Measured as the Text renders it, escapes and icons included
    char *prefix = calloc(this->position + 1, sizeof(char));
    strncpy(prefix, text, this->position);

    SDL_Size size = $(this->text, sizeText, prefix);
    free(prefix);

    // An empty prefix has no height; borrow the line's
    if (size.h == 0) {
      size.h = $(this->text, sizeText, " ").h;
    }

    const int w = size.w, h = size.h;

    SDL_Rect frame = $((View *) this->text, renderFrame);

    const SDL_Point points[] = {
      { frame.x + w, frame.y },
      { frame.x + w, frame.y + h }
    };

    $(renderer, drawLine, points, &Colors.White);
  }
}

/**
 * @see View::resignKeyResponder
 */
static void resignKeyResponder(View *self) {

  super(View, self, resignKeyResponder);

  SDL_StopTextInput(self->window);
}

/**
 * @brief Returns the length in bytes of the cursor unit starting at `position`.
 * @details A unit is one UTF-8 encoded character, a color escape sequence (`^0` through `^9`,
 * or `^^`), or a registered icon escape (`:name:`), so that the cursor never lands inside one.
 */
static size_t unitLengthAt(const char *chars, size_t len, size_t position, const ImageAtlas *icons) {

  if (position >= len) {
    return 0;
  }

  if (chars[position] == '^' && position + 1 < len &&
      ((chars[position + 1] >= '0' && chars[position + 1] <= '9') || chars[position + 1] == '^')) {
    return 2;
  }

  if (chars[position] == ':') {
    const size_t n = MVC_IconEscapeLength(chars + position, icons, NULL);
    if (n && position + n <= len) {
      return n;
    }
  }

  size_t n = 1;
  while (position + n < len && (chars[position + n] & 0xC0) == 0x80) {
    n++;
  }

  return n;
}

/**
 * @brief Returns the length in bytes of the cursor unit ending at `position`.
 * @details Units are found by parsing forward from the start of the string, as they are
 * rendered: scanning backward would read the `1` in `^^1` as the escape `^1`.
 * @see unitLengthAt
 */
static size_t unitLengthBefore(const char *chars, size_t len, size_t position, const ImageAtlas *icons) {

  size_t unit = 0;

  for (size_t p = 0; p < position; p += unit) {
    unit = unitLengthAt(chars, len, p, icons);
    if (unit == 0) {
      break;
    }
  }

  return unit;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  bool didEdit = false, didCaptureEvent = false;

  View *view = (View *) self;

  TextView *this = (TextView *) self;

  if (this->isEditable) {
    if (event->type == SDL_EVENT_TEXT_INPUT) {
      if ($(self, isFocused)) {
        if (this->position == this->attributedText->length) {
          $(this->attributedText, appendCharacters, event->text.text);
        } else {
          $(this->attributedText, insertCharactersAtIndex, event->text.text, this->position);
        }
        this->position += strlen(event->text.text);
        didEdit = true;
        didCaptureEvent = true;
      }
    } else if (event->type == SDL_EVENT_KEY_DOWN) {
      didCaptureEvent = true;

      const char *chars = this->attributedText->chars;
      const size_t len = this->attributedText->length;

      Theme *theme = view->window ? $$(Theme, theme, view->window) : NULL;
      const ImageAtlas *icons = theme ? $(theme, icons) : NULL;

      switch (event->key.key) {

        case SDLK_ESCAPE:
        case SDLK_KP_ENTER:
        case SDLK_RETURN:
        case SDLK_TAB:
        case SDLK_KP_TAB:
          $(view, resignKeyResponder);
          break;

        case SDLK_BACKSPACE:
        case SDLK_KP_BACKSPACE:
          if (this->position > 0) {
            const size_t n = unitLengthBefore(chars, len, this->position, icons);
            const Range range = { .location = this->position - n, .length = n };
            $(this->attributedText, deleteCharactersInRange, range);
            this->position -= n;
            didEdit = true;
          }
          break;

        case SDLK_DELETE:
          if (this->position < len) {
            const Range range = { .location = this->position, .length = unitLengthAt(chars, len, this->position, icons) };
            $(this->attributedText, deleteCharactersInRange, range);
            didEdit = true;
          }
          break;

        case SDLK_LEFT:
          if (SDL_GetModState() & SDL_KMOD_CTRL) {
            while (this->position > 0 && chars[this->position] == ' ') {
              this->position -= unitLengthBefore(chars, len, this->position, icons);
            }
            while (this->position > 0 && chars[this->position] != ' ') {
              this->position -= unitLengthBefore(chars, len, this->position, icons);
            }
          } else {
            this->position -= unitLengthBefore(chars, len, this->position, icons);
          }
          break;

        case SDLK_RIGHT:
          if (SDL_GetModState() & SDL_KMOD_CTRL) {
            while (this->position < len && chars[this->position] == ' ') {
              this->position += unitLengthAt(chars, len, this->position, icons);
            }
            while (this->position < len && chars[this->position] != ' ') {
              this->position += unitLengthAt(chars, len, this->position, icons);
            }
            if (this->position < len) {
              this->position += unitLengthAt(chars, len, this->position, icons);
            }
          } else {
            this->position += unitLengthAt(chars, len, this->position, icons);
          }
          break;

        case SDLK_HOME:
          this->position = 0;
          break;

        case SDLK_END:
          this->position = len;
          break;

        case SDLK_A:
          if (SDL_GetModState() & SDL_KMOD_CTRL) {
            this->position = 0;
          }
          break;
        case SDLK_E:
          if (SDL_GetModState() & SDL_KMOD_CTRL) {
            this->position = len;
          }
          break;

        case SDLK_V:
          if ((SDL_GetModState() & (SDL_KMOD_CTRL | SDL_KMOD_GUI)) && SDL_HasClipboardText()) {
            char *text = SDL_GetClipboardText();
            if (text != NULL) {
              if (*text != '\0') {
                if (this->position == len) {
                  $(this->attributedText, appendCharacters, text);
                } else {
                  $(this->attributedText, insertCharactersAtIndex, text, this->position);
                }
                this->position += strlen(text);
                didEdit = true;
              }
              SDL_free(text);
            }
          }
          break;
      }
    }

    if (didEdit) {
      $((View *) self, setNeedsLayout);
      if (this->delegate.didEdit) {
        this->delegate.didEdit(this);
      }
      
      $((View *) self, emitViewEvent, ViewEventChange, NULL);
    }
  }

  if (didCaptureEvent) {
    return true;
  }
  
  return super(Control, self, captureEvent, event);
}

/**
 * @fn Control::stateDidChange(Control *)
 */
static void stateDidChange(Control *self) {

  TextView *this = (TextView *) self;

  if ($(self, isFocused)) {

    SDL_StartTextInput(self->view.window);

    if (this->delegate.didBeginEditing) {
      this->delegate.didBeginEditing(this);
    }
  } else {

    SDL_StopTextInput(self->view.window);

    if (this->delegate.didEndEditing) {
      this->delegate.didEndEditing(this);
    }
  }

  super(Control, self, stateDidChange);
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame)
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame) {

  self = (TextView *) super(Control, self, initWithFrame, frame);
  if (self) {
    self->attributedText = $$(String, string);
    assert(self->attributedText);

    self->isEditable = true;

    self->text = $(alloc(Text), initWithText, NULL, NULL);
    assert(self->text);

    $((View *) self, addSubview, (View *) self->text);

    self->control.view.clipsSubviews = true;
  }

  return self;
}

/**
 * @fn void TextView::setAttributedText(TextView *self, const char *attributedText)
 * @memberof TextView
 */
static void setAttributedText(TextView *self, const char *attributedText) {

  if (strcmp(self->attributedText->chars ?: "", attributedText ?: "")) {

    $(self->attributedText, setCharacters, attributedText);

    self->position = self->attributedText->length;

    $((View *) self, setNeedsLayout);
  }
}

/**
 * @fn void TextView::setDefaultText(TextView *self, const char *defaultText)
 * @memberof TextView
 */
static void setDefaultText(TextView *self, const char *defaultText) {

  if (strcmp(self->defaultText ?: "", defaultText ?: "")) {

    free(self->defaultText);

    if (defaultText) {
      self->defaultText = strdup(defaultText);
    } else {
      self->defaultText = NULL;
    }

    $((View *) self, setNeedsLayout);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->becomeKeyResponder = becomeKeyResponder;
  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->render = render;
  ((ViewInterface *) clazz->interface)->resignKeyResponder = resignKeyResponder;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
  ((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;

  ((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((TextViewInterface *) clazz->interface)->setAttributedText = setAttributedText;
  ((TextViewInterface *) clazz->interface)->setDefaultText = setDefaultText;
}

/**
 * @fn Class *TextView::_TextView(void)
 * @memberof TextView
 */
Class *_TextView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TextView",
      .superclass = _Control(),
      .instanceSize = sizeof(TextView),
      .interfaceSize = sizeof(TextViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
